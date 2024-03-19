// File: NoSQLDatabase.h

#ifndef NOSQL_DATABASE_H
#define NOSQL_DATABASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <unordered_map>
#include "BTree.h"

using namespace std;

// Define constants for file system parameters
constexpr int BLOCK_SIZE = 256;           // Block size in bytes
constexpr int INITIAL_SIZE = 1024 * 1024; // Initial size of database file in bytes (1024 Mbytes)
constexpr int DATA_RECORD_SIZE = 40;           // Size of each record in bytes (key + value)
constexpr int CHILD_BLOCK_SIZE = 8;              // Size Child block
constexpr int INDEX_BLOCK_SIZE = 10 + 8;         // Size Index block (key, block #)

// Define structures for File Control Block (FCB) and directory entry
struct FCB
{
    string filename;
    int fileSize;     // Number of blocks used
    time_t timestamp; // Last modified time
    string filetype;  // Data file or index file
    int startBlock;
    // Add other necessary fields such as create time, create date, etc.
};

struct DirectoryEntry
{
    FCB fcb;
    // Add other necessary fields
};

class NoSQLDatabase
{
private:
    string databaseName;
    fstream databaseFile;
    vector<DirectoryEntry> directory;
    BTree bTree; // include BTree for indexing

    // Other private members for block management, file operations, etc.

    enum Command
    {
        OPEN,
        PUT,
        GET,
        RM,
        DIR,
        FIND,
        KILL,
        QUIT,
        INVALID
    };

    Command getCommandType(const string &command);

public:
    // constructor of NoSQLDatabase
    NoSQLDatabase();
    ~NoSQLDatabase();

    // File system operations
    // command: operations to perform.

    // open PFSfile: Allocate a new 1 MByte "PFS" file if it does not already exist. If it does exist, begin using it for further commands.
    void openOrCreateDatabase(string &PFSFile);
    // put myFile: Insert data from your OS file, i.e., "myfile" into your NoSQL database, i.e., PFS file.
    void putDataIntoDatabase(string &myFile);
    // get myFile: Download data file "myfile" from your NoSQL database, i.e., PFS file, and save it to the current OS directory.
    void getDataFromDatabase();
    // rm myFile: Delete "myfile" from your NoSQL database, i.e. PFS file.
    void delFileFromDatabase();
    // dir: List all data files in your NoSQL database, i.e., PFS file.
    void listAllDataFromDatabase();
    // find [myfile.key]: Find ‘value’ using a given ‘key’, i.e., return a record which is associate with a key from ‘myfile’ in your NoSQL database. In addition, you need to show how many blocks are accessed during ‘find’
    void findValueFromDatabase();
    // kill PFSfile: Delete the NoSQL database, i.e., remove PFSfile from OS file system
    void killDatabase();
    // quit: exit NoSQL database
    void quitDatabase();

    // Index operations

    // Index operations using B-tree
    void insertIntoBTree(int key);
    bool searchInBTree(int key);

    // Free block management
    // Add functions for bit map vector approach

    // Command-line interface
    void runCLI();
};

#endif // NOSQL_DATABASE_H
