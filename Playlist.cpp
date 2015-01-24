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
