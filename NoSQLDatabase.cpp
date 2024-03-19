// File: NoSQLDatabase.cpp

#include "NoSQLDatabase.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>

using namespace std;
using namespace std::filesystem;

NoSQLDatabase::NoSQLDatabase() : bTree(0)
{
}

NoSQLDatabase::~NoSQLDatabase()
{
    // Close the database file
    if (databaseFile.is_open())
    {
        databaseFile.close();
    }
}

// Implement B-tree insertion method
void NoSQLDatabase::insertIntoBTree(int key)
{
    bTree.Insert(key);
}

// Implement B-tree search method
bool NoSQLDatabase::searchInBTree(int key)
{
    vector<int> NodeIds;
    return bTree.Lookup(bTree.getRootNode(), key, NodeIds);
}

void NoSQLDatabase::openOrCreateDatabase(string &PFSFile)
{
    databaseName = PFSFile;
    // Open the database file or Create if db not exist
    path filePath = databaseName + ".db0";
    if (exists(filePath))
    {
        // check file path
        cout << "filePath: " << filePath << endl;
        // open file
        databaseFile.open(filePath, ios::in | ios::out | ios::binary);
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to open database file." << endl;
            return;
        }
        cout << "Database opened successfully." << endl;
    }
    else
    {
        cout << "Creating database " << databaseName << endl;
        // Open the database file for appending in binary mode
        databaseFile.open(filePath, ios::out | ios::binary);
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to create database file." << endl;
            return;
        }

        // Allocate a new 1 MByte "PFS" file if it does not already exist.
        // -1 byte is used to position the put pointer at the byte at the end of the file
        databaseFile.seekp(INITIAL_SIZE - 1);
        // Write a single byte at the end of the file
        databaseFile.write("", 1);

        int indexBlockingFactor = BLOCK_SIZE / MAX_INDEX_RECORD_SIZE;
        // Initialize B-tree index
        bTree = BTree(indexBlockingFactor);
    }
}

void NoSQLDatabase::putDataIntoDatabase(string &myFile)
{
    // Insert data from OS file into NoSQL database
    // Accessing the databaseName member variable
    cout << "Putting data into database " << databaseName << " from file " << myFile << endl;
    ifstream fileToRead(myFile);
    if (!fileToRead.is_open())
    {
        cout << "Error: Unable to open file " << myFile << endl;
        return;
    }

    if (!databaseFile.is_open())
    {
        cout << "Error: Unable to open database file " << databaseName << endl;
        return;
    }

    // Skip the first row (header)
    string header;
    getline(fileToRead, header);

    // Calculate the number of records that can fit in a block
    int dataBfr = BLOCK_SIZE / MAX_DATA_RECORD_SIZE; // 256 / 40 = 6
    int currentPos = 0;
    int currentBlock = 0;

    // Read and parse the remaining lines of the CSV file
    string line;
    while (getline(fileToRead, line))
    {
        stringstream ss(line);
        string keyString;

        // Assuming the key is located in the first field of each line
        if (getline(ss, keyString, ','))
        {
            // Convert the key to the appropriate data type (e.g., integer)
            int key = stoi(keyString);

            // Assuming the rest of the line constitutes the value
            // Cuts of at MAX_DATA_RECORD_SIZE (40 bytes)
            string value = line.substr(keyString.length() + 1, MAX_DATA_RECORD_SIZE);

            // Check if adding the record would exceed the block size
            if (currentPos + MAX_DATA_RECORD_SIZE > BLOCK_SIZE)
            {
                // Move to the next block
                databaseFile.seekp((currentBlock + 1) * BLOCK_SIZE);
                currentBlock++;
                cout << "Writing to data block " << currentBlock << endl;
                currentPos = 0;
            }
            if (currentBlock == 0)
            {
                cout << "Writing to data block " << currentBlock << endl;
            }
            databaseFile << key << " " << value << "\n";

            // Index the key using B-tree
            insertIntoBTree(key);
            currentPos += MAX_DATA_RECORD_SIZE;
        }
    }

    // Close the file to read
    fileToRead.close();

    cout << "Data from file " << myFile << " inserted into database " << databaseName << " successfully." << endl;
}

void NoSQLDatabase::getDataFromDatabase()
{
    // Download data file from NoSQL database to OS directory
}

void NoSQLDatabase::delFileFromDatabase()
{
    // Delete file from NoSQL database
}

void NoSQLDatabase::listAllDataFromDatabase()
{
    // List all data files in NoSQL database
}

void NoSQLDatabase::findValueFromDatabase()
{
    // Find value using a given key
}

void NoSQLDatabase::killDatabase()
{
    // Delete the NoSQL database
}

void NoSQLDatabase::quitDatabase()
{
    // Exit NoSQL database
    exit(0);
}

NoSQLDatabase::Command NoSQLDatabase::getCommandType(const string &command)
{
    if (command == "open")
    {
        return OPEN;
    }
    else if (command == "put")
    {
        return PUT;
    }
    else if (command == "get")
    {
        return GET;
    }
    else if (command == "rm")
    {
        return RM;
    }
    else if (command == "dir")
    {
        return DIR;
    }
    else if (command == "find")
    {
        return FIND;
    }
    else if (command == "kill")
    {
        return KILL;
    }
    else if (command == "quit")
    {
        return QUIT;
    }
    else
    {
        return INVALID;
    }
}

void NoSQLDatabase::runCLI()
{
    // Implement the command-line interface
    string command;
    string file;

    do
    {
        // prompt user input
        cout << "NoSQL > ";

        // get user input
        getline(cin, command);

        // process the command
        if (command.empty())
        {
            cout << "Invalid command" << endl;
            continue;
        }

        // extract the command keyword
        string keyword = command.substr(0, command.find(" "));

        // Convert the command keyword to lowercase for case-insensitive matching
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

        if (command.length() > command.find(" "))
        {
            file = command.substr(command.find(" ") + 1, command.length());
        }

        // Execute actions based on the command keyword
        switch (getCommandType(keyword))
        {
        case OPEN:
            // file here is the database
            openOrCreateDatabase(file);
            break;
        case PUT:
            // file here is the file to be put into the database
            putDataIntoDatabase(file);
            break;
        case GET:
            getDataFromDatabase();
            break;
        case RM:
            delFileFromDatabase();
            break;
        case DIR:
            listAllDataFromDatabase();
            break;
        case FIND:
            findValueFromDatabase();
            break;
        case KILL:
            killDatabase();
            break;
        case QUIT:
            quitDatabase();
            break;
        case INVALID:
            cout << "Invalid command" << endl;
            break;
        }

    } while (true);
}
