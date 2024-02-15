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

using namespace std;

/**
 * @brief Node definition Construct a new Node:: Node object
 *
 */
Node::Node(int _degree, bool _leaf) : degree(_degree), leaf(_leaf), NodeId(-1)
{
    // allocate enough space for a new Node
    values = new int[2 * degree - 1];
    // initialize node pointers for the Node's children ptr
    children = new Node *[2 * degree];
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
    if (leaf)
    {
        if (values[i] == value)
        {
            // value already exist, nothing is inserted
            return;
        }
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
        // non leaf
        // see index to be inserted is full or not
        if (children[i]->size == 2 * degree - 1)
        {
            SplitChild(i, children[i], NodeIdCounter);
            if (values[i] < value)
            {
                i++;
            }
            children[i]->NodeInsert(value, NodeIdCounter);
        }
        else
        {
            children[i]->NodeInsert(value, NodeIdCounter);
        }
    }
}

/**
 * @brief splitting child when current node is full
 *
 * @param CurrNode
 */
void Node::SplitChild(int i, Node *CurrNode, int &NodeIdCounter)
{
    // Create a new node that will be the right sibling of CurrNode
    Node *newNode = new Node(degree, CurrNode->leaf);
    newNode->NodeId = NodeIdCounter++;
    // Copy the right half of CurrNode's values and children to newNode
    newNode->size = degree - 1;
    for (int j = 0; j < degree - 1; j++)
    {
        newNode->values[j] = CurrNode->values[j + degree];
    }

    if (!CurrNode->leaf)
    {
        for (int j = 0; j < degree; j++)
        {
            newNode->children[j] = CurrNode->children[j + degree];
        }
    }

    // Reduce the size of CurrNode
    CurrNode->size = degree - 1;

    // Insert the middle value of CurrNode into the parent node at index i
    for (int j = size; j > i; j--)
    {
        children[j + 1] = children[j];
    }
    children[i + 1] = newNode;

    for (int j = size - 1; j >= i; j--)
    {
        values[j + 1] = values[j];
    }
    values[i] = CurrNode->values[degree - 1];

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
    if (i < 2 * degree + 1)
    {
        // format ","s per requirement
        int CurrSize = i;
        while (CurrSize < 2 * degree - 1)
        {
            cout << ",";
            CurrSize++;
        }
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
 * degree initialize as input
 *
 */
BTree::BTree(int _degree) : degree(_degree), nodes(nullptr), NodeIdCounter(0) {}

/**
 * @brief Search the search value
 *
 * @param value search value
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
 * @brief get access to nodes
 *
 * @return Node*
 */
Node *BTree::getRootNode()
{
    return nodes;
}

/**
 * @brief reset root nodes to its root
 *
 * @param root
 */
void BTree::setRootNode(Node *root)
{
    nodes = root;
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
        // create a new root with minimum degree and marks it as leaf since it is the only node initially
        nodes = new Node(degree, true);
        // insert the value into the root node at index 0
        nodes->values[0] = value;
        // update the number of keys in the root node to 1
        nodes->size = 1;
        // update nodeId
        nodes->NodeId = NodeIdCounter++;
    }
    else
    {
        // if node is full after insertion, need to split
        if (nodes->size == 2 * degree - 1)
        {
            // create a new node 'new_root' which will become the new root after splitting the current root
            Node *new_root = new Node(degree, false);
            // update node Id
            new_root->NodeId = NodeIdCounter++;
            // set the current root to be child of new root 'new_root'
            new_root->children[0] = nodes;
            // split the child node of 'new_root' at index 0, which is the current root node
            new_root->SplitChild(0, nodes, NodeIdCounter);
            // inserts the new key 'value' into the appropriate child node of 'new_root'
            int i = 0;
            if (new_root->values[0] < value)
                i++;
            new_root->children[i]->NodeInsert(value, NodeIdCounter);
            // update root of the B-tree to be new node 'new_root'
            nodes = new_root;
        }
        else
            // insert new key 'value' into root node if it is not full
            nodes->NodeInsert(value, NodeIdCounter);
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
