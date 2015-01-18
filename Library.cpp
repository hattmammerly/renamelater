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
 */
int CLibrary::PrepareDatabase()
{
    PQexec(mConnection,
            "CREATE TABLE IF NOT EXISTS tracks (\
                id SERIAL NOT NULL PRIMARY KEY,\
                location TEXT NOT NULL,\
                flag INTEGER NOT NULL DEFAULT 0\
          )");
    PQexec(mConnection,
            "CREATE TABLE IF NOT EXISTS playlists (\
                id SERIAL NOT NULL PRIMARY KEY,\
                title TEXT NOT NULL,\
                length INTEGER NOT NULL,\
                flag INTEGER NOT NULL DEFAULT 0\
          )");
    PQexec(mConnection,
            "CREATE TABLE IF NOT EXISTS tracks_playlists (\
                id SERIAL NOT NULL PRIMARY KEY,\
                track_id INTEGER NOT NULL,\
                playlist_id INTEGER NOT NULL,\
                position FLOAT NOT NULL\
          )");

    // Create a function to adjust the length of a playlist
    // Doesn't actually work yet
    PQexec(mConnection,
            "CREATE FUNCTION IF NOT EXISTS tracks_playlists_insert_func() RETURNS TRIGGER\
            LANGUAGE plpgsql\
            AS $tracks_playlists_insert_func$\
            BEGIN\
                WITH Sub AS (SELECT COUNT(id) AS c FROM tracks_playlists WHERE playlist_id = NEW.playlist_id)\
                    UPDATE playlists AS Main SET length = Sub.c FROM Sub WHERE Main.id = NEW.playlist_id;\
                RETURN NULL;\
            END;\
            $tracks_playlists_insert_func$;");

    // Trigger to adjust the length of a playlist on each insert/remove
    PQexec(mConnection,
            "CREATE TRIGGER IF NOT EXISTS tracks_playlists_insert_trg\
            AFTER INSERT OR DELETE ON tracks_playlists\
            FOR EACH ROW EXECUTE PROCEDURE tracks_playlists_insert_func();");


    return 0;
}

/**
 * \brief Destroy all database objects used in this application
 *
 * \returns -1 if something goes wrong? I don't know what or why, frankly.
 */
int CLibrary::DestroyDatabase()
{

    PQexec(mConnection, "DROP TABLE IF EXISTS tracks;");
    PQexec(mConnection, "DROP TABLE IF EXISTS playlists;");
    PQexec(mConnection, "DROP TABLE IF EXISTS tracks_playlists;");
    PQexec(mConnection, "DROP FUNCTION IF EXISTS tracks_playlists_insert_func;");
    PQexec(mConnection, "DROP TRIGGER IF EXISTS tracks_playlists_insert_trg;");

    return 0;
}
