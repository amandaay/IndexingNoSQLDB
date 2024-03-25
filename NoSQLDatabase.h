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
#include <cstdint>
using namespace std;

// Define constants for file system parameters
constexpr int BLOCK_SIZE = 256;           // Block size in bytes
constexpr int INITIAL_SIZE = 1024 * 1024; // Initial size of database file in bytes (1024 Mbytes)
constexpr int DATA_RECORD_SIZE = 40;      // Size of each record in bytes (key + value)
constexpr int BLOCK_NUMBER_SIZE = 5;      // Size of Block # or Child block # (5 digit block #)
constexpr int KEY_NUMBER_SIZE = 8;        // Size of Key # in Index Block
constexpr int DIRECTORY_SIZE = 256 * 20;  // Size of metadata, 3 FCBS, bit map

class NoSQLDatabase
{
private:
    string databaseName;
    fstream databaseFile;
    int currentPosInDb;    // position starts from 0 in each PFS file (e.g. .db0, .db1, .db2,...)
    int currentPosInBlock; // position starts from 0 to 255 in each block
    int currentBlock;      // block number starts from 0
    int dbNumber;          // database number starts from 0

    BTree bTree;  // include BTree for indexing
    int indexBfr; // index blocking factor

    // Define structures for File Control Block (FCB) and directory entry
    struct FCB
    {
        string filename;
        uintmax_t fileSize;
        time_t timestamp; // Last modified time
        int startBlock;
        int numberOfBlocksUsed; // Number of blocks used
        int startingBlockIndex; //  root of the btree
        // TODO: index info
        // Add other necessary fields such as create time, create date, etc.
    };

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
    FCB fcb;
    vector<FCB> directory;
    // formating the data to write into the database
    void writeDataBoundaries(string &data, int &currentBlock, int &currentPosInBlock);
    string intToFiveDigitString(int number);

public:
    // constructor of NoSQLDatabase
    NoSQLDatabase();
    ~NoSQLDatabase();

    // File system operations
    // command: operations to perform.

    // open PFSfile: Allocate a new 1 MByte "PFS" file if it does not already exist. If it does exist, begin using it for further commands.
    void openOrCreateDatabase(string &PFSFile, int dbNumber);
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
