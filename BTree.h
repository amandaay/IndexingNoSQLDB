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

public:
    // Node Constructor Declaration: initiates a new node
    Node(int _degree, bool _leaf);
    // nodeLookup(int value) - search the index of the value in the specific node
    int NodeLookup(int value);
    /*
     * nodeInsert(int value, int pointer)
     *    - -2 if the value already exists in the specified node
     *    - -1 if the value is inserted into the node or
     *            something else if the parent node has to be restructured
     */
    void NodeInsert(int value);
    // split child helper function if current node is full
    void SplitChild(int i, Node* CurrNode);
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
    // //  Number of currently used nodes
    // int CntNodes;
    // // number of currently used values
    // int CntValues;
    // degree
    int degree;

public:
    // constructor for BTree
    BTree(int _degree);
    // Lookup(int value)
    // - True if the value was found.
    bool Lookup(Node *root, int value);
    // Public method to access the root nodes
    Node *getRootNode();
    // Insert(int value)
    void Insert(int value);
    // Display of the entire tree
    void Display(bool Lookup);
    // deconstructor
    ~BTree();
};

#endif // BTREE_H