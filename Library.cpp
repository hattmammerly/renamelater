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
