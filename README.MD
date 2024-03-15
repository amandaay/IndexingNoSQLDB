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

## Source

https://www.baeldung.com/cs/b-trees-vs-btrees <br/>
https://www.programiz.com/dsa/b-tree<br/>
https://stackoverflow.com/questions/28846377/what-is-the-difference-btw-order-and-degree-in-terms-of-tree-data-structure<br/>
https://stackoverflow.com/questions/50800605/using-binary-search-in-b-tree#:~:text=In%20practice%2C%20a%20linear%20search,followed%20by%20a%20linear%20search.<br/>

# Project 2: Developing Key-Value NoSQL database with Indexing
### A continuation of project1


## Authors

So Man Amanda Au-Yeung, NUID: 001673193<br/>
Chin Yuen Au (Isaac), NUID: 002639665<br/>

### Grading TA
Moreno Gong (Gongtianchou)

## Demo Details
Group 1
