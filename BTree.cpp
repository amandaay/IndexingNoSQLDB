/**
 * @file BTree.cpp
 * @author So Man Amanda Au-Yeung, Chin Yuen Au (Isaac)
 * @brief CS7280 Special Topics in Database Management
 *        Project 1: B-tree implementation.
 * @date due 2024-02-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "BTree.h"
#include <iostream>
#include <queue>
#include <cmath>

using namespace std;

/**
 * @brief Node definition Construct a new Node:: Node object
 * @param _nodesize of the BTree
 * @param _leaf if it's leafnode or not
 *
 */
Node::Node(int _nodesize, bool _leaf) : nodesize(_nodesize), leaf(_leaf), NodeId(-1)
{
    values = new int[nodesize];
    // initialize node pointers for the Node's children ptr
    children = new Node *[nodesize + 1];
    // initialize number of values = 0
    size = 0;
}

/**
 * @brief search value in the specific node
 * @return the search value if exist, otherwise leftmost index
 *
 */
int Node::NodeLookup(int value)
{
    // if node does not exit, you cannot find the existing value
    if (size <= 0)
    {
        return -1;
    }
    // if size > 0, perform binary search within a node
    int left = 0, right = size;
    while (left < right)
    {
        int mid = left + (right - left) / 2;
        if (values[mid] < value)
        {
            left = mid + 1;
        }
        else
        {
            right = mid;
        }
    }
    return left;
}

/**
 * @brief return nodeId
 *
 * @return int return the NodeId
 */
int Node::getNodeId()
{
    return NodeId;
}

/**
 * @brief assuming the leaf node is non full
 * insertion of the dedicated value will be performed
 *
 */
void Node::NodeInsert(int value, int &NodeIdCounter)
{
    // binary search and insert where the element should be at
    // search the index if exist and return null, otherwise where it should be
    int i = NodeLookup(value);
    if (values[i] == value)
    {
        // value already exist, nothing is inserted
        return;
    }
    if (leaf)
    {
        // shift all the other values towards the right until it reaches the index to be inserted
        for (int j = size; j > i; j--)
        {
            values[j] = values[j - 1];
        }
        values[i] = value;
        size++;
    }
    else
    {
        if (children[i]->values[i] == value)
        {
            return;
        }
        // non leaf
        // see index to be inserted is full or not
        if (children[i]->size == nodesize)
        {
            SplitChild(i, children[i], NodeIdCounter);
            if (values[i] < value)
            {
                i++;
            }
        }
        children[i]->NodeInsert(value, NodeIdCounter);
    }
}

/**
 * @brief splitting child when current node is full
 *
 * @param CurrNode node ptr to current node
 * @param NodeIdCounter is to keep track of the to the NodeId count
 *
 */
void Node::SplitChild(int i, Node *CurrNode, int &NodeIdCounter)
{
    // Create a new node that will be the right sibling of CurrNode
    Node *newNode = new Node(nodesize, CurrNode->leaf);
    newNode->NodeId = NodeIdCounter++;
    // Calculate the index to split at
    int splitIndex = (nodesize % 2) ? nodesize / 2 : nodesize / 2 - 1;
    // Copy the right half of CurrNode's values and children to newNode
    newNode->size = nodesize - splitIndex - 1; // Subtract 1 for the middle value
    for (int j = 0; j < newNode->size; j++)
    {
        newNode->values[j] = CurrNode->values[j + splitIndex + 1]; // Start from the element after the middle value
    }

    if (!CurrNode->leaf)
    {
        for (int j = 0; j < newNode->size + 1; j++)
        {
            newNode->children[j] = CurrNode->children[j + splitIndex + 1]; // Start from the child after the middle value
        }
    }

    // Reduce the size of CurrNode
    CurrNode->size = splitIndex;
    // Insert the middle value of CurrNode into the parent node at index i
    for (int j = size; j > i; j--)
    {
        children[j + 1] = children[j];
    }
    children[i + 1] = newNode;

    // Move the middle value of CurrNode to the parent node
    for (int j = size - 1; j >= i; j--)
    {
        values[j + 1] = values[j];
    }
    values[i] = CurrNode->values[splitIndex];
    size++;
}

/**
 * @brief displaying every single done with its node index
 *
 * @param _size current size of the nodes
 * @param NodeId node ID's index
 */
void Node::Display(int _size, int NodeId)
{
    int i = 0;
    for (i = 0; i < _size; i++)
    {
        if (i != _size - 1)
        {
            cout << values[i] << ",";
        }
        else
        {
            cout << values[i];
        }
    }
    // format ","s per requirement
    int CurrSize = i;
    while (CurrSize < nodesize)
    {
        cout << ",";
        CurrSize++;
    }
}

/**
 * @brief Destroy the Node:: Node object
 *
 */
Node::~Node()
{
    for (int i = 0; i < size; i++)
    {
        if (children[i] != nullptr)
        {
            delete children[i];
        }
    }
    delete[] children;
    delete[] values;
}

/**
 * @brief BTree constructor
 * Construct a new BTree::BTree object
 * @param _nodesize user input nodesize
 *
 */
BTree::BTree(int _nodesize) : nodesize(_nodesize), nodes(nullptr), NodeIdCounter(0), layers(-1) {}

/**
 * @brief Search the lookup value
 *
 * @param root root node of every recursive function of the tree
 * @param value search value
 * @param NodeIds a list of node ids
 *
 * @return true if search value found else return false
 */
bool BTree::Lookup(Node *root, int value, vector<int> &NodeIds)
{
    nodes = root;
    // if root node is null, then we cannot perform search
    if (!nodes)
    {
        return false;
    }
    int i = nodes->NodeLookup(value);
    NodeIds.push_back(nodes->getNodeId());
    // check the current node if the search value exist
    if (i != -1 && nodes->values[i] == value)
    {
        return true;
    }
    // if it reaches leaf node, it indicates there's no such value
    if (nodes->leaf)
    {
        return false;
    }
    return Lookup(nodes->children[i], value, NodeIds);
}

/**
 * @brief Search the lookup value to reach the closest node
 *        and append the nodes that are full to the FullNodes vector
 *
 * @param root root node of every recursive function of the tree
 * @param value search value
 * @param FullNodes a list of node ids
 *
 * @return true if reaches leaf node else value exist
 *         false if value does not exist
 */
bool BTree::Lookup(Node *root, int value, vector<Node *> &FullNodes)
{
    nodes = root;
    // if root node is null, then we cannot perform search
    if (!nodes)
    {
        return false;
    }
    int i = nodes->NodeLookup(value);
    if (nodes->values[i] == value)
    {
        // value already exist, nothing is inserted
        cout << "Value already exist, nothing to be inserted. Please choose another value." << endl;
        return false;
    }
    // check the current node if the search value exist
    if (nodes->size == nodesize)
    {
        FullNodes.push_back(nodes);
    }
    else
    {
        // if the node is not full, we clear the FullNodes vector
        FullNodes.clear();
        // we make sure we add back the current node
        FullNodes.push_back(nodes);
    }
    if (nodes->leaf)
    {
        // it reaches the end of the tree to find where the value should be inserted
        return true;
    }
    return Lookup(nodes->children[i], value, FullNodes);
}

/**
 * @brief get access to nodes
 *
 * @return Node pointer
 */
Node *BTree::getRootNode()
{
    return nodes;
}

/**
 * @brief reset root nodes to its root
 *
 * @param root root pointer
 */
void BTree::setRootNode(Node *root)
{
    nodes = root;
}

/**
 * @brief Helper function for inserting a new value
 *
 * @param node root node that needs to be split
 * @param value value to be inserted
 * @return newRoot updated root node
 */
Node *BTree::SplitRoot(Node *node, int value)
{
    // create a new node 'new_root' which will become the new root after splitting the current root
    Node *newRoot = new Node(nodesize, false);
    // update node Id
    newRoot->NodeId = NodeIdCounter++;
    // set the current root to be child of new root 'new_root'
    newRoot->children[0] = node;
    // split the child node of 'new_root' at index 0, which is the current root node
    newRoot->SplitChild(0, node, NodeIdCounter);
    // inserts the new key 'value' into the appropriate child node of 'new_root'
    int i = (newRoot->values[0] < value) ? 1 : 0;
    newRoot->children[i]->NodeInsert(value, NodeIdCounter);
    // update root of the B-tree to be new node 'new_root'
    return newRoot;
}

/**
 * @brief BTree insertion with a new value
 * @param value insertion value
 *
 */
void BTree::Insert(int value)
{ // if root node is null, need to create a new root for the empty tree
    if (!nodes)
    {
        // create a new root with nodesize and marks it as leaf since it is the only node initially
        nodes = new Node(nodesize, true);
        // insert the value into the root node at index 0
        nodes->values[0] = value;
        // update the number of keys in the root node to 1
        nodes->size = 1;
        // update nodeId
        nodes->NodeId = NodeIdCounter++;
    }
    else
    {
        // assign root
        Node *root = nodes;
        // if there's more than 2 layers we have to traverse each level to see if the nodes are full
        if (layers > 2)
        {
            vector<Node *> FullNodes;
            // this is a recursive overload look up function to store all the nodes that are full to a vector
            // this traverses all the way to leaf node to find the closest node to insert
            if (!Lookup(nodes, value, FullNodes))
            {
                // if the value already exist, we do not insert
                // we reset the root back to the original root
                setRootNode(root);
                return;
            }
            // if it's leaf node and it's not full, we just insert
            if (nodes->leaf && nodes->size < nodesize)
            {
                nodes->NodeInsert(value, NodeIdCounter);
                // resets the node
                setRootNode(root);
                return;
            }
            // if the full node is not empty, meaning we have to traverse thru layers that require a split
            if (FullNodes.size() == layers && root->size == nodesize)
            {
                Node *newRoot = SplitRoot(root, value);
                root = newRoot;
                setRootNode(root);
            }
            else if (FullNodes.size() > 1)
            {
                FullNodes.front()->NodeInsert(value, NodeIdCounter);
                setRootNode(root);
            }
            else
            {
                setRootNode(root);
                nodes->NodeInsert(value, NodeIdCounter);
            }
        }
        else
        {
            // only 2 layers, we can insert directly
            int i = nodes->NodeLookup(value);
            if (nodes->values[i] == value)
            {
                // value already exist, nothing is inserted
                return;
            }
            // if the root is full, we split the root
            if ((nodes->children[i] && nodes->children[i]->size == nodesize && nodes->size == nodesize) || (nodes->leaf && nodes->size == nodesize))
            {
                Node *newRoot = SplitRoot(nodes, value);
                nodes = newRoot;
            }
            nodes->NodeInsert(value, NodeIdCounter);
        }
    }
}

/**
 * @brief Display of the entire constructed tree using level order traversal
 *
 */
void BTree::Display()
{
    // if root is null, we ignore
    if (!nodes)
    {
        cout << "No tree found" << endl;
        return;
    }
    queue<Node *> q;
    q.push(nodes);
    int level = 0;
    while (!q.empty())
    {
        int NodeCount = q.size();
        // display levels
        cout << "L-" << level << ": ";
        layers = level + 1;
        while (NodeCount > 0)
        {
            Node *node = q.front();
            q.pop();
            // display the current node
            cout << node->getNodeId() << "[";
            node->Display(node->size, node->getNodeId());
            cout << "] ";

            // enqueue the children
            for (int i = 0; i < node->size + 1; i++)
            {
                if (node->children[i])
                {
                    q.push(node->children[i]);
                }
            }
            NodeCount--;
        }
        level++;
        cout << endl;
    }
    cout << endl;
}

/**
 * @brief Destroy the BTree::BTree object
 *
 */
BTree::~BTree()
{
    if (nodes != nullptr)
    {
        delete nodes;
        nodes = nullptr;
    }
}
