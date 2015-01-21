/**
 * \file tests.cpp
 * \author Matt Hammerly
 * \brief This file contains int main() which will run tests as they're written
 */
#include <iostream>
#include <cassert>
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

    Test_LibraryConstructor();

    Test_PrepareDatabase();

    Test_DestroyDatabase();

    // So I can poke around manually after running tests
    CLibrary library;
    library.PrepareDatabase();

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

/**
 * \brief Ensure all expected database objects are created
 */
void Test_PrepareDatabase()
{
    cout << "Test_PrepareDatabase... ";
    CLibrary library;

    library.PrepareDatabase();

    // We need the unwrapped connection object for arbitrary queries to test
    PGconn *conn = library.GetConnection();

    // Check to see if tables exist
    // Three rows should be returned; one for tracks, one for playlists, one for tracks_playlists
    PGresult* res_tables = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_name IN ('tracks', 'playlists', 'tracks_playlists');");
    assert(PQntuples(res_tables) == 3);

    // Check to see if the tracks_playlists_insert_func procedure exists
    PGresult* res_func = PQexec(conn, "SELECT routine_name FROM information_schema.routines WHERE routine_name = 'tracks_playlists_insert_func';");
    assert(PQntuples(res_func) == 1);

    // Check to see if the tracks_playlists_insert_trg trigger exists
    // Two records should be returned; one for ON INSERT, one for ON DELETE
    PGresult* res_trg = PQexec(conn, "SELECT trigger_name FROM information_schema.triggers WHERE trigger_name = 'tracks_playlists_insert_trg';");
    assert(PQntuples(res_trg) == 2);

    // clean up, I guess
    library.DestroyDatabase();

    cout << "OK" << endl;
}

/**
 * \brief Ensure database objects are deleted when asked
 */
void Test_DestroyDatabase()
{
    cout << "Test_DestroyDatabase... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    library.DestroyDatabase();

    // We need the unwrapped connection object for arbitrary queries to test
    PGconn *conn = library.GetConnection();

    // Check to see if tables exist
    PGresult* res_tables = PQexec(conn, "SELECT table_name FROM information_schema.tables WHERE table_name IN ('tracks', 'playlists', 'tracks_playlists');");
    assert(PQntuples(res_tables) == 0);

    // Check to see if the tracks_playlists_insert_func procedure exists
    PGresult* res_func = PQexec(conn, "SELECT routine_name FROM information_schema.routines WHERE routine_name = 'tracks_playlists_insert_func';");
    assert(PQntuples(res_func) == 0);

    // Check to see if the tracks_playlists_insert_trg trigger exists
    PGresult* res_trg = PQexec(conn, "SELECT trigger_name FROM information_schema.triggers WHERE trigger_name = 'tracks_playlists_insert_trg';");
    assert(PQntuples(res_trg) == 0);


    cout << "OK" << endl;

}
