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

/// File location of one track
const std::string track1 = "/home/matt/hdd/dev/cpp/musicmanager/test_data/Gregory And The Hawk/The Boats & Birds EP/01 Boats & Birds.mp3";

/// Title of one playlist
const std::string playlist1 = "test1";

/**
 * \brief Main entry point of program, where tests will be run
 */
int main()
{

    Test_LibraryConstructor();

    Test_PrepareDatabase();

    Test_DestroyDatabase();
    
    Test_AddTrack();

    Test_AddPlaylist();

    // So I can poke around manually after running tests
    //CLibrary library;
    //library.PrepareDatabase();

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
    PQclear(res_tables);

    // Check to see if the tracks_playlists_insert_func procedure exists
    PGresult* res_func = PQexec(conn, "SELECT routine_name FROM information_schema.routines WHERE routine_name = 'tracks_playlists_insert_func';");
    assert(PQntuples(res_func) == 1);
    PQclear(res_func);

    // Check to see if the tracks_playlists_insert_trg trigger exists
    // Two records should be returned; one for ON INSERT, one for ON DELETE
    PGresult* res_trg = PQexec(conn, "SELECT trigger_name FROM information_schema.triggers WHERE trigger_name = 'tracks_playlists_insert_trg';");
    assert(PQntuples(res_trg) == 2);
    PQclear(res_trg);

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
    PQclear(res_tables);

    // Check to see if the tracks_playlists_insert_func procedure exists
    PGresult* res_func = PQexec(conn, "SELECT routine_name FROM information_schema.routines WHERE routine_name = 'tracks_playlists_insert_func';");
    assert(PQntuples(res_func) == 0);
    PQclear(res_func);

    // Check to see if the tracks_playlists_insert_trg trigger exists
    PGresult* res_trg = PQexec(conn, "SELECT trigger_name FROM information_schema.triggers WHERE trigger_name = 'tracks_playlists_insert_trg';");
    assert(PQntuples(res_trg) == 0);
    PQclear(res_trg);


    cout << "OK" << endl;

}

/**
 * \brief Ensure tracks can be added properly
 *
 * Maybe later I'll figure out how to verify that the date is correct
 */
void Test_AddTrack()
{
    cout << "Test_AddTrack... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    std::string id = library.AddTrack(track1);

    // We need the unwrapped connection object for arbitrary queries to test
    PGconn* conn = library.GetConnection();

    std::string query = "SELECT * FROM tracks WHERE id=";

    // id should never not be a numeric string but safety first
    char escaped_id[30];
    PQescapeStringConn(conn, escaped_id, id.c_str(), 30, 0);
    query.append(escaped_id);

    PGresult* res = PQexec(conn, query.c_str());

    // Is there actually a row inserted with our id?
    std::string new_id(PQgetvalue(res, 0, 0));
    assert(new_id == id);

    // Is it the same one (same filepath) that we entered?
    std::string filepath(PQgetvalue(res, 0, 1));
    assert(filepath == track1);

    PQclear(res);

    library.DestroyDatabase();

    cout << "OK" << endl;
}

/**
 * \brief Ensure playlists can be added properly
 */
void Test_AddPlaylist()
{
    cout << "Test_AddPlaylist... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    std::string id = library.AddPlaylist(playlist1);
    
    // We need the unwrapped connection object for arbitrary queries to test
    PGconn *conn = library.GetConnection();

    std::string query = "SELECT * FROM playlists WHERE id=";

    // id should never not be a numeric string but safety first
    char escaped_id[30];
    PQescapeStringConn(conn, escaped_id, id.c_str(), 30, 0);
    query.append(escaped_id);

    PGresult *res = PQexec(conn, query.c_str());

    // Is there actually a row inserted with our id?
    std::string new_id(PQgetvalue(res, 0, 0));
    assert(new_id == id);

    // Is it the same one (same title) that we entered?
    std::string title(PQgetvalue(res, 0, 1));
    assert(title == playlist1);

    PQclear(res);

    library.DestroyDatabase();

    cout << "OK" << endl;
}
