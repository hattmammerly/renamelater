/**
 * \file Playlist.h
 * \author Matt Hammerly
 * \brief Contains the definition of the Playlist class
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include "Library.h"

/**
 * \brief This class will represent a music playlist
 *
 * Dealing with the database directly is ugly, so we'll use
 * these when we can. Also allows temporary playlists to be
 * created without having to write to the database (which
 * really isn't that hard in any case).
 */
class CPlaylist
{
public:

    /** \brief Default constructor (disabled) */
    CPlaylist() = delete;

    CPlaylist(CLibrary *library);
    CPlaylist(CLibrary *library, std::string);

    /** \brief Copy constructor (disabled)
     * \param playlist Playlist to construct this based on */
    CPlaylist(const CPlaylist &playlist) = delete;

    /** \brief Assignment operator (disabled)
     * \param playlist Playlist whose attributes will override those of the current playlist */
    CPlaylist& operator=(const CPlaylist &playlist) = delete;

    /** \brief Destructor */
    ~CPlaylist() {}

private:
    /// The id of the playlist in the database
    std::string mId;

    /// The title of the playlist
    std::string mTitle;

    /// The length of the playlist
    std::string mLength;

    /// The library this playlist belongs to
    CLibrary *mLibrary;
};

#endif
