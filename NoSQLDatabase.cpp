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
    currentPosInBlock = 0; // position starts from 0 to 255 in each block
    currentBlock = 0;      // which block in each db
    dbNumber = 0;          // defines database number (e.g. test.db0, test.db1, ...)
    uploadedFilesPos = 80; // position of uploaded files in the block
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

void NoSQLDatabase::writeDataBoundaries(string &data, int &currentBlock, int &currentPosInBlock)
{
    // current block: row, currentPosInBlock: column
    // adding 1 byte for newline character for formatting reasons

    // check if the current data file is full
    if ((currentPosInBlock + (currentBlock * (BLOCK_SIZE + 1))) + DATA_RECORD_SIZE >= (INITIAL_SIZE * (dbNumber + 1)))
    {
        cout << "Database file is full." << endl;
        updateDirectory();
        databaseFile.close();
        dbNumber++;
        openOrCreateDatabase(databaseName, dbNumber);
        currentPosInBlock = 0;
        fcb.numberOfBlocksUsed++;
        currentBlock = DIRECTORY_SIZE / BLOCK_SIZE;
    }

    // Check if adding the record would exceed the block size
    // Each block is 256 bytes that includes a child block size of 4 bytes
    if (currentPosInBlock + DATA_RECORD_SIZE >= BLOCK_SIZE)
    {
        // Move to the next block
        cout << "Block is full." << endl;
        fcb.numberOfBlocksUsed++;
        currentBlock++;
        currentPosInBlock = 0;
    }
    databaseFile.seekp(currentBlock * (BLOCK_SIZE + 1) + currentPosInBlock);
    databaseFile << data;
    databaseFile.flush();
    currentPosInBlock++;
}

string NoSQLDatabase::intToFiveDigitString(int number)
{
    // Convert an integer to a 5-digit string
    stringstream ss;
    ss << setw(5) << setfill('0') << number;
    return ss.str();
}

// tm *NoSQLDatabase::getTimestamp(time_t timestamp)
// {
//     // Get the current timestamp
//     tm *localTime;
//     // Ensure hour is within 0-23 range
//     if (localTime->tm_hour < 0)
//     {
//         localTime->tm_hour += 24;
//         localTime->tm_mday -= 1;
//     }
//     return localtime(&timestamp);
// }

void NoSQLDatabase::updateDirectory()
{
    cout << "enter update directory" << endl;
    // metadata total files uploaded
    uploadedFilesPos = 80; // position of uploaded files in the block
    databaseFile.seekp(uploadedFilesPos);
    databaseFile << to_string(directory.size() + 1);
    cout << "# of uploaded Files:   " << to_string(directory.size()) << endl;
    cout << "Current Block:    " << currentBlock << endl;
    cout << "Current Position: " << currentPosInBlock << endl;
    tm *localTime;
    if (directory.empty())
    {
        // filename
        databaseFile.seekp(BLOCK_SIZE + 1); // starting from block 2, 0 - 49th byte
        databaseFile << fcb.filename;

        // file size
        databaseFile.seekp((BLOCK_SIZE + 1) + 50); // starting from block 2, 50th byte
        databaseFile << to_string(fcb.fileSize);

        // last modified time
        databaseFile.seekp((BLOCK_SIZE + 1) + 60);
        localTime = localtime(&fcb.timestamp);

        // Ensure hour is within 0-23 range
        if (localTime->tm_hour < 0)
        {
            localTime->tm_hour += 24;
            localTime->tm_mday -= 1;
        }
        databaseFile << put_time(localTime, "%Y-%m-%d,%H:%M:%S");

        // start block
        databaseFile.seekp((BLOCK_SIZE + 1) + 80);
        databaseFile << intToFiveDigitString(fcb.startBlock);

        // Number of blocks used
        databaseFile.seekp((BLOCK_SIZE + 1) + 90);
        databaseFile << intToFiveDigitString(fcb.numberOfBlocksUsed);

        // Starting block for index (i.e. root)
        databaseFile.seekp((BLOCK_SIZE + 1) + 100);
        databaseFile << intToFiveDigitString(fcb.startingBlockIndex);
        databaseFile.flush();
    }
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
        cout << "Number of blocks used: " << directory[i].numberOfBlocksUsed << endl;
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 90);
        databaseFile << intToFiveDigitString(directory[i].numberOfBlocksUsed);

        // Starting block for index (i.e. root)
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 100);
        databaseFile << intToFiveDigitString(directory[i].startingBlockIndex);
        databaseFile.flush();
    }
}

void NoSQLDatabase::openOrCreateDatabase(string &PFSFile, int dbNumber)
{
    databaseName = PFSFile;
    // Open the database file or Create if db not exist
    string filePath = databaseName + ".db" + to_string(dbNumber);
    if (exists(filePath))
    {
        // open file
        for (int i = 0; i < dbNumber; i++)
        {
            databaseFile.open(databaseName + ".db" + to_string(i), ios::in | ios::app);
            // check file path
            cout << "Open database file " << databaseName << ".db" << i << endl;
            if (!databaseFile.is_open())
            {
                cout << "Error: Unable to open database file." << endl;
                return;
            }
        }
        cout << "Database opened successfully." << endl;
    }
    else
    {
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
        // replace " " with metadata
        currentBlock = 0;
        currentPosInBlock = 0;

        // databaseName
        writeDataBoundaries(databaseName, currentBlock, currentPosInBlock); // takes up 0-49th byte

        // total size of the database (1 PFS = 1 Mbyte)
        int metaDataSizePos = 50; // position of metadata size in the block
        string pfsSize = to_string(INITIAL_SIZE * (dbNumber + 1));
        writeDataBoundaries(pfsSize, currentBlock, metaDataSizePos); // takes up 50-59th byte

        // total number of files (PFS)
        int totalPfsFilesPos = 60; // position of total pfs files in the block
        string totalPfsFiles = to_string(dbNumber + 1);
        writeDataBoundaries(totalPfsFiles, currentBlock, totalPfsFilesPos); // takes up 60-69th byte

        // blocksize
        int blocksizePos = 70; // position of block size in the block
        string blocksize = to_string(BLOCK_SIZE);
        writeDataBoundaries(blocksize, currentBlock, blocksizePos); // takes up 70-79th byte

        // number of uploaded files e.g. movies-small.csv, should be empty
        // when the database is created, there's no file uploaded yet
        // it gets updated when a file is uploaded (PUT command)
        uploadedFilesPos = 80; // position of uploaded files in the block
        string uploadedFiles = to_string(directory.size());
        writeDataBoundaries(uploadedFiles, currentBlock, uploadedFilesPos); // takes up 80-89th byte

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
    // data block starts after the directory structure
    currentBlock = DIRECTORY_SIZE / BLOCK_SIZE;
    currentPosInBlock = 0;

    // Initialize FCB information
    fcb.filename = myFile;
    fcb.fileSize = file_size(myFile); // The actual file size
    fcb.timestamp = time(nullptr);    // Set the timestamp to current time
    fcb.startBlock = currentBlock;    // The data starts after the directory structure
    fcb.numberOfBlocksUsed = 1;       // Number of blocks used
    fcb.startingBlockIndex = 0;       // The starting block for the index (i.e. root)

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
            else
            {
                // Remove the newline character
                line.resize(line.size() - 1);
                // Pad the line with spaces to reach 40 bytes
                line.resize(DATA_RECORD_SIZE, ' ');
            }
            // Write the data to the database
            writeDataBoundaries(line, currentBlock, currentPosInBlock);

            // Index the key using B-tree
            insertIntoBTree(key);

            cout << "KEY inserting into BTree: " << key << endl;
            currentPosInBlock += DATA_RECORD_SIZE; // each individual block
            fcb.timestamp = time(nullptr);         // update the timestamp
        }
    }
    // Add the FCB to the directory
    directory.push_back(fcb);

    cout << fcb.numberOfBlocksUsed << " blocks used." << endl;

    // Add FCB to the directory structure
    updateDirectory();

    // Close the file to read
    fileToRead.close();

    cout << "Data from file " << myFile << " inserted into database " << databaseName << " successfully." << endl;
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
