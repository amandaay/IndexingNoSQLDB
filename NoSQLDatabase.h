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
constexpr int METADATA_START_POS = 0;
constexpr int INDEX_BFR = (BLOCK_SIZE - BLOCK_NUMBER_SIZE) / (KEY_NUMBER_SIZE + (BLOCK_NUMBER_SIZE * 2)); // Start position of metadata in the block

class NoSQLDatabase
{
private:
    string databaseName;
    fstream databaseFile;
    int currentPosInBlock; // position starts from 0 to 255 in each block
    int currentBlock;      // block number starts from 0
    int dbNumber;          // database number starts from 0
    int uploadedFilesPos;  // position of uploaded files in the block
    BTree bTree;           // include BTree for indexing
    int indexBlock;        // index block number

    // Define structures for File Control Block (FCB) and directory entry
    struct FCB
    {
        string filename;
        uintmax_t fileSize;  // actual file size of the uploaded file (e.g. movies-small.csv)
        time_t timestamp;    // Last modified time
        int dataStartBlock;  // Starting block of the data
        int dataBlockUsed;   // Number of blocks used
        int indexStartBlock; // The starting block for the index (i.e. root of its BTree)
    };
    FCB fcb;
    // Define a directory structure with the metadata, 3 FCBs
    void updateDirectory(int dbNumber);
    // A list of FCBs to store the directory entries
    vector<FCB> directory;
    // bitmap for block management
    void bitMap(int &currentBlock, bool isSet, bool initialize);
    // check first available block
    int firstAvailableBlock();
    // helper function to write data
    string intToFiveDigitString(int number);
    string intToEightDigitString(int number);
    // formating the data to write into the database
    void writeDataBoundaries(string &data, int &currentBlock, int &currentPosInBlock);
    // Index operations
    void handleIndexAllocation(int &currentBlock);
    // handle index search
    string handleIndexSearch(string &idxStartBlock, string &key, int &blkAccessed);
    // handle index search for delete bitmap
    void handleIndexSearchForDelete(string &idxStartBlock, int &pos);

    // Commands for client operation
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
    // get the command type
    Command getCommandType(const string &command);

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
    void getDataFromDatabase(string &myFile);
    // rm myFile: Delete "myfile" from your NoSQL database, i.e. PFS file.
    void delFileFromDatabase(string &myFile);
    // dir: List all data files in your NoSQL database, i.e., PFS file.
    void listAllDataFromDatabase();
    // find [myfile.key]: Find ‘value’ using a given ‘key’, i.e., return a record which is associate with a key from ‘myfile’ in your NoSQL database. In addition, you need to show how many blocks are accessed during ‘find’
    void findValueFromDatabase(string &myfileKey, int &blkAccessed);
    // kill PFSfile: Delete the NoSQL database, i.e., remove PFSfile from OS file system
    void killDatabase(string &PFSFile);
    // quit: exit NoSQL database
    void quitDatabase();

    // Command-line interface
    void runCLI();
};

#endif // NOSQL_DATABASE_H
