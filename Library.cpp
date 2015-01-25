/**
 * \file Library.cpp
 * \author Matt Hammerly
 */

#include <string>
#include <iostream>
#include "Library.h"

/**
 * \brief Default constructor
 *
 * Todo: sanitize the db credentials I guess lol
 */
CLibrary::CLibrary()
{
    char connectionString[512];
    snprintf(connectionString, 512, "dbname=%s host=%s user=%s password=%s",
             DBNAME, DBHOST, DBUSER, DBPW);

    mConnection = PQconnectdb(connectionString);

    if (PQstatus(mConnection) == CONNECTION_BAD)
    {
        std::cout << "Failed to connect to the database" << std::endl;
        exit(0);
    }
}

/**
 * \brief Destructor
 *
 * Will close the database connection before exiting
 */
CLibrary::~CLibrary()
{
    PQfinish(mConnection);
}

/**
 * \brief Create the database tables for this application
 *
 * \returns -1 if something goes wrong
 *
 * Todo: Write database schema in this comment
 *       `create trigger IF NOT EXISTS` or similar
 */
int CLibrary::PrepareDatabase()
{
    PGresult *res;
    res = PQexec(mConnection,
            "CREATE TABLE IF NOT EXISTS tracks (\
                id SERIAL NOT NULL PRIMARY KEY,\
                filepath TEXT NOT NULL,\
                date_added TIMESTAMPTZ NOT NULL DEFAULT NOW()\
          )");
    PQclear(res);

    res = PQexec(mConnection,
            "CREATE TABLE IF NOT EXISTS playlists (\
                id SERIAL NOT NULL PRIMARY KEY,\
                title TEXT NOT NULL,\
                length INTEGER NOT NULL DEFAULT 0\
          )");
    PQclear(res);

    res = PQexec(mConnection,
            "CREATE TABLE IF NOT EXISTS tracks_playlists (\
                id SERIAL NOT NULL PRIMARY KEY,\
                track_id INTEGER NOT NULL,\
                playlist_id INTEGER NOT NULL,\
                position FLOAT NOT NULL\
          )");
    PQclear(res);

    // Create a function to adjust the length of a playlist
    res = PQexec(mConnection,
            "CREATE OR REPLACE FUNCTION tracks_playlists_insert_func() RETURNS TRIGGER\
            LANGUAGE plpgsql\
            AS $tracks_playlists_insert_func$\
            DECLARE\
                pid INTEGER;\
            BEGIN\
                pid=0;\
                IF (TG_OP = 'INSERT') THEN pid = NEW.playlist_id;\
                ELSIF (TG_OP = 'DELETE') THEN pid = OLD.playlist_id;\
                END IF;\
                WITH Sub AS (SELECT COUNT(id) AS c FROM tracks_playlists WHERE playlist_id = pid)\
                    UPDATE playlists AS Main SET length = Sub.c FROM Sub WHERE Main.id = pid AND 1=1;\
                RETURN NULL;\
            END;\
            $tracks_playlists_insert_func$;");
    PQclear(res);

    // Create a trigger to adjust the length of a playlist on each insert or delete
    res = PQexec(mConnection,
            "CREATE TRIGGER tracks_playlists_insert_trg\
            AFTER INSERT OR DELETE ON tracks_playlists\
            FOR EACH ROW EXECUTE PROCEDURE tracks_playlists_insert_func();");
    PQclear(res);

    // Create a default playlist for all songs to be added to
    res = PQexec(mConnection, "INSERT INTO playlists (title) VALUES ('library')");
    PQclear(res);

    return 0;
}

/**
 * \brief Destroy all database objects used in this application
 *
 * \returns -1 if something goes wrong? I don't know what or why, frankly.
 */
int CLibrary::DestroyDatabase()
{
    PGresult *res;
    res = PQexec(mConnection, "DROP TRIGGER IF EXISTS tracks_playlists_insert_trg ON tracks_playlists;");
    PQclear(res);

    res = PQexec(mConnection, "DROP FUNCTION IF EXISTS tracks_playlists_insert_func() CASCADE;");
    PQclear(res);

    res = PQexec(mConnection, "DROP TABLE IF EXISTS tracks;");
    PQclear(res);

    res = PQexec(mConnection, "DROP TABLE IF EXISTS playlists;");
    PQclear(res);

    res = PQexec(mConnection, "DROP TABLE IF EXISTS tracks_playlists;");
    PQclear(res);

    return 0;
}

/**
 * \brief Exposes the database connection status
 * \returns PostgreSQL connection status object
 *
 * Just in case.
 */
ConnStatusType CLibrary::GetStatus()
{
    return PQstatus(mConnection);
}

/**
 * \brief Returns a pointer to the connection object
 *
 * It makes tests easier. Might make this protected and
 * require a testing subclass to use this later, whatever.
 */
PGconn* CLibrary::GetConnection()
{
    return mConnection;
}

/**
 * \brief Add a track to the database
 * \param filepath The filepath of the file to be added
 * \returns The ID of the new track (as a string)
 *
 * This method also adds the track to the all-library playlist created on database setup
 */
std::string CLibrary::AddTrack(std::string filepath)
{
    std::string query = "INSERT INTO tracks (filepath) VALUES (";

    // Safety first
    char *escaped_filepath = PQescapeLiteral(mConnection, filepath.c_str(), filepath.length());
    query.append(escaped_filepath);
    PQfreemem(escaped_filepath);

    query.append(") RETURNING id");

    PGresult *res = PQexec(mConnection, query.c_str());
    char *id = PQgetvalue(res, 0, 0);

    // Create an std::string to return so we can appropriately free the PGresult
    std::string std_id(id);
    PQclear(res);

    // Add this track to the all-library playlist created on database setup
    std::string query2 = "INSERT INTO tracks_playlists (track_id, playlist_id, position) SELECT " + std_id + ", 1, COALESCE(MAX(position), 0) + 1 FROM tracks_playlists RETURNING id";
    res = PQexec(mConnection, query2.c_str());
    std::cout << query2 + " ";
    PQclear(res);

    return std_id;
}

/**
 * \brief Add a playlist to the database
 * \param title The title of the playlist to be added
 * \returns The ID of the new playlist (as a string)
 */
std::string CLibrary::AddPlaylist(std::string title)
{
    std::string query = "INSERT INTO playlists (title) VALUES (";

    // Safety first
    char *escaped_title = PQescapeLiteral(mConnection, title.c_str(), title.length());
    query.append(escaped_title);
    PQfreemem(escaped_title);

    query.append(") RETURNING id");

    PGresult *res = PQexec(mConnection, query.c_str());
    char *id = PQgetvalue(res, 0, 0);

    // Create an std::string to return so we can appropriately free the PGresult
    std::string std_id(id);
    PQclear(res);

    return std_id;
}

/**
 * \brief Removes a track from the database, and all records of the track's playlist membership
 * \param id ID of the track to be deleted
 * \returns ID of the deleted track
 */
std::string CLibrary::RemoveTrack(std::string id)
{
    std::string query = "DELETE FROM tracks WHERE id=";

    char escaped_id[30];
    PQescapeStringConn(mConnection, escaped_id, id.c_str(), 30, 0);
    query.append(escaped_id);
    query.append("; DELETE FROM tracks_playlists WHERE track_id=");
    query.append(escaped_id);

    PGresult *res = PQexec(mConnection, query.c_str());
    PQclear(res);

    return id;
}

/**
 * \brief Removes a playlist from the database, and all records of membership in the playlist
 * \param id ID of the playlist to be deleted
 * \returns ID of the deleted playlist
 */
std::string CLibrary::RemovePlaylist(std::string id)
{
    std::string query = "DELETE FROM tracks WHERE id=";

    char escaped_id[30];
    PQescapeStringConn(mConnection, escaped_id, id.c_str(), 30, 0);
    query.append(escaped_id);
    query.append("; DELETE FROM tracks_playlists WHERE playlist_id=");
    query.append(escaped_id);

    PGresult *res = PQexec(mConnection, query.c_str());
    PQclear(res);

    return id;
}
