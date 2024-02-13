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

int main(int argc, char *argv[])
{
    // default size
    int Nodesize = 5;
    // check if there's an input file with the same arg length
    if (argc != 2 && argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <test-file> [<Nodesize(default=5)>]" << endl;
        return 1;
    }
    if (argc == 3)
    {
        // convert the input string to its nodesize
        istringstream(argv[2]) >> Nodesize;
    }
    // parse the filename
    string filename = argv[1];
    ifstream testFile(filename);
    // check if we are able to open the test file
    if (!testFile.is_open())
    {
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
    while (getline(testFile, line))
    {
        istringstream iss(line);
        string token;
        while (getline(iss, token, ','))
        {
            int value = stoi(token);
            DemoTree.Insert(value);
            // TODO: DemoTree.Display();
        }
    }

    bool quit = false;
    char choice;

    while (!quit)
    {
        cout << "(1) Look-up, (2) Insert, or (q) Quit?";
        cin >> choice;
        switch (choice)
        {
        case '1':
        {
            int LookUpVal;
            cout << "What key are you searching for?";
            cin >> LookUpVal;
            if (DemoTree.Lookup(DemoTree.getRootNode(), LookUpVal))
            {
                // TODO: Display
            }
            else
            {
                cout << "No key found" << endl;
            }
            break;
        }
        case '2':
        {
            cout << "What key do you insert?";
            int InsertVal;
            cin >> InsertVal;
            DemoTree.Insert(InsertVal);
            break;
        }
        case 'q':
        {
            cout << "Good bye!!";
            quit = true;
            break;
        }
        default:
        {
            cout << "Invalid choice" << endl;
            break;
        }
        }
    }

    // close test file
    testFile.close();
    return 0;
}