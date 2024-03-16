// File: NoSQLDatabase.cpp

#include "NoSQLDatabase.h"
#include <iostream>
#include <fstream>

using namespace std;

NoSQLDatabase::NoSQLDatabase(const string& dbName) : databaseName(dbName) {
    // Initialize other members
}

NoSQLDatabase::~NoSQLDatabase() {
    // Cleanup resources
}

void NoSQLDatabase::openOrCreateDatabase() {
    // Open the database file or Create if db not exist

}

void NoSQLDatabase::putDataIntoDatabase() {
    // Insert data from OS file into NoSQL database

}

void NoSQLDatabase::getDataFromDatabase() {
    // Download data file from NoSQL database to OS directory

}   

void NoSQLDatabase::delFileFromDatabase() {
    // Delete file from NoSQL database

}

void NoSQLDatabase::listAllDataFromDatabase() {
    // List all data files in NoSQL database

}

void NoSQLDatabase::findValueFromDatabase() {
    // Find value using a given key

}

void NoSQLDatabase::killDatabase() {
    // Delete the NoSQL database

}

void NoSQLDatabase::runCLI() {
    // Implement the command-line interface
    string command;

    do {
        // prompt user input
        cout << "NoSQL > ";

        // get user input
        getline(cin, command);

        // process the command
        if (command.empty()){
            cout << "Invalid command" << endl;
            continue;
        }

        // extract the command keyword
        string keyword = command.substr(0, command.find(" "));

        // Convert the command keyword to lowercase for case-insensitive matching
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

        // Execute actions based on the command keyword
        switch (getCommandType(keyword)) {
            case OPEN:
                openOrCreateDatabase();
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
