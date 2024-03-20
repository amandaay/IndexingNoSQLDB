// File: NoSQLDatabase.cpp

#include "NoSQLDatabase.h"
#include <iostream>
#include <fstream>
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

void NoSQLDatabase::openOrCreateDatabase(string &PFSFile, int dbNumber)
{
    databaseName = PFSFile;
    // Open the database file or Create if db not exist
    string filePath = databaseName + ".db" + to_string(dbNumber);
    if (exists(filePath))
    {
        // check file path
        cout << "filePath: " << filePath << endl;
        // open file
        databaseFile.open(filePath);
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
        cout << "Creating database file " << filePath << endl;
        // Open the database file
        databaseFile.open(filePath, ios::out | ios::app | ios::in);
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to create database file." << endl;
            return;
        }

        // Allocate a new 1 MByte "PFS" file if it does not already exist.
        databaseFile.seekp(INITIAL_SIZE);
        // Explicitly set the get pointer's position to the beginning of the file
        databaseFile.seekp(DIRECTORY_SIZE, ios::beg);

        int indexBfr = (BLOCK_SIZE - CHILD_BLOCK_SIZE) / (INDEX_BLOCK_SIZE + CHILD_BLOCK_SIZE);
        // Initialize B-tree index
        bTree = BTree(indexBfr);
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
    int currentPosInDb = DIRECTORY_SIZE; // position starts after the directory structure starting 0 in the beginning
    int currentPosInBlock = 0;
    int currentBlock = 0;
    int dbNumber = 0;

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

            // Cuts of at DATA_RECORD_SIZE (40 bytes)
            // Truncate the data to fit within MAX_DATA_RECORD_SIZE bytes
            if (line.size() > DATA_RECORD_SIZE)
            {
                line.resize(DATA_RECORD_SIZE - 1);
                // Check if the last character is not a newline
                if (line.back() != '\n')
                {
                    // If not, add the newline character
                    line.push_back('\n');
                }
            }

            cout << "Writing to block: " << currentBlock << endl;
            cout << "Writing at position in the dataFile " << currentPosInDb << endl;
            cout << "Writing at position in the block " << currentPosInBlock << endl;

            // check if the current data file is full
            if (currentPosInDb + DATA_RECORD_SIZE >= (INITIAL_SIZE * (dbNumber + 1)))
            {
                cout << "Database file is full." << endl;
                databaseFile.close();
                dbNumber++;
                openOrCreateDatabase(databaseName, dbNumber);
                // Explicitly set the get pointer's position to the beginning of the file
                // update position in database and each block
                currentPosInDb = DIRECTORY_SIZE;
                cout << "Updated position in DB " << currentPosInDb << endl;
                currentPosInBlock = 0;
                cout << "Updated position in the block " << currentPosInBlock << endl;
                currentBlock++;
                cout << "Updated block " << currentBlock << endl;
            }

            // Check if adding the record would exceed the block size
            if (currentPosInBlock + DATA_RECORD_SIZE >= BLOCK_SIZE)
            {
                // Move to the next block
                cout << "Block is full." << endl;
                currentBlock++;
                cout << "Writing to updated block " << currentBlock << endl;
                currentPosInDb = currentBlock * BLOCK_SIZE;
                cout << "Writing to updated position in the dataFile " << currentPosInDb << endl;
                currentPosInBlock = 0;
                cout << "Writing at position in the block " << currentPosInBlock << endl;
                databaseFile.seekp(currentPosInDb);
            }
            else
            {
                cout << "Writing to current data block " << currentBlock << endl;
                databaseFile.seekp(currentPosInDb);
            }

            databaseFile << line;

            // Index the key using B-tree
            insertIntoBTree(key);
        }
        currentPosInDb += DATA_RECORD_SIZE;
        currentPosInBlock += DATA_RECORD_SIZE;
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
            openOrCreateDatabase(file, 0);
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
