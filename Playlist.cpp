/**
 * \file Playlist.h
 * \author Matt Hammerly
 */

#include "Playlist.h"

/**
 * \brief Constructor for a playlist not in the database
 * \param library Pointer to the library this playlist belongs to
 */
CPlaylist::CPlaylist(CLibrary *library)
{
    mLibrary = library;
    mId = "temp";
    mTitle = "working playlist";
    mLength = "0";

    // Fill in any logic necessary to initialize an empty container for tracks
}

/**
 * \brief Constructor for a playlist to be fetched from the database
 * \param library Pointer to the library this playlist belongs to
 * \param id Id of the playlist in the database
 */
CPlaylist::CPlaylist(CLibrary *library, std::string id)
{
    mLibrary = library;
    mId = id;

    // We need to fetch the playlist data from the database
    PGconn *conn = mLibrary->GetConnection();

    std::string query = "SELECT id, title, length FROM playlists WHERE id=";

    char escaped_id[30];
    PQescapeStringConn(conn, escaped_id, mId.c_str(), 30, 0);
    query.append(escaped_id);
    

    PGresult *res = PQexec(conn, query.c_str());

    mTitle = PQgetvalue(res, 0, 1);
    mLength = PQgetvalue(res, 0, 2);

    PQclear(res);

    // Fill in any logic necessary to populate a container with associated tracks
}

/**
 * \brief Append a track to a playlist
 * \param id The database ID of the track to append
 * \returns ID of the association record, or "temp" if a temp playlist
 */
std::string CPlaylist::AppendTrack(std::string id)
{
    // Fill in any logic necessary to append a Track object to this playlist's container

    if (mId != "temp")
    {
        PGconn* conn = mLibrary->GetConnection();

        char escaped_playlist_id[30];
        PQescapeStringConn(conn, escaped_playlist_id, mId.c_str(), 30, 0);
        char escaped_track_id[30];
        PQescapeStringConn(conn, escaped_track_id, id.c_str(), 30, 0);

        std::string query = "INSERT INTO tracks_playlists (playlist_id, track_id, position) SELECT ";
        query.append(escaped_playlist_id);
        query.append(", ");
        query.append(escaped_track_id);
        query.append(", COALESCE(MAX(position), 0) + 1 FROM tracks_playlists WHERE playlist_id=");
        query.append(escaped_playlist_id);
        query.append(" RETURNING id");

        PGresult *res = PQexec(conn, query.c_str());
        
        std::string associationId(PQgetvalue(res, 0, 0));
        PQclear(res);

        return associationId;
    }
    else {  // No association was created; this is for a temporary playlist
        return "temp";
    }
}

/**
 * \brief Insert a track into a playlist
 * \param id ID of the track to insert
 * \param position Index you want the track to occupy, as a string
 * \returns ID of the association record, or "temp" if a temp playlist
 */
std::string CPlaylist::InsertTrack(std::string id, std::string position)
{
    // Fill in any logic necessary to insert a Track object into this playlist's container

    if (mId != "temp")
    {
        PGconn *conn = mLibrary->GetConnection();

        char escaped_playlist_id[30];
        PQescapeStringConn(conn, escaped_playlist_id, mId.c_str(), 30, 0);
        char escaped_track_id[30];
        PQescapeStringConn(conn, escaped_track_id, id.c_str(), 30, 0);
        char escaped_position[30];
        PQescapeStringConn(conn, escaped_position, position.c_str(), 30, 0);

        std::string query = "INSERT INTO tracks_playlists (playlist_id, track_id, position) VALUES (";
        query.append(escaped_playlist_id);
        query.append(", ");
        query.append(escaped_track_id);
        query.append(", ");
        query.append(escaped_position);
        query.append(" - 0.5) RETURNING id"); // subtract 0.5

        PGresult *res = PQexec(conn, query.c_str());

        std::string associationId(PQgetvalue(res, 0, 0));
        PQclear(res);

        // We inserted a fraction, and now we want to correct all records to have integer positions
        Normalize();

        return associationId;
    }

    return "temp";
}

/**
 * \brief Normalize a playlist's positions to be all integers
 *
 * Inserts are done by inserting fractions into the position column,
 * and this function will normalize a playlist so all positions are
 * integers.
 */
void CPlaylist::Normalize()
{
    if (mId != "temp")
    {
        PGconn *conn = mLibrary->GetConnection();

        char escaped_id[30];
        PQescapeStringConn(conn, escaped_id, mId.c_str(), 30, 0);

        std::string query = "WITH Sub AS (SELECT id, row_number() OVER (ORDER BY position) FROM tracks_playlists WHERE playlist_id=";
        query.append(escaped_id);
        query.append(") UPDATE tracks_playlists AS Main SET position = Sub.row_number FROM Sub WHERE Main.id = Sub.id");

        PGresult *res = PQexec(conn, query.c_str());
        PQclear(res);
    }

    return;
}

void CPlaylist::RemoveTrack(std::string position)
{
    // Fill in any logic necessary to remove a track from the collection of Track objects

    if (mId != "temp")
    {
        PGconn *conn = mLibrary->GetConnection();

        char escaped_id[30];
        PQescapeStringConn(conn, escaped_id, mId.c_str(), 30, 0);
        char escaped_position[30];
        PQescapeStringConn(conn, escaped_position, position.c_str(), 30, 0);

        std::string query = "DELETE FROM tracks_playlists WHERE position=";
        query.append(escaped_position);
        query.append(" AND playlist_id=");
        query.append(escaped_id);

        PGresult *res = PQexec(conn, query.c_str());
        PQclear(res);
    }
}
