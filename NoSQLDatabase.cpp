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
    currentBlock = firstBlockAvailable();
    if (currentBlock == (INITIAL_SIZE / BLOCK_SIZE) * (dbNumber + 1))
    // if (currentBlock == -1)
    // change to if there's empty in our bitmap
    {
        cout << "Database file is full." << endl;
        databaseFile.close();
        dbNumber++;
        openOrCreateDatabase(databaseName, dbNumber); // create the new PFS file
        currentPosInBlock = 0;
        fcb.numberOfBlocksUsed++;
        // currentBlock = DIRECTORY_SIZE / BLOCK_SIZE + dbNumber * (INITIAL_SIZE / BLOCK_SIZE);
    }

    // Check if adding the record would exceed the block size
    // Each block is 256 bytes that includes a child block size of 4 bytes
    if (currentPosInBlock + DATA_RECORD_SIZE >= BLOCK_SIZE)
    {
        // Move to the next block
        cout << "Block is full." << endl;
        bitMap(currentBlock, true, false);
        fcb.numberOfBlocksUsed++;
        currentBlock = firstBlockAvailable();
        cout << "current block: " << currentBlock << endl;
        currentPosInBlock = 0;
    }
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + currentPosInBlock);
    databaseFile << data;
    databaseFile.flush();
}

string NoSQLDatabase::intToFiveDigitString(int number)
{
    // Convert an integer to a 5-digit string
    stringstream ss;
    ss << setw(5) << setfill('0') << number;
    return ss.str();
}

void NoSQLDatabase::updateDirectory(int dbNumber)
{
    currentBlock = (currentBlock / (INITIAL_SIZE / BLOCK_SIZE)) * (INITIAL_SIZE / BLOCK_SIZE);
    cout << "update directory curr block: " << currentBlock << endl;
    currentPosInBlock = 0;

    // databaseName
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + currentPosInBlock);
    databaseFile << databaseName;
    // writeDataBoundaries(databaseName, currentBlock, currentPosInBlock); // takes up 0-49th byte

    // total size of the database (1 PFS = 1 Mbyte)
    int metaDataSizePos = 50; // position of metadata size in the block
    string pfsSize = to_string(INITIAL_SIZE * (dbNumber + 1));
    // writeDataBoundaries(pfsSize, currentBlock, metaDataSizePos); // takes up 50-59th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + metaDataSizePos);
    databaseFile << pfsSize;

    // total number of files (PFS)
    int totalPfsFilesPos = 60; // position of total pfs files in the block
    string totalPfsFiles = to_string(dbNumber + 1);
    // writeDataBoundaries(totalPfsFiles, currentBlock, totalPfsFilesPos); // takes up 60-69th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + totalPfsFilesPos);
    databaseFile << totalPfsFiles;

    // blocksize
    int blocksizePos = 70; // position of block size in the block
    string blocksize = to_string(BLOCK_SIZE);
    // writeDataBoundaries(blocksize, currentBlock, blocksizePos); // takes up 70-79th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + blocksizePos);
    databaseFile << blocksize;

    // number of uploaded files e.g. movies-small.csv, should be empty
    // when the database is created, there's no file uploaded yet
    // it gets updated when a file is uploaded (PUT command)
    uploadedFilesPos = 80; // position of uploaded files in the block
    string uploadedFiles = to_string(directory.size());
    // writeDataBoundaries(uploadedFiles, currentBlock, uploadedFilesPos); // takes up 80-89th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + uploadedFilesPos);
    databaseFile << uploadedFiles;

    // update metadata bitmap
    bitMap(currentBlock, true, false);

    // fcb structure
    currentBlock += 1; // start from block 1

    tm *localTime;
    // Update the directory structure
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
        bitMap(currentBlock, true, false);
        currentBlock++;
        cout << "current block after fcb: " << currentBlock << endl;
        databaseFile.flush();
    }
    databaseFile.flush();
}

void NoSQLDatabase::bitMap(int &currentBlock, bool isSet, bool initialize)
{
    cout << "database file: " << databaseName + to_string(dbNumber)<< " current block: " << currentBlock << " isSet: " << isSet << " initialize: " << initialize << endl;
    // initialize the bitmap
    if (initialize)
    {
        // Initialize bitmap
        for (int i = 4; i < DIRECTORY_SIZE / BLOCK_SIZE; i++)
        {
            for (int j = 0; j < BLOCK_SIZE; j++)
            {
                databaseFile.seekp(j + (i * (BLOCK_SIZE + 1)));
                if (i == 4 && j >= 4 && j <= 19)
                {
                    // bitmap itself
                    databaseFile << "1";
                }
                else
                {
                    databaseFile << "0";
                }
            }
        }
        initialize = false;
    }
    // Update the bitmap to indicate that the block is allocated
    // bit map
    // 0 indicates a free block, 1 indicates that the block is allocated.
    if (isSet)
    {
        cout << "Setting block " << currentBlock << " to 1" << endl;
        databaseFile.seekp(((currentBlock / BLOCK_SIZE) + 4) * (BLOCK_SIZE + 1) + (currentBlock % BLOCK_SIZE));
        databaseFile << "1";
        isSet = false;
    }
    databaseFile.flush();
}

int NoSQLDatabase::firstBlockAvailable()
{
    // to read the bitmap
    openOrCreateDatabase(databaseName, dbNumber);
    // Checks first available block
    // return currentBlock = first available block
    // 0 indicates a free block, 1 indicates that the block is allocated.
    
    for (int i = 4; i < (DIRECTORY_SIZE / BLOCK_SIZE); i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            if (i == 4 && j < DIRECTORY_SIZE / BLOCK_SIZE)
            {
                continue;
            }
            databaseFile.seekg(j + (i * (BLOCK_SIZE + 1)));
            char blockStatus;
            databaseFile >> blockStatus;
            if (blockStatus == '0')
            {
                return ((i - 4) * BLOCK_SIZE + (INITIAL_SIZE/BLOCK_SIZE * dbNumber) + j);
            }
        }
    }
    return (INITIAL_SIZE / BLOCK_SIZE) * (dbNumber + 1);
    // return -1;
}

void NoSQLDatabase::openOrCreateDatabase(string &PFSFile, int dbNumber)
{
    // close existing database file before opening others or creating other databases
    if (databaseFile.is_open())
    {
        databaseFile.close();
        // cout << "Closed: " << databaseName << endl;
    }
    databaseName = PFSFile;
    // Open the database file or Create if db not exist
    string filePath = databaseName + ".db" + to_string(dbNumber);
    if (exists(filePath))
    {
        // open file
        databaseFile.open(filePath, ios::out | ios::in);
        // check file path
        // cout << "Opening database file " << filePath << endl;
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to open database file." << endl;
            return;
        }
        // cout << "Database opened successfully." << endl;
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

        // Initialize the bitmap
        bitMap(currentBlock, false, true);

        // Explicitly set the get pointer's position to the beginning of the file
        // replace " " with metadata
        updateDirectory(dbNumber);

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
    openOrCreateDatabase(databaseName, dbNumber);
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
    currentBlock = firstBlockAvailable();
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
            // rm newline character
            line.pop_back();

            // consider special character like single " ' " and double " " " quotes
            for (int i = 0; i < line.size(); i++)
            {
                const unsigned char value = (unsigned char)line[i];
                if (!isalnum(value) && !isspace(value) && !ispunct(value))
                {
                    line.erase(i, 1);
                    i--;
                }
            }

            // pad the rest of the line with spaces
            line.resize(DATA_RECORD_SIZE, ' ');

            // Write the data to the database
            writeDataBoundaries(line, currentBlock, currentPosInBlock);

            // Index the key using B-tree
            insertIntoBTree(key);

            cout << "KEY inserting into BTree: " << key << endl;

            currentPosInBlock += DATA_RECORD_SIZE; // each individual block
            fcb.timestamp = time(nullptr);         // update the timestamp
        }
    }
    bitMap(currentBlock, true, false);

    // Add the FCB to the directory
    directory.push_back(fcb);

    // Update the directory structure in each PFS file
    for (int i = 0; i <= dbNumber; i++)
    {
        // close the database File (PFS file) we were working on first then update the rest
        databaseFile.close();
        openOrCreateDatabase(databaseName, i); //  opens each PFS file prior
        updateDirectory(dbNumber);
        databaseFile.close();
    }

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

void NoSQLDatabase::delFileFromDatabase(string &myFile)
{
    // Delete myfile from NoSQL database
    // deletes the data, the index, fcb (within the directory),
    // and update metadata (size, total PFS files, total uploaded files)
    // rm the PFS files if there's extra
}

void NoSQLDatabase::listAllDataFromDatabase()
{
    // List all data files in NoSQL database
    // e.g. movie 128 bytes 12:30 PM September 2
    //      myFiles filesize timestamp
    if (directory.empty())
    {
        cout << "No files found in the database." << endl;
        return;
    }
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

void NoSQLDatabase::killDatabase(string &PFSFile)
{
    // Delete the NoSQL database
    // e.g. rm PFSFile
    databaseName = PFSFile;

    for (int i = 0; i <= dbNumber; i++)
    {
        // check if the file exist first
        string filePath = databaseName + ".db" + to_string(i);
        if (exists(filePath))
        {
            // delete the file
            if (!remove(filePath))
            {
                cerr << "Error: Unable to delete file " << filePath << endl;
            }
            else
            {
                for (int i = 0; i < directory.size(); i++)
                {
                    directory.pop_back();
                }
                cout << "File " << filePath << " deleted successfully." << endl;
            }
        }
        else
        {
            cout << "File " << filePath << " does not exist." << endl;
        }
    }
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
            delFileFromDatabase(file);
            break;
        case DIR:
            listAllDataFromDatabase();
            break;
        case FIND:
            findValueFromDatabase();
            break;
        case KILL:
            killDatabase(file);
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
