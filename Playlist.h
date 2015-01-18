/**
 * \file Playlist.h
 * \author Matt Hammerly
 * \brief Contains the definition of the Playlist class
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

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

    /** \brief Copy constructor (disabled)
     * \param playlist Playlist to construct this based on */
    CPlaylist(const CPlaylist &playlist) = delete;

    /** \brief Assignment operator (disabled)
     * \param playlist Playlist whose attributes will override those of the current playlist */
    CPlaylist& operator=(const CPlaylist &playlist) = delete;

    /** \brief Destructor */
    ~CPlaylist() {}
};

#endif
