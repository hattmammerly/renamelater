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

    /**
     * \brief Returns the ID of this playlist
     * \returns ID as a string
     */
    std::string GetId() { return mId; }

    /**
     * \brief Returns the title of this playlist
     * \returns Title as a string
     */
    std::string GetTitle() { return mTitle; }

    /**
     * \brief Returns the length of this playlist
     * \returns Playlist length as a string
     */
    std::string GetLength() { return mLength; }

    /**
     * \brief Returns the library this playlist belongs to
     * \returns Pointer to library object
     */
    CLibrary *GetLibrary() { return mLibrary; }

    std::string AppendTrack(std::string id);

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
