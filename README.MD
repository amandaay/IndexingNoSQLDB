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

2. Each data record has a maximum of 40 bytes, bfr = 256/40 = 6 records

3. Each block has a maximum of 256 bytes

4. Index block allocation assumption:
- child block # 5 bytes
- key, block # (8 bytes, 5 bytes)
- index blocking factor (bfri) for index block: (256 - 5) / (8 + 5 + 5) = 13 key

5. Directory Structure Assumptions:

- Database name: 50 bytes
- Total size: 10 bytes
- Number of total files (PFS): 2 bytes
- Block size: 3 bytes
- Number of uploaded files: 1 byte

Assume that there are 3 FCBs:
- filename: 50 bytes
- filesize: 10 bytes
- date/time: 20 bytes
- start block: 5 bytes
- Number of blocks used: 5 bytes
- Starting block for index (i.e. root): 5 bytes

Bit Map:
- Number of blocks = 1 Mbytes / block size = 1024 * 1024 / 256 = 4096 blocks
- Each block has 1 or 0. (bit)
- 0 indicates a free block, 1 indicates that the block is allocated.
- Total blocks in a PFS / BLOCK_SIZE = 4096 / 256 = 16 blocks
- Thus, we will use 16 blocks to represent.

- Assume that metadata uses 1 block, each FCB uses 1 block (3 * 1 block), bitmap uses 16 blocks
- The directory structure takes around 20 blocks.

## Authors

So Man Amanda Au-Yeung, NUID: 001673193<br/>
Chin Yuen Au (Isaac), NUID: 002639665<br/>

### Grading TA

Moreno Gong (Gongtianchou)

## Demo Details

Group 1
