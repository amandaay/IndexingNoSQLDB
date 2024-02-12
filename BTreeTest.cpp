/**
 * @file BTreeTest.cpp
 * @author So Man Amanda Au-Yeung, Chin Yuen Au (Isaac)
 * @brief CS7280 Special Topics in Database Management
 *        Project 1: B-tree Test program.
 * @date due 2024-02-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "BTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]) {
    // default size
    int Nodesize = 5;
    // check if there's an input file with the same arg length
    if (argc != 2 && argc != 3) {
        cerr << "Usage: " << argv[0] << " <test-file> [<Nodesize(default=5)>]" << endl;
        return 1;
    }
    if (argc == 3) {
        // convert the input string to its nodesize
        istringstream(argv[2]) >> Nodesize;
    }
    // parse the filename
    string filename = argv[1];
    ifstream testFile(filename);
    // check if we are able to open the test file
    if (!testFile.is_open()) {
        cerr << "Error: Unable to open test file." << endl;
        return 1;
    }
    // parsing file one tree per line
    // from Knuth Order to CLRS degree
    // ref: https://stackoverflow.com/questions/28846377/what-is-the-difference-btw-order-and-degree-in-terms-of-tree-data-structure
    BTree DemoTree(static_cast<int>(ceil(Nodesize / 2.0)));
    cout << "Creating BTree..." << endl;
    string line;
    int value;
    while (getline(testFile, line)) {
        istringstream iss(line);
        string token;
        while (getline(iss, token, ',')) {
            int value = stoi(token);
            DemoTree.Insert(value);
            cout << value << endl;
            // DemoTree.Display();
        }
    }
    // close test file
    testFile.close();
    // test lookup function
    char choice;
    cout << "Would you like to search a value in your BTree? (y/n)";
    cin >> choice;
    choice = tolower(choice);
    if (choice != 'y' && choice != 'n') {
        cerr << "Invalid Choice";
    }
    if (choice == 'y') {
        int LookUpVal;
        cout << "Enter the value you want to lookup (ctrl+c to kill the program): ";
        while (cin >> LookUpVal) {
            cout << (DemoTree.Lookup(DemoTree.getRootNode(), LookUpVal) ? "FOUND" : "NOT FOUND") << endl;
        }
    }
    else {
        cout << "Exiting the program";
    }
    return 0;
}