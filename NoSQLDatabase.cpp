// File: NoSQLDatabase.cpp

#include "NoSQLDatabase.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <queue>
#include <set>
#include "BTree.h"
#include <istream>

using namespace std;
using namespace std::filesystem;

NoSQLDatabase::NoSQLDatabase() : bTree(INDEX_BFR)
{
    currentPosInBlock = 0; // position starts from 0 to 255 in each block
    currentBlock = 0;      // which block in each db
    dbNumber = 0;          // defines database number (e.g. test.db0, test.db1, ...)
    uploadedFilesPos = 80; // position of uploaded files in the block
    leftmost = "99999";    // leftmost key in the index block after rm cmd
    blkAccessed = 0;       // number of blocks accessed during find cmd
}

void NoSQLDatabase::updateDirectory(int dbNumber)
{
    currentBlock = (currentBlock / (INITIAL_SIZE / BLOCK_SIZE)) * (INITIAL_SIZE / BLOCK_SIZE);
    currentPosInBlock = 0;

    // databaseName
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + currentPosInBlock);
    databaseFile << databaseName; // takes up 0-49th byte
    databaseFile << string(50 - databaseName.size(), ' ');

    // total size of the database (1 PFS = 1 Mbyte)
    int metaDataSizePos = 50;                                  // position of metadata size in the block
    string pfsSize = to_string(INITIAL_SIZE * (dbNumber + 1)); // takes up 50-59th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + metaDataSizePos);
    databaseFile << pfsSize;
    databaseFile << string(60 - 50 - pfsSize.size(), ' ');

    // total number of files (PFS)
    int totalPfsFilesPos = 60;                      // position of total pfs files in the block
    string totalPfsFiles = to_string(dbNumber + 1); // takes up 60-69th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + totalPfsFilesPos);
    databaseFile << totalPfsFiles;
    databaseFile << string(70 - 60 - totalPfsFiles.size(), ' ');

    // blocksize
    int blocksizePos = 70;                    // position of block size in the block
    string blocksize = to_string(BLOCK_SIZE); // takes up 70-79th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + blocksizePos);
    databaseFile << blocksize;
    databaseFile << string(80 - 70 - blocksize.size(), ' ');

    // number of uploaded files e.g. movies-small.csv, should be empty
    // when the database is created, there's no file uploaded yet
    // it gets updated when a file is uploaded (PUT command)
    uploadedFilesPos = 80;                              // position of uploaded files in the block
    string uploadedFiles = to_string(directory.size()); // takes up 80-89th byte
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + uploadedFilesPos);
    databaseFile << uploadedFiles;
    databaseFile << string(BLOCK_SIZE - 80 - uploadedFiles.size(), ' ');

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
        databaseFile << string(50 - directory[i].filename.size(), ' ');

        // file size
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 50); // starting from block 2, 50th byte
        databaseFile << to_string(directory[i].fileSize);
        databaseFile << string(60 - 50 - to_string(directory[i].fileSize).size(), ' ');

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
        databaseFile << intToFiveDigitString(directory[i].dataStartBlock);

        // Number of blocks used
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 90);
        databaseFile << intToFiveDigitString(directory[i].dataBlockUsed);

        // Starting block for index (i.e. root)
        databaseFile.seekp((i + 1) * (BLOCK_SIZE + 1) + 100);
        databaseFile << intToFiveDigitString(directory[i].indexStartBlock);
        bitMap(currentBlock, true, false);
        currentBlock++;
        databaseFile.flush();
    }
    databaseFile.flush();
}

void NoSQLDatabase::bitMap(int &currentBlock, bool isSet, bool initialize)
{
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
        databaseFile.seekp(((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) / BLOCK_SIZE + 4) * (BLOCK_SIZE + 1) + (currentBlock % BLOCK_SIZE));
        databaseFile << "1";
        isSet = false;
    }
    else
    {
        databaseFile.seekp(((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) / BLOCK_SIZE + 4) * (BLOCK_SIZE + 1) + (currentBlock % BLOCK_SIZE));
        databaseFile << "0";
    }
    databaseFile.flush();
}

int NoSQLDatabase::firstAvailableBlock()
{
    // to read the bitmap (seekg usage)
    int db = dbNumber;
    if (leftmost != "99999")
    {
        // means removal happened, update first available block in the corresponding db
        db = stoll(leftmost) / (INITIAL_SIZE / BLOCK_SIZE);
    }
    openOrCreateDatabase(databaseName, db);
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
                return ((i - 4) * BLOCK_SIZE + (INITIAL_SIZE / BLOCK_SIZE * db) + j);
            }
        }
    }
    return (INITIAL_SIZE / BLOCK_SIZE) * (db + 1) + (DIRECTORY_SIZE / BLOCK_SIZE); // after directory structure
}

bool NoSQLDatabase::isBlockAvailable(int &currentBlock)
{
    // Check if the block is available
    // 0 indicates a free block, 1 indicates that the block is allocated.
    // if this location belongs to the directory structure, we skip
    int db = currentBlock / (INITIAL_SIZE / BLOCK_SIZE);
    openOrCreateDatabase(databaseName, db);
    if ((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) < DIRECTORY_SIZE / BLOCK_SIZE)
    {
        cout << "Error: Invalid block number. It belongs to the directory structure." << endl;
        return false;
    }
    databaseFile.seekg(((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) / BLOCK_SIZE + 4) * (BLOCK_SIZE + 1) + (currentBlock % BLOCK_SIZE));
    // Check if the seek operation failed
    if (!databaseFile)
    {
        cout << "Error: Seek operation failed." << endl;
        return false;
    }
    char blockStatus;
    databaseFile >> blockStatus;
    // Check if the read operation failed
    if (!databaseFile)
    {
        cout << "Error: Read operation failed. " << databaseName << " dbNumber " << dbNumber << endl;
        cout << "Current block: " << currentBlock << endl;
        cout << "File position after seek: " << databaseFile.tellg() << endl;
        return false;
    }
    return blockStatus == '0';
}

string NoSQLDatabase::intToFiveDigitString(int number)
{
    // Convert an integer to a 5-digit string
    stringstream ss;
    ss << setw(5) << setfill('0') << number;
    return ss.str();
}

string NoSQLDatabase::intToEightDigitString(long long int number)
{
    // Convert an integer to a 8-digit string
    stringstream ss;
    ss << setw(8) << setfill('0') << number;
    return ss.str();
}

void NoSQLDatabase::writeDataBoundaries(string &data, int &currentBlock, int &currentPosInBlock)
{
    // current block: row, currentPosInBlock: column
    // adding 1 byte for newline character for formatting reasons

    // check if the current data file is full
    currentBlock = firstAvailableBlock();

    // Check if adding the record would exceed the block size
    // Each block is 256 bytes that includes a child block size of 4 bytes
    if (currentPosInBlock + DATA_RECORD_SIZE >= BLOCK_SIZE)
    {
        // Block is full, move to the next block
        bitMap(currentBlock, true, false);
        // if curr block = 4095 and it's marked as 1, we should create a new PFS file
        if (currentBlock == (INITIAL_SIZE / BLOCK_SIZE) * (dbNumber + 1) - 1)
        {
            // PFS file is full, create a new PFS file
            dbNumber++;
            openOrCreateDatabase(databaseName, dbNumber); // create the new PFS file
        }
        currentBlock = firstAvailableBlock();
        fcb.dataBlockUsed++;
        currentPosInBlock = 0;
    }
    databaseFile.seekp((currentBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + currentPosInBlock);
    databaseFile << data;
    // Ensure no overlapping of Data Blk and Index Blk
    if (currentPosInBlock != BLOCK_SIZE - 1)
    {
        databaseFile << string(BLOCK_SIZE - currentPosInBlock - DATA_RECORD_SIZE, ' ');
    }
    databaseFile.flush();
}

void NoSQLDatabase::handleIndexAllocation(int &currentBlock)
{
    // Index operations
    // if current PFS file is full, create a new PFS file
    // edge case: data block filled entire first PFS file
    if (currentBlock >= (INITIAL_SIZE / BLOCK_SIZE) * (dbNumber + 1))
    {
        // Current PFS is full
        dbNumber++;
        openOrCreateDatabase(databaseName, dbNumber); // create the new PFS file
    }
    // Index operations using B-tree
    // for testing purpose
    long long int currentBlk = currentBlock;
    int firstDb = dbNumber;
    bTree.Display(currentBlk, firstDb);
    fcb.indexStartBlock = bTree.getRootId(); // The starting block for the index (i.e. root)

    // if root is null, we ignore
    if (!bTree.getRootNode())
    {
        return;
    }
    int firstIndexBlock = currentBlock;
    string parent = "99999"; // parent block number
    queue<tuple<Node *, string>> q;
    q.push({bTree.getRootNode(), parent});

    int db = dbNumber;
    int level = 0;

    while (!q.empty())
    {
        int NodeCount = q.size();
        while (NodeCount > 0)
        {
            // current block number
            auto [node, parent] = q.front();
            q.pop();

            // index block number
            indexBlock = node->getNodeId() + firstIndexBlock;
            int currDb = firstDb + 1;
            while (indexBlock >= (INITIAL_SIZE / BLOCK_SIZE) * currDb)
            {
                indexBlock += (DIRECTORY_SIZE / BLOCK_SIZE);
                currDb++;
            }

            db = indexBlock / (INITIAL_SIZE / BLOCK_SIZE);
            if (db > dbNumber)
            {
                // update the biggest db number
                dbNumber = db;
            }
            openOrCreateDatabase(databaseName, db);

            if (isBlockAvailable(indexBlock))
            {
                databaseFile.seekp(indexBlock % (INITIAL_SIZE / BLOCK_SIZE) * (BLOCK_SIZE + 1));
                databaseFile << node->getChildKeyBlk();
                databaseFile << string(BLOCK_SIZE - node->getChildKeyBlk().size() - parent.size(), ' ');
                databaseFile.seekp((indexBlock % (INITIAL_SIZE / BLOCK_SIZE)) * (BLOCK_SIZE + 1) + (BLOCK_SIZE - 5));
                databaseFile << parent;
                bitMap(indexBlock, true, false);
                databaseFile.flush();
            }
            else
            {
                cout << "index Blk: " << indexBlock << " node Id: " << node->getNodeId() << " firstIndexBlock: " << firstIndexBlock << " db: " << db << endl;
            }

            // enqueue the children
            for (int i = 0; i < node->getChildrenSize(); i++)
            {
                if (node->getChildren()[i])
                {
                    q.push({node->getChildren()[i], intToFiveDigitString(indexBlock)});
                }
            }
            NodeCount--;
        }
        level++;
    }
}

string NoSQLDatabase::handleIndexSearch(string &idxStartBlock, string &key, int &blkAccessed, int &db)
{
    // idxBlkLine reads the index block
    string idxBlkLine;
    long long int idxStartBlk = stoll(idxStartBlock);
    long long int targetKey = stoll(key);
    long long int dataKey;

    openOrCreateDatabase(databaseName, db);

    databaseFile.seekg(idxStartBlk % (INITIAL_SIZE / BLOCK_SIZE) * (BLOCK_SIZE + 1));
    if (!databaseFile)
    {
        cout << "Error: Seek operation failed." << endl;
        return "99999";
    }
    // reading idxBlkLine without parent
    getline(databaseFile, idxBlkLine, ' ');
    for (int i = 5; i < idxBlkLine.size(); i += 18)
    {
        if (i + 8 < idxBlkLine.size())
        {
            dataKey = stoll(idxBlkLine.substr(i, 8));
            if (targetKey == dataKey)
            {
                // found the value (blk value)
                blkAccessed++;
                return idxBlkLine.substr(i + 8, 5);
            }
            else if (i - 5 >= 0 && targetKey < dataKey)
            {
                // return the child block number to search further
                blkAccessed++;
                return idxBlkLine.substr(i - 5, 5);
            }
        }
    }
    // return right child block number to search further
    blkAccessed++;
    return idxBlkLine.substr(idxBlkLine.size() - 5);
}

void NoSQLDatabase::handleIndexSearchForDelete(string &idxStartBlock, string &leftmost)
{
    stringstream ss;
    ss << idxStartBlock;
    int idxBlk = 0;
    ss >> idxBlk;

    // mark index block as free
    int db = idxBlk / (INITIAL_SIZE / BLOCK_SIZE);
    openOrCreateDatabase(databaseName, db);
    bitMap(idxBlk, false, false);

    // idxBlkLine reads the index block
    string idxBlkLine;
    databaseFile.seekg(idxBlk % (INITIAL_SIZE / BLOCK_SIZE) * (BLOCK_SIZE + 1));
    // reading idxBlkLine without parent
    getline(databaseFile, idxBlkLine, ' ');
    int resetBitmapPos;
    for (int i = 5; i < idxBlkLine.size(); i += 18)
    {
        if (i + 8 < idxBlkLine.size())
        {
            // data block #
            if (idxBlkLine.substr(i + 8, 5) < leftmost)
            {
                leftmost = idxBlkLine.substr(i + 8, 5);
            }
            string resetStr = idxBlkLine.substr(i + 8, 5);
            stringstream ss;
            ss << resetStr;
            ss >> resetBitmapPos;
            int resetDb = resetBitmapPos / (INITIAL_SIZE / BLOCK_SIZE);
            openOrCreateDatabase(databaseName, resetDb);
            bitMap(resetBitmapPos, false, false);
            openOrCreateDatabase(databaseName, db);
        }
        if (i - 5 >= 0)
        {
            // child block number
            string child = idxBlkLine.substr(i - 5, 5);
            if (child != "99999")
            {
                handleIndexSearchForDelete(child, leftmost);
            }
        }
    }
    string rightMostChild = idxBlkLine.substr(idxBlkLine.size() - 5);
    if (rightMostChild != "99999")
    {
        handleIndexSearchForDelete(rightMostChild, leftmost);
    }
}

void NoSQLDatabase::handleIndexSearchGetData(string &idxStartBlock, set<string> &datablocks)
{
    // idxBlkLine reads the index block
    string idxBlkLine;
    stringstream ss;
    ss << idxStartBlock;
    int currBlk = 0;
    ss >> currBlk;

    int db = currBlk / (INITIAL_SIZE / BLOCK_SIZE);
    openOrCreateDatabase(databaseName, db);
    databaseFile.seekg(currBlk % (INITIAL_SIZE / BLOCK_SIZE) * (BLOCK_SIZE + 1));
    // reading idxBlkLine without parent
    getline(databaseFile, idxBlkLine, ' ');
    for (int i = 5; i < idxBlkLine.size(); i += 18)
    {
        if (i + 8 < idxBlkLine.size())
        {
            // data block #
            string dataBlk = idxBlkLine.substr(i + 8, 5);
            datablocks.insert(dataBlk);
        }
        if (i - 5 >= 0)
        {
            // child block number
            string child = idxBlkLine.substr(i - 5, 5);
            if (child != "99999")
            {
                handleIndexSearchGetData(child, datablocks);
            }
        }
    }
    string rightMostChild = idxBlkLine.substr(idxBlkLine.size() - 5);
    if (rightMostChild != "99999")
    {
        handleIndexSearchGetData(rightMostChild, datablocks);
    }
}

void NoSQLDatabase::openOrCreateDatabase(string &PFSFile, int dbNumber)
{
    if (PFSFile.empty())
    {
        cout << "Please include a database name." << endl;
        return;
    }
    // close existing database file before opening others or creating other databases
    if (databaseFile.is_open())
    {
        databaseFile.close();
    }
    databaseName = PFSFile;
    // Open the database file or Create if db not exist
    string filePath = databaseName + ".db" + to_string(dbNumber);
    if (exists(filePath))
    {
        // open file
        databaseFile.open(filePath, ios::out | ios::in);
        // check file path
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to open database file." << endl;
            return;
        }
    }
    else
    {
        cout << "Creating database file " << filePath << endl;
        // Open the database file
        databaseFile.open(filePath, ios::out);
        if (!databaseFile.is_open())
        {
            cout << "Error: Unable to create a file." << endl;
            return;
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
    }
}

void NoSQLDatabase::putDataIntoDatabase(string &myFile)
{
    // Insert data from OS file into NoSQL database
    // Accessing the databaseName member variable
    if (databaseName.empty())
    {
        cout << "Error: No database is open." << endl;
        return;
    }
    cout << "Putting data into database " << databaseName << " from file " << myFile << endl;
    ifstream fileToRead(myFile);
    if (fileToRead.peek() == ifstream::traits_type::eof())
    {
        cout << "Error: File is empty." << endl;
        return;
    }
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

    // Initialize B-tree index
    bTree = BTree(INDEX_BFR);

    // Initialize FCB information
    fcb.fileSize = file_size(myFile);  // The actual file size
    fcb.timestamp = time(nullptr);     // Set the timestamp to current time
    fcb.dataStartBlock = currentBlock; // The starting block
    fcb.dataBlockUsed = 1;             // Number of blocks used

    // search fcb files
    int lineNumber = 1;
    string fcbs;
    while (getline(databaseFile, fcbs))
    {
        if (fcbs.substr(0, fcbs.find(' ')) == myFile)
        {
            myFile = myFile.substr(0, myFile.find('.')) + "_copy" + myFile.substr(myFile.find('.'));
            cout << "myFile already exists. Renaming to " << myFile << endl;
            break;
        }
        lineNumber++;
        if (lineNumber == 4)
        {
            break;
        }
    }

    // data block starts after the directory structure
    currentBlock = firstAvailableBlock();
    currentPosInBlock = 0;

    // Initialize FCB information
    fcb.filename = myFile;

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

            // Convert the key to the appropriate data type (e.g., integer)
            // concatenate the unpadded (supposingly 8 byte) key with current block number
            // when presenting in the index block, we will pad the key to 8 bytes, e.g. 00000002
            // e.g. key = 2, currentBlock = 00001, key = 200001, int key = 200001
            string keyStr = keyString + intToFiveDigitString(currentBlock);
            long long int key = stoll(keyStr);

            // Index the key using B-tree
            bTree.Insert(key);

            currentPosInBlock += DATA_RECORD_SIZE; // each individual block
            fcb.timestamp = time(nullptr);         // update the timestamp
        }
    }

    // set the last data block bitmap to 1
    bitMap(currentBlock, true, false);

    // start of index blocks
    currentBlock = firstAvailableBlock();

    // start of index blocks
    handleIndexAllocation(currentBlock);

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

void NoSQLDatabase::getDataFromDatabase(string &myFile)
{
    // Download data file from NoSQL database to OS directory
    openOrCreateDatabase(databaseName, dbNumber);
    if (directory.empty())
    {
        cout << "No files found in the database." << endl;
        return;
    }

    // check if the file exists in the database

    // skip metadata
    string metadata;
    getline(databaseFile, metadata);

    // read fcb
    string fcb;
    int lineNumber = 1;
    string rootBlk;
    while (getline(databaseFile, fcb))
    {
        if (fcb.empty())
        {
            lineNumber++;
            cout << "fcb empty block." << endl;
            continue;
        } else if (fcb.substr(0, fcb.find(' ')) == myFile)
        {
            rootBlk = fcb.substr(100, 5);
            break;
        }
        lineNumber++;
        if (lineNumber == 4)
        {
            cout << "No such file found. Cannot download data." << endl;
            return;
        }
    }

    // create the file to write
    ofstream fileToWrite(myFile);
    if (!fileToWrite.is_open())
    {
        cout << "Error: Unable to open file " << myFile << endl;
        return;
    }

    set<string> datablocks;
    handleIndexSearchGetData(rootBlk, datablocks);

    // read data block
    for (auto &blk : datablocks)
    {
        int db = stoll(blk) / (INITIAL_SIZE / BLOCK_SIZE);
        openOrCreateDatabase(databaseName, db);
        databaseFile.seekg(stoll(blk) % (INITIAL_SIZE / BLOCK_SIZE) * (BLOCK_SIZE + 1));
        string line;
        if (getline(databaseFile, line))
        {
            // each line only contains 1 record (40 bytes)
            for (int i = 0; i < line.size(); i += DATA_RECORD_SIZE)
            {
                // output record to the file
                fileToWrite << line.substr(i, DATA_RECORD_SIZE) << endl;
            }
        }
    }
    fileToWrite.flush();
    fileToWrite.close();
}

void NoSQLDatabase::delFileFromDatabase(string &myFile)
{
    // rm the PFS files if there's extra
    // 1. update directory structure, remove fcb
    // 2. update bitmap, set originally used block = 0
    // 3. remove the index blocks correlated to movies-1.csv
    // 4. data file blocks remain the same and just gets overwritten if new data comes in by checking bitmap =0 (Free)
    if (directory.empty())
    {
        cout << "No files found in the database." << endl;
        return;
    }
    string rootStartblk;
    for (int db = 0; db <= dbNumber; db++)
    {
        openOrCreateDatabase(databaseName, db);

        // skip metadata
        string metadata;
        getline(databaseFile, metadata);

        // search fcb files
        int lineNumber = 1;
        string line;

        while (getline(databaseFile, line))
        {
            if (line.empty())
            {
                lineNumber++;
                continue;
            }
            string fcbFileName = line.substr(0, line.find(" "));
            if (fcbFileName == myFile)
            {
                rootStartblk = line.substr(100, 5);
                databaseFile.seekp(lineNumber * (BLOCK_SIZE + 1));
                databaseFile << string(line.size(), ' ');
                databaseFile.flush();
                bitMap(lineNumber, false, false);
                cout << "Removed " << myFile << " from db " << db << endl;
                break;
            }
            lineNumber++;
            if (lineNumber == 4)
            {
                cout << "No such file found. Cannot delete file." << endl;
                return;
            }
        }
    }
    // remove from directory
    for (int i = 0; i < directory.size(); i++)
    {
        if (directory[i].filename == myFile)
        {
            directory.erase(directory.begin() + i);
            break;
        }
    }
    // index search
    handleIndexSearchForDelete(rootStartblk, leftmost);
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

void NoSQLDatabase::findValueFromDatabase(string &myFileKey, int &blkAccessed)
{
    if (!myFileKey.find("."))
    {
        cout << "Invalid command, it should be formatted `find myFile.key`." << endl;
        return;
    }
    string idxStartBlk;

    string myFile = myFileKey.substr(0, myFileKey.find("."));
    string keyStr = myFileKey.substr(myFileKey.find(".") + 1, 8);
    for (char c : keyStr)
    {
        if (!isdigit(c))
        {
            cout << "Invalid key. Key should be a number." << endl;
            return;
        }
    }
    long long int inputKey = stoll(keyStr);
    string key = intToEightDigitString(inputKey);

    openOrCreateDatabase(databaseName, dbNumber);

    // skip metadata
    string metadata;
    getline(databaseFile, metadata);

    // search fcb files
    int lineNumber = 1;
    string line;
    while (getline(databaseFile, line))
    {
        if (line.empty())
        {
            lineNumber++;
            continue;
        }
        string fcbFileName = line.substr(0, line.find("."));
        blkAccessed++;
        if (fcbFileName == myFile)
        {
            idxStartBlk = line.substr(100, 5);
            break;
        }
        lineNumber++;
        if (lineNumber == 4)
        {
            cout << "No such file found. Cannot find value." << endl;
            return;
        }
    }

    // search data block
    string idxBlk = idxStartBlk;
    string dataBlk;
    string resultBlk = "99999";
    bool found = false;
    while (!found)
    {
        long long int intIdxBlk = stoll(idxBlk);
        int db = intIdxBlk / (INITIAL_SIZE / BLOCK_SIZE);
        idxBlk = handleIndexSearch(idxBlk, key, blkAccessed, db);
        for (char c : idxBlk)
        {
            if (!isdigit(c))
            {
                resultBlk = dataBlk;
                found = true;
                break;
            }
        }
        if (found)
        {
            break;
        }
        if (idxBlk == "99999")
        {
            cout << "No key found." << endl;
            break;
        }
        dataBlk = idxBlk;
    }
    if (resultBlk == "99999")
    {
        blkAccessed = 0;
        return;
    }
    // result Blk search data block
    string record;
    string exactKey = to_string(inputKey) + ',';
    databaseFile.seekg(stoll(resultBlk) % (INITIAL_SIZE / BLOCK_SIZE) * (BLOCK_SIZE + 1));
    if (getline(databaseFile, line))
    {
        record = line.substr(line.find(exactKey), DATA_RECORD_SIZE);
        // output record to the console
        cout << record << endl;
    }
    cout << endl;
    // includes the fcb block, index blocks, and data blocks
    cout << "# of Blocks = " << blkAccessed << endl;
    blkAccessed = 0;
}

void NoSQLDatabase::killDatabase(string &PFSFile)
{
    // Delete the NoSQL database
    // e.g. rm PFSFile
    if (PFSFile.empty())
    {
        cout << "Please include a database name." << endl;
        return;
    }
    databaseName = PFSFile;
    cout << "Deleting database " << databaseName << "from db0 up to db" << dbNumber << endl;

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
                cout << "File " << filePath << " deleted successfully." << endl;
            }
        }
        else
        {
            cout << "File " << filePath << " does not exist." << endl;
        }
    }
    directory.clear();
    // restart the dbNumber
    dbNumber = 0;
}

void NoSQLDatabase::quitDatabase()
{
    // Exit NoSQL database
    exit(0);
}

NoSQLDatabase::~NoSQLDatabase()
{
    // Close the database file
    if (databaseFile.is_open())
    {
        databaseFile.close();
    }
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
    string query;

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
            query = command.substr(command.find(" ") + 1, command.length());
        }

        // Execute actions based on the command keyword
        switch (getCommandType(keyword))
        {
        case OPEN:
            // file here is the database
            openOrCreateDatabase(query, 0);
            break;
        case PUT:
            // file here is the file to be put into the database
            putDataIntoDatabase(query);
            break;
        case GET:
            getDataFromDatabase(query);
            break;
        case RM:
            delFileFromDatabase(query);
            break;
        case DIR:
            listAllDataFromDatabase();
            break;
        case FIND:
            findValueFromDatabase(query, blkAccessed);
            break;
        case KILL:
            killDatabase(query);
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
