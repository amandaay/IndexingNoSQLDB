/*
 * CS7280 Special Topics in Database Management
 * Project 1: B-tree implementation.
 *
 * You need to code for the following functions in this program
 *   1. Lookup(int value) -> nodeLookup(int value, int node)
 *   2. Insert(int value) -> nodeInsert(int value, int node)
 *   3. Display(int node)
 *
 */

#include <iostream>
#include <cmath>

using namespace std;

/* Size of Node. */
#define NODESIZE 5

/*
 * Node data structure.
 *   - This is the simplest structure for nodes used in B-tree
 *   - This will be used for both internal and leaf nodes.
 */
class Node {
    private:
        // Node Values (Leaf Values / Key Values for the children nodes).
        int* values;
        // Node Array, pointing to the children nodes. This array is not initialized for leaf nodes.
        Node** children;
        // Number of entries (Rule in B Trees: d <= size <= 2 * d)
        int size;
        // indicates if the current node is a leaf node
        bool leaf;

    public:
        // Node Constructor Declaration: initiates a new node, leaf is T if it's a leaf node else F
        Node(bool leaf);
        // nodeLookup(int value) - True if the value was found in the specified node.
        bool NodeLookup(int value);
        /*
        * nodeInsert(int value, int pointer)
        *    - -2 if the value already exists in the specified node
        *    - -1 if the value is inserted into the node or
        *            something else if the parent node has to be restructured
        */
        void NodeInsert(int value);
        // split child helper function if current node is full
        // void SplitChild(Node* CurrNode);
        // check if the node is a leaf node
        bool IsLeaf(Node* node);
        // Display node
        void Display(int node);
        // deconstructor
        ~Node();

    // allow access of Node class for BTree
    friend class BTree;
};


class BTree {
    private:
        // Node array, including the root node
        Node* nodes;  
        //  Number of currently used nodes
        int CntNodes;
        // number of currently used values
        int CntValues;

    public:
        // constructor for BTree
        BTree();
        // Lookup(int value)
        // - True if the value was found.
        bool Lookup(Node* root, int value);
        // Insert(int value)
        // - If -1 is returned, the value is inserted and increase cntValues.
        // - If -2 is returned, the value already exists.
        void Insert(int value);
        // CntValues()- Returns the number of used values.
        int CntValues();
        // deconstructor
        ~BTree();
};


/**
 * @brief Node definition Construct a new Node:: Node object
 * @param _leaf True if it's the leaf node else false
 * 
 */
Node::Node(bool _leaf) {
    // T if it's leaf else F
    leaf = _leaf;
    // allocate enough space for a new Node
    values = new int[NODESIZE];
    // initialize new node pointers for the Node's children
    children = new Node*[ceil(NODESIZE / 2)];
    // initialize number of values = 0
    size = 0;
}

/**
 * @brief search value in the specific node
 * @return T if found else F
 * 
 */
bool Node::NodeLookup(int value) {
    // if node does not exit, you cannot find the existing value
    if (size <= 0) {
        return false;
    } 
    // if size > 0, perform binary search within a node
    int low = 0, high = size - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (value == values[mid]) {
            return true;
        } else if (value < values[mid]) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return false;
}


/**
 * @brief insertion of value in the specific node
 * 
 */
void Node::NodeInsert(int value) {
    //...
    size++;
}


/**
 * @brief splitting child when current node is full
 * 
 * @param CurrNode 
 */
// void Node::SplitChild(Node* CurrNode) {
//     //
// }


/**
 * @brief check if there's any children,
 * 
 * @param node 
 * @return true if no children
 * @return false there's children
 */
bool Node::IsLeaf(Node* node) {
    return node->children == NULL;
}

/**
 * @brief Destroy the Node:: Node object
 * 
 */
Node::~Node(){
    delete[] values;
    delete[] children;
}

/**
 * @brief BTree constructor
 * Construct a new BTree::BTree object
 * 
 */
BTree::BTree() {
    // root node is initialize to NULL
    nodes = NULL;
}

/**
 * @brief Search the search value 
 * 
 * @param root root node of current subtree
 * @param value search value
 * @return true if search value found else return false
 */

bool BTree::Lookup(Node* root, int value) {
    nodes = root;
    // if root node is null, then we cannot perform search
    if (!nodes) {
        return false;
    } 
    // check the current node if the search value exist
    if (nodes->NodeLookup(value)) {
        return true;
    }
    int i = 0;
    while (i < CntNodes && value > nodes->children[i]->values[nodes->size - 1]) {
        i++;
    }
    return Lookup(nodes->children[i], value);
}

/**
 * @brief Destroy the BTree::BTree object
 * 
 */
BTree::~BTree() {
    delete nodes;
}