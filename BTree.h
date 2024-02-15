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
    // CLRS degree of the node, i.e. minimum number of children per node
    int degree;
    // Number of entries (Rule in B Trees: d <= size <= 2 * d)
    int size;
    // indicates if the current node is a leaf node
    bool leaf;
    // Node Id
    int NodeId;

public:
    // Node Constructor Declaration: initiates a new node
    Node(int _degree, bool _leaf);
    // nodeLookup(int value) - search the index of the value in the specific node
    int NodeLookup(int value);
    // getter for nodeId
    int getNodeId();
    // Node insert values to the specific node
    void NodeInsert(int value, int &NodeIdCounter);
    // split child helper function if current node is full
    void SplitChild(int i, Node *CurrNode, int &NodeIdCounter);
    // Display node
    void Display(int _size, int NodeId);
    // deconstructor
    ~Node();

    // allow access of Node class for BTree
    friend class BTree;
};

class BTree
{
private:
    // Node array, including the root nodes
    Node *nodes;
    // degree
    int degree;
    // NodeIdCounter
    int NodeIdCounter;

public:
    // constructor for BTree
    BTree(int _degree);
    // Lookup - True if the value was found.
    bool Lookup(Node *root, int value, vector<int> &NodeIds);
    // Public method to access the root nodes
    Node *getRootNode();
    // Setter method for the root node pointer
    void setRootNode(Node *root);
    // Insert specific value to the tree
    void Insert(int value);
    // Display of the entire tree
    void Display();
    // deconstructor
    ~BTree();
};

#endif // BTREE_H