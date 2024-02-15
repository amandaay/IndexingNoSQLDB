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
#include <vector>

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
        }
        // creating a tree
        DemoTree.Display();
    }

    bool quit = false;
    char choice;

    while (!quit)
    {
        cout << "(1) Look-up, (2) Insert, or (q) Quit? ";
        cin >> choice;
        Node *root = DemoTree.getRootNode();
        switch (choice)
        {
        case '1':
        {
            string StrLookUpVal;
            cout << "What key are you searching for? ";
            cin >> StrLookUpVal;
            int LookUpVal;
            try
            {
                LookUpVal = stoi(StrLookUpVal);
            }
            catch (const invalid_argument &e)
            {
                cout << "Invalid input. Please enter an integer." << endl;
                break;
            }
            vector<int> NodeIds;
            if (DemoTree.Lookup(root, LookUpVal, NodeIds))
            {
                for (auto j = NodeIds.begin(); j != NodeIds.end(); j++)
                {
                    cout << *j;
                    // Check if j is not pointing to the last element
                    if (j != NodeIds.end() - 1)
                    {
                        cout << " -> ";
                    }
                }
                cout << endl;
            }
            else
            {
                cout << "No key found." << endl;
            }
            DemoTree.setRootNode(root);
            break;
        }
        case '2':
        {
            string StrInsertVal;
            cout << "What key do you insert? ";
            cin >> StrInsertVal;
            int InsertVal;
            try
            {
                InsertVal = stoi(StrInsertVal);
            }
            catch (const invalid_argument &e)
            {
                cout << "Invalid input. Please enter an integer." << endl;
                break;
            }
            DemoTree.Insert(InsertVal);
            DemoTree.Display();
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