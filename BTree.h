/**
 * @file BTree.h
 * @author So Man Amanda Au-Yeung, Chin Yuen Au (Isaac)
 * @brief CS7280 Special Topics in Database Management
 *        Project 1: B-tree implementation.
 * @date due 2024-02-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BTREE_H
#define BTREE_H
#include <vector>
using namespace std;

/*
 * Node data structure.
 *   - This is the simplest structure for nodes used in B-tree
 *   - This will be used for both internal and leaf nodes.
 */
class Node
{
private:
    // Node Values (Leaf Values / Key Values for the children nodes).
    int *values;
    // Children ptrs from current node
    Node **children;
    // Number of value
    int size;
    // indicates if the current node is a leaf node
    bool leaf;
    // Node Id
    int NodeId;
    // nodesize
    int nodesize;
    // string for child key block
    string childKeyBlk;

public:
    // Node Constructor Declaration: initiates a new node
    Node(int nodesize, bool _leaf);
    // nodeLookup(int value) - search the index of the value in the specific node
    int NodeLookup(int value);
    // getter for nodeId
    int getNodeId();
    // getter for childKeyBlk chain
    string getChildKeyBlk();
    // getter for children nodes
    Node **getChildren();
    // getter for size of the children nodes
    int getChildrenSize();
    // Node insert values to the specific node
    void NodeInsert(int value, int &NodeIdCounter);
    // split child helper function if current node is full
    void SplitChild(int i, Node *CurrNode, int &NodeIdCounter);
    // to convert child to 5 digit string
    string intToFiveDigitString(int number);
    // to convert key+block# to 8+5 digit string
    string intToThirteenDigitString(int number);
    // Display node
    void Display(int _size, int &currentBlock);
    // deconstructor
    ~Node();

    // allow access of Node class for BTree
    friend class BTree;
};

/**
 * @brief BTree class for building the BTree
 *
 */
class BTree
{
private:
    // Node array, including the root nodes
    Node *nodes;
    // NodeIdCounter
    int NodeIdCounter;
    // nodesize --> blocking factor (bfr)
    int nodesize;
    // count layers if there's more than 2 layers meaning internal nodes exist
    int layers;
    // root node ID
    int rootId;

public:
    // constructor for BTree
    BTree(int nodesize);
    // Lookup - True if the value was found.
    bool Lookup(Node *root, int value, vector<int> &NodeIds);
    // Search for the full ndoes
    bool Lookup(Node *root, int value, vector<Node* > &FullNodes);
    // Public method to access the root nodes
    Node *getRootNode();
    // Get root node ID
    int getRootId();
    // Setter method for the root node pointer
    void setRootNode(Node *root);
    // helper function for inserting a new value
    Node *SplitRoot(Node *node, int value);
    // Insert specific value to the tree
    void Insert(int value);
    // Display of the entire tree
    int Display(int &currentBlock);
    // deconstructor
    ~BTree();
};

#endif // BTREE_H