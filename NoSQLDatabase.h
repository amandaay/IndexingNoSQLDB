// File: NoSQLDatabase.h

#ifndef NOSQL_DATABASE_H
#define NOSQL_DATABASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Define constants for file system parameters
constexpr int BLOCK_SIZE = 256;  // Block size in bytes
constexpr int INITIAL_SIZE = 1024 * 1024;  // Initial size of database file in bytes

// Define structures for File Control Block (FCB) and directory entry
struct FCB {
    std::string filename;
    std::string filetype; // Data file or index file
    int startBlock;
    int endBlock;
    int fileSize;  // Number of blocks used
    // Add other necessary fields such as create time, create date, etc.
};

struct DirectoryEntry {
    FCB fcb;
    // Add other necessary fields
};

class NoSQLDatabase {
private:
    std::string databaseName;
    std::fstream databaseFile;
    std::vector<DirectoryEntry> directory;

    // Other private members for block management, file operations, etc.

public:
    NoSQLDatabase(const std::string& dbName);
    ~NoSQLDatabase();

    // File system operations
    bool openDatabase();
    bool createDatabase();
    bool closeDatabase();
    bool deleteDatabase();

    // Directory operations
    void listFiles();
    bool addFile(const std::string& filename, const std::string& filetype);
    bool removeFile(const std::string& filename);

    // Data operations
    bool putData(const std::string& filename, const std::string& data);
    std::string getData(const std::string& filename);
    bool removeData(const std::string& filename);

    // Index operations
    // Add functions for block-based indexing

    // Free block management
    // Add functions for bit map vector approach

    // Command-line interface
    void runCLI();
};

#endif // NOSQL_DATABASE_H
