/**
 * \file tests.cpp
 * \author Matt Hammerly
 * \brief This file contains int main() which will run tests as they're written
 */
#include "Library.h"
#include "Track.h"
#include "Playlist.h"

/**
 * \brief Main entry point of program, where tests will be run
 */
int main()
{
    // Create a library
    CLibrary library;

    library.DestroyDatabase();
    library.PrepareDatabase();

    return 0;
}

