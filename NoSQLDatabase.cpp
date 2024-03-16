// File: NoSQLDatabase.cpp

#include "NoSQLDatabase.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

NoSQLDatabase::NoSQLDatabase()
{
}

NoSQLDatabase::~NoSQLDatabase()
{
    // Cleanup resources
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
        databaseFile.open(filePath, ios::out | ios::binary);
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to create database file." << endl;
            return;
        }

        int numIndexBlocks = INITIAL_SIZE / BLOCK_SIZE;
        for (int i = 0; i < numIndexBlocks; ++i)
        {
            // Initialize index entries to -1 (indicating free block)
            index[i] = -1;
        }
        databaseFile.close();
    }
}

void NoSQLDatabase::putDataIntoDatabase()
{
    // Insert data from OS file into NoSQL database
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

        if (command.length() > command.find(" ")) {
            file = command.substr(command.find(" ") + 1, command.length());
        }

        // Execute actions based on the command keyword
        switch (getCommandType(keyword))
        {
        case OPEN:
            openOrCreateDatabase(file);
            break;
        case PUT:
            putDataIntoDatabase();
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
