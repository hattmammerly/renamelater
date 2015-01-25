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

/// File location of another track
const std::string track2 = "/home/matt/hdd/dev/cpp/musicmanager/test_data/Beirut/The Flying Club Cup/05 la banlieue.m4a";

/// Title of one playlist
const std::string playlist1 = "test1";

/**
 * \brief Main entry point of program, where tests will be run
 */
int main()
{

    Test_Library_Constructor();

    Test_Library_PrepareDatabase();

    Test_Library_DestroyDatabase();
    
    Test_Library_AddTrack();

    Test_Library_AddPlaylist();

    Test_Library_RemoveTrack();

    Test_Library_RemovePlaylist();

    Test_Playlist_Constructors();

    Test_Playlist_AppendTrack();

    // So I can poke around manually after running tests
    //CLibrary library;
    //library.PrepareDatabase();

    return 0;
}

/**
 * \brief I mean, there's constructor logic, let's test it.
 */
void Test_Library_Constructor()
{
    cout << "Test_Library_Constructor... ";
    CLibrary library;
    assert(library.GetStatus() == CONNECTION_OK);
    cout << "OK" << endl;
}

/**
 * \brief Ensure all expected database objects are created
 */
void Test_Library_PrepareDatabase()
{
    cout << "Test_Library_PrepareDatabase... ";
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

    // Check to see if the default library playlist was properly created
    PGresult *res_playlist = PQexec(conn, "SELECT id, title, length FROM playlists WHERE id=1");
    assert(PQntuples(res_playlist) == 1);
    PQclear(res_playlist);

    // clean up, I guess
    library.DestroyDatabase();

    cout << "OK" << endl;
}

/**
 * \brief Ensure database objects are deleted when asked
 */
void Test_Library_DestroyDatabase()
{
    cout << "Test_Library_DestroyDatabase... ";
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
void Test_Library_AddTrack()
{
    cout << "Test_Library_AddTrack... ";
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
void Test_Library_AddPlaylist()
{
    cout << "Test_Library_AddPlaylist... ";
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

/**
 * \brief Ensure tracks are removed from the database properly
 * This includes removal from all playlists they have been added to
 */
void Test_Library_RemoveTrack()
{
    cout << "Test_Library_RemoveTrack... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    std::string track1_id = library.AddTrack(track1);
    std::string track2_id = library.AddTrack(track2);

    std::string playlist_id = library.AddPlaylist("test");
    CPlaylist playlist(&library, playlist_id);
    playlist.AppendTrack(track1_id);
    playlist.AppendTrack(track2_id);

    PGconn *conn = library.GetConnection();

    std::string query = "SELECT * FROM tracks_playlists WHERE track_id = ";
    char escaped_track1_id[30];
    char escaped_track2_id[30];
    PQescapeStringConn(conn, escaped_track1_id, track1_id.c_str(), 30, 0);
    PQescapeStringConn(conn, escaped_track2_id, track2_id.c_str(), 30, 0);

    std::string query1 = query + escaped_track1_id;
    std::string query2 = query + escaped_track2_id;
    PGresult *res1 = PQexec(conn, query1.c_str());
    PGresult *res2 = PQexec(conn, query2.c_str());

    assert(PQntuples(res1) == 2);
    assert(PQntuples(res2) == 2);

    PQclear(res1);
    PQclear(res2);

    library.RemoveTrack(track1_id);
    library.RemoveTrack(track2_id);

    std::string query3 = "SELECT * FROM tracks WHERE id = ";
    query3.append(escaped_track1_id);
    std::string query4 = "SELECT * FROM tracks WHERE id = ";
    query3.append(escaped_track2_id);
    PGresult *res3 = PQexec(conn, query3.c_str());
    PGresult *res4 = PQexec(conn, query4.c_str());

    res1 = PQexec(conn, query1.c_str());
    res2 = PQexec(conn, query2.c_str());

    assert(PQntuples(res1) == 0);
    assert(PQntuples(res2) == 0);
    assert(PQntuples(res3) == 0);
    assert(PQntuples(res4) == 0);

    PQclear(res1);
    PQclear(res2);
    PQclear(res3);
    PQclear(res4);

    library.DestroyDatabase();

    cout << "OK" << endl;
}

void Test_Library_RemovePlaylist()
{
    cout << "Test_Library_RemovePlaylist... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    std::string track1_id = library.AddTrack(track1);
    std::string track2_id = library.AddTrack(track2);

    std::string playlist_id = library.AddPlaylist("test");
    CPlaylist playlist(&library, playlist_id);
    playlist.AppendTrack(track1_id);
    playlist.AppendTrack(track2_id);

    PGconn *conn = library.GetConnection();

    std::string query = "SELECT * FROM tracks_playlists WHERE playlist_id = ";
    char escaped_playlist_id[30];
    PQescapeStringConn(conn, escaped_playlist_id, playlist_id.c_str(), 30, 0);

    query.append(escaped_playlist_id);
    PGresult *res = PQexec(conn, query.c_str());

    assert(PQntuples(res) == 2);

    PQclear(res);

    library.RemovePlaylist(playlist_id);

    res = PQexec(conn, query.c_str());

    assert(PQntuples(res) == 0);

    PQclear(res);

    library.DestroyDatabase();

    cout << "OK" << endl;
}

/**
 * \brief Ensure playlist objects are constructed properly
 *
 * Note: This isn't done. Verify that non-empty playlists are properly
 * populated with Track objects when the Track class is implemented
 */
void Test_Playlist_Constructors()
{
    cout << "Test_Playlist_Constructors... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    // Should create an empty playlist
    CPlaylist playlist(&library);
    assert(playlist.GetId() == "temp");
    assert(playlist.GetTitle() == "working playlist");
    assert(playlist.GetLength() == "0");
    assert(playlist.GetLibrary() == &library);

    // Should create an object for the default playlist in db
    CPlaylist playlist2(&library, "1");
    assert(playlist2.GetId() == "1");
    assert(playlist2.GetTitle() == "library");
    assert(playlist2.GetLength() == "0");
    assert(playlist2.GetLibrary() == &library);
    
    library.DestroyDatabase();

    cout << "OK" << endl;
}

/**
 * \brief Ensure tracks can be properly appended to playlists
 *
 * Note: This isn't done. Verify that Track objects are properly appended
 * to their containers when the Track Class is implemented
 */
void Test_Playlist_AppendTrack()
{
    cout << "Test_Playlist_AppendTrack... ";
    CLibrary library;

    // Make sure all tables and such exist
    library.PrepareDatabase();

    // Adding tracks that can be added to playlists
    std::string track1_id = library.AddTrack(track1);
    std::string track2_id = library.AddTrack(track2);

    CPlaylist db_playlist(&library, "1"); // a playlist from the database
    CPlaylist temp_playlist(&library);    // a temp playlist not in the database

    std::string association1 = db_playlist.AppendTrack(track1_id);
    std::string association2 = db_playlist.AppendTrack(track2_id);

    std::string association3 = temp_playlist.AppendTrack(track1_id);
    std::string association4 = temp_playlist.AppendTrack(track2_id);

    assert(association3 == "temp");
    assert(association4 == "temp");

    // Insert logic to make sure Track objects were appropriately added to the container
    // for both playlists

    // Verify that the proper database records were created
    if (db_playlist.GetId() != "temp")
    {
        PGconn *conn = library.GetConnection();

        std::string query = "SELECT id, playlist_id, track_id, position FROM tracks_playlists WHERE playlist_id = ";

        char escaped_playlist_id[30];
        PQescapeStringConn(conn, escaped_playlist_id, db_playlist.GetId().c_str(), 30, 0);

        query.append(escaped_playlist_id);

        PGresult *res = PQexec(conn, query.c_str());

        // 0 and 1 should be auto-created when tracks are added
        // 2 and 3 are the new records we just added
        std::string association1_id(PQgetvalue(res, 2, 0));
        std::string association1_playlist_id(PQgetvalue(res, 2, 1));
        std::string association1_track_id(PQgetvalue(res, 2, 2));
        std::string association1_position(PQgetvalue(res, 2, 3));
        std::string association2_id(PQgetvalue(res, 3, 0));
        std::string association2_playlist_id(PQgetvalue(res, 3, 1));
        std::string association2_track_id(PQgetvalue(res, 3, 2));
        std::string association2_position(PQgetvalue(res, 3, 3));

        assert(association1 == association1_id);
        assert(db_playlist.GetId() == association1_playlist_id);
        assert(track1_id == association1_track_id);
        assert("3" == association1_position);
        assert(association2 == association2_id);
        assert(db_playlist.GetId() == association2_playlist_id);
        assert(track2_id == association2_track_id);
        assert("4" == association2_position);
        
        PQclear(res);
    }
    else
    {
        assert(association1 == "temp");
        assert(association2 == "temp");
    }

    library.DestroyDatabase();

    cout << "OK" << endl;
}
