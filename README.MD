# B Tree Implementation

## How to run?

1. `git clone https://github.com/amandaay/IndexingNoSQLDB.git`
2. To compile, `clang++ -std=c++11 BTree.cpp -o <name-you-want-to-give>` or `g++ -std=c++11 BTree.cpp BTreeTest.cpp -o BTreeTest <name-you-want-to-give>`
3. `./BTreeTest <testFile.txt> <NODESIZE></NODESIZE>`, note that `<name-you-want-to-give>` = `BTreeTest` and default NODESIZE=5

## Instructions
1. In this program, user will have to feed in a textfile similar to `DemoTestFile.t` to implement a BTree. i.e. The textfile must be a set of integers separated by commas.
2. After building the BTree, the user can input values to be searched.

## Known bugs

## Limitations

## Assumptions
1. If user does not provide a nodesize, the default value will be 5. <br/>
2. Note that Nodesize is equivalent to Knuth order of a B Tree.

## Source

https://www.baeldung.com/cs/b-trees-vs-btrees <br/>
https://www.programiz.com/dsa/b-tree<br/>
https://stackoverflow.com/questions/28846377/what-is-the-difference-btw-order-and-degree-in-terms-of-tree-data-structure<br/>
https://stackoverflow.com/questions/50800605/using-binary-search-in-b-tree#:~:text=In%20practice%2C%20a%20linear%20search,followed%20by%20a%20linear%20search.<br/>

## Author

So Man Amanda Au-Yeung, NUID: 001673193<br/>
Chin Yuen Au (Isaac)
