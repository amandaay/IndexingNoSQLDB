// File: NoSQLDatabase.cpp

#include "NoSQLDatabase.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;
using namespace std::filesystem;

NoSQLDatabase::NoSQLDatabase() : bTree(0)
{
    currentPosInDb = DIRECTORY_SIZE;            // position starts after the directory structure
    currentPosInBlock = 0;                      // position starts from 0 to 255 in each block
    currentBlock = DIRECTORY_SIZE / BLOCK_SIZE; // which block in each db, starts from block 20
    dbNumber = 0;                               // defines database number (e.g. test.db0, test.db1, ...)
    indexBfr = (BLOCK_SIZE - BLOCK_NUMBER_SIZE) / (KEY_NUMBER_SIZE + (BLOCK_NUMBER_SIZE * 2));
}

NoSQLDatabase::~NoSQLDatabase()
{
    // Close the database file
    if (databaseFile.is_open())
    {
        databaseFile.close();
    }
}

void NoSQLDatabase::writeDataBoundaries(string &data, int currentBlock, int currentPosInBlock)
{
    // current block: row, currentPosInBlock: column
    // Check if adding the record would exceed the block size
    // Each block is 256 bytes that includes a child block size of 4 bytes
    // adding 1 byte for newline character for formatting reasons
    if (currentPosInBlock + DATA_RECORD_SIZE >= BLOCK_SIZE)
    {
        // Move to the next block
        cout << "Block is full." << endl;
        fcb.numberOfBlocksUsed++;
        currentBlock++;
        currentPosInBlock = 0;
        cout << "Moving to the next block " << currentBlock << endl;
        cout << "Writing at position in the block " << currentPosInBlock << endl;
        cout << "currentBlock * (BLOCK_SIZE + 1): " << currentBlock * (BLOCK_SIZE + 1) << endl;
        databaseFile.seekp(currentBlock * (BLOCK_SIZE + 1) + currentPosInBlock);
        databaseFile << data;
        currentPosInBlock++;
    }
    else
    {
        databaseFile.seekp(currentBlock * (BLOCK_SIZE + 1) + currentPosInBlock);
        databaseFile << data;
    }
    databaseFile.flush();
}

string NoSQLDatabase::intToFiveDigitString(int number)
{
    // Convert an integer to a 5-digit string
    stringstream ss;
    ss << setw(5) << setfill('0') << number;
    return ss.str();
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
        databaseFile.open(filePath, ios::out);
        if (!databaseFile.is_open())
        {
            if (!databaseFile.is_open())
            {
                cout << "Error: Unable to create a file." << endl;
                return;
            }
        }

        // Allocate a new 1 MByte "PFS" file if it does not already exist.
        for (int i = 0; i < (INITIAL_SIZE / BLOCK_SIZE); i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                databaseFile << " ";
            }
            databaseFile << endl;
        }

        // Explicitly set the get pointer's position to the beginning of the file
        // replace "-" with metadata

        // databaseName
        databaseFile.seekp(0, ios::beg);
        databaseFile << databaseName; // takes up 0-49th byte

        // initial file size
        databaseFile.seekp(50);
        databaseFile << to_string(INITIAL_SIZE);

        // total number of files (PFS)
        databaseFile.seekp(60);
        databaseFile << to_string(dbNumber + 1);

        // blocksize
        databaseFile.seekp(70);
        databaseFile << to_string(BLOCK_SIZE);

        // number of uploaded files e.g. movies-small.csv, should be empty
        databaseFile.seekp(80);
        fcb.fileSize = 0;
        databaseFile << to_string(fcb.fileSize);
        databaseFile.flush();

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

    // Initialize FCB information
    fcb.filename = myFile;
    fcb.fileSize = file_size(myFile);             // The actual file size
    fcb.timestamp = time(nullptr);                // Set the timestamp to current time
    fcb.startBlock = DIRECTORY_SIZE / BLOCK_SIZE; // The data starts after the directory structure
    fcb.numberOfBlocksUsed = 0;                   // Number of blocks used

    // Skip the first row (header)
    string header;
    getline(fileToRead, header);

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
            // Truncate the data to fit within DATA_RECORD_SIZE bytes
            if (line.size() > DATA_RECORD_SIZE)
            {
                // leaves 1 byte to account for nextline character
                line.resize(DATA_RECORD_SIZE);
            }
            else if (line.size() < DATA_RECORD_SIZE)
            {
                // Remove the newline character
                line.resize(line.size() - 1);
                // Pad the line with spaces to reach 40 bytes
                line.resize(DATA_RECORD_SIZE, ' ');
            }

            // check if the current data file is full
            if (currentPosInDb + DATA_RECORD_SIZE >= (INITIAL_SIZE * (dbNumber + 1)))
            {
                cout << "Database file is full." << endl;
                databaseFile.close();
                dbNumber++;
                openOrCreateDatabase(databaseName, dbNumber);
                cout << "Updated position in DB " << currentPosInDb << endl;
                currentPosInBlock = 0;
                cout << "Updated position in the block " << currentPosInBlock << endl;
                fcb.numberOfBlocksUsed++;
                currentBlock++;
            }

            cout << "Writing at block " << currentBlock << endl;
            cout << "Writing at position in the block " << currentPosInBlock << endl;
            cout << "Writing current line database file: " << line << endl;
            writeDataBoundaries(line, currentBlock, currentPosInBlock);

            // Index the key using B-tree
            insertIntoBTree(key);

            cout << "KEY inserting into BTree: " << key << endl;

            currentPosInDb += DATA_RECORD_SIZE;    // include directory structure
            currentPosInBlock += DATA_RECORD_SIZE; // each individual block
            fcb.timestamp = time(nullptr);         // update the timestamp

            cout << "Entered new record." << endl;
            cout << endl;
        }
    }
    // Add the FCB to the directory
    directory.push_back(fcb);

    // update metadata 1 block total
    // database total size
    databaseFile.seekp(50);
    databaseFile << to_string(INITIAL_SIZE * (dbNumber + 1)); // takes 50-59th byte

    // total number of files (PFS)
    databaseFile.seekp(60);
    databaseFile << to_string((dbNumber + 1));

    // metadata total files uploaded
    databaseFile.seekp(80);
    databaseFile << to_string(directory.size());

    tm *localTime;
    char time[100];

    // Add FCB to the directory structure
    for (int i = 0; i < directory.size(); i++)
    {
        // filename
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1)); // starting from block 2, 0 - 49th byte
        databaseFile << directory[i].filename;

        // file size
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 50); // starting from block 2, 50th byte
        databaseFile << to_string(directory[i].fileSize);

        // last modified time
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 60);
        localTime = localtime(&directory[i].timestamp);

        // Ensure hour is within 0-23 range
        if (localTime->tm_hour < 0)
        {
            localTime->tm_hour += 24;
            localTime->tm_mday -= 1;
        }
        databaseFile << put_time(localTime, "%Y-%m-%d,%H:%M:%S");

        // start block
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 80);
        databaseFile << intToFiveDigitString(directory[i].startBlock);

        // Number of blocks used
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 90);
        databaseFile << intToFiveDigitString(directory[i].numberOfBlocksUsed);

        // Starting block for index (i.e. root)
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 100);
        databaseFile << intToFiveDigitString(directory[i].startingBlockIndex);
        databaseFile.flush();
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
    // e.g. movie 128 bytes 12:30 PM September 2
    //      myFiles filesize timestamp
    tm *localTime;
    char time[50];

    for (int i = 0; i < directory.size(); i++)
    {
        localTime = localtime(&directory[i].timestamp);
        strftime(time, 50, "%I:%M %p %B %d", localTime);
        cout << directory[i].filename << "\t" << directory[i].fileSize << " bytes\t" << time << endl;
    }
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
