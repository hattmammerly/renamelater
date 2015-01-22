/**
 * \file Library.h
 * \author Matt Hammerly
 * \brief Contains the definition of the Library class
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include <postgresql/libpq-fe.h>
#include "config.h"

/**
 * \brief This class will talk to postgres so you don't have to
 *
 * This class will handle the plumbing in managing the library
 * so there isn't hideous handwritten sql and old c library
 * use dirtying up the rest of our codebase.
 */
class CLibrary
{
public:

    CLibrary();
    ~CLibrary();

    /** \brief Copy constructor (disabled)
     * \param library Library to construct this based on */
    CLibrary(const CLibrary &library) = delete;

    /** \brief Assignment operator (disabled)
     * \param library Library whose attributes will override those of the current library */
    CLibrary& operator=(const CLibrary &library) = delete;

    int PrepareDatabase();
    int DestroyDatabase();

    ConnStatusType GetStatus();

    PGconn* GetConnection();

    std::string AddTrack(std::string filepath);

    std::string AddPlaylist(std::string title);

    std::string RemovePlaylist(std::string id);

private:
    PGconn *mConnection;                ///< Postgres database connection struct

};

#endif
