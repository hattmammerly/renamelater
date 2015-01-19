/**
 * \file tests.cpp
 * \author Matt Hammerly
 * \brief This file contains int main() which will run tests as they're written
 */
#include <iostream>
#include "Library.h"
#include "Track.h"
#include "Playlist.h"
#include "tests.h"

using std::cout; using std::endl;

/**
 * \brief Main entry point of program, where tests will be run
 */
int main()
{

    Test_Library_Constructor();

    return 0;
}

/**
 * \brief I mean, there's constructor logic, let's test it.
 */
void Test_LibraryConstructor()
{
    cout << "Test_LibraryConstructor... ";
    CLibrary library;
    assert(library.GetStatus() == CONNECTION_OK);
    cout << "OK" << endl;
}

