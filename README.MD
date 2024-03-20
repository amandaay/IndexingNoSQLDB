# Project 1: B Tree Implementation

## How to run?

1. `git clone https://github.com/amandaay/IndexingNoSQLDB.git`
2. To compile, `clang++ -std=c++17 BTree.cpp BTreeTest.cpp -o <name-you-want-to-give>` or `g++ -std=c++17 BTree.cpp BTreeTest.cpp -o <name-you-want-to-give>`
3. `./<name-you-want-to-give> <testFile.txt> <NODESIZE>`, e.g. `<name-you-want-to-give>` = `BTreeTest`

## Instructions

1. In this program, user will have to feed in a textfile similar to `DemoTestFile.txt` to implement a BTree. i.e. The textfile must be a set of integers separated by commas.
2. After building the BTree, the user can input values to be searched.

## Known bugs

No known bugs so far.

## Limitations

1. NodeId cannot be -1 according to our implementation.

## Assumptions

1. If user does not provide a nodesize, the default value will be 5. <br/>
2. NodeId cannot be -1. <br/>
3. Assume keys are unique without duplicates.
4. Assume nodesize is greater than 2.

## Source

https://www.baeldung.com/cs/b-trees-vs-btrees <br/>
https://www.programiz.com/dsa/b-tree<br/>
https://stackoverflow.com/questions/28846377/what-is-the-difference-btw-order-and-degree-in-terms-of-tree-data-structure<br/>
https://stackoverflow.com/questions/50800605/using-binary-search-in-b-tree#:~:text=In%20practice%2C%20a%20linear%20search,followed%20by%20a%20linear%20search.<br/>

# Project 2: Developing Key-Value NoSQL database with Indexing

### A continuation of project1

## How to run??

1. To compile, `g++ -std=c++17 NoSQLDatabase.o BTree.cpp main.o -o NoSQLDb` or `clang++ -std=c++17 NoSQLDatabase.cpp BTree.cpp main.cpp -o NoSQLDb`
2. To run the program, `./NoSQLDb`.
3. Prompt `NoSQL > ` will be shown to enter desired commands.

## Assumptions

1. Test file should always have:

- Header
- Endline ("\n") at the end of each line or record
- First character or integer before the first comma (",") is our key

2. Each data record has a maximum of 40 bytes including the endline ("\n")

3. Each block has a maximum of 256 bytes

4. Index block allocation assumption:
- child block # (integer) 4 bytes
- key, block # (integer, integer) (4 bytes, 4 bytes)
- blocking factor for index block: (256 - 4) / (4 * 3) = 21 bytes

5. Directory Structure Assumptions:

- Blocksize: 4 bytes
- Database name: 24 bytes
- Total size: 4 bytes
- Number of total files (PFS): 4 bytes
- Number of records of data file: 4 bytes
- Total metadata size = 40 bytes

Assume that there are 4 FCBs:
- filename: 24 bytes
- filesize: 4 bytes
- time: 8 bytes
- start block: 4 bytes
- Index file info: 4 bytes (Root pointer of BTree)
- Total (4 FCBs) = 44 * 4 = 176 bytes

Bit Map: 16 bytes
- Number of blocks = 1 Mbytes / block = 1024 * 1024 / 256 = 4096 blocks
- Each block has 1 or 0. (bit)
- 1 indicates free block, 0 indicates that the block is allocated.
- Thus, we have a total of 4096 bit.
- To convert to bytes = 4096 / 8 = 512 bytes. (Equivalent to around 2 blocks.)

- Directory uses up around 728 bytes (~ 3 block sizes), so we will reserve 3 * 256 = 768 bytes for the directory structure. 

## Authors

So Man Amanda Au-Yeung, NUID: 001673193<br/>
Chin Yuen Au (Isaac), NUID: 002639665<br/>

### Grading TA

Moreno Gong (Gongtianchou)

## Demo Details

Group 1
