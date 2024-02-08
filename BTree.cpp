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
        // int CntValues();
        // deconstructor
        ~BTree();
};


/**
 * @brief Node definition Construct a new Node:: Node object
 * @param _leaf True if it's the leaf node else false
 * 
 */
Node::Node(bool _leaf) : leaf(_leaf) {
    // allocate enough space for a new Node
    values = new int[NODESIZE];
    // initialize new node pointers for the Node's children
    children = new Node*[static_cast<int>(ceil(NODESIZE / 2))];
    // initialize number of values = 0
    size = 0;
}

/**
 * @brief search value in the specific node 
 * @return the search value if exist, otherwise leftmost index
 * 
 */
int Node::NodeLookup(int value) {
    // if node does not exit, you cannot find the existing value
    if (size <= 0) {
        return -1;
    } 
    // if size > 0, perform binary search within a node
    int left = 0, right = size - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (values[mid] < value) {
            left = mid + 1;
        } else {
            right = mid;
        }
        // if (value == values[mid]) {
        //     return true;
        // } else if (value < values[mid]) {
        //     right = mid - 1;
        // } else {
        //     left = mid + 1;
        // }
    }
    // return false;
    return left;
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
    for (int i = 0; i < size; i++) {
        delete children[i];
    }
    delete[] children;
    delete[] values;
}

/**
 * @brief BTree constructor
 * Construct a new BTree::BTree object
 * 
 */
BTree::BTree() {
    // root node is initialize to NULL
    nodes = nullptr;
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
    int i = nodes->NodeLookup(value);
    // check the current node if the search value exist
    if (nodes->values[i] == value) {
        return true;
    }
    // if it reaches leaf node, it indicates there's no such value
    if (nodes->IsLeaf(nodes)) {
        return false;
    }
    // int i = 0;
    // while (i < nodes->size && value > nodes->values[i]) {
    //     i++;
    // }
    return Lookup(nodes->children[i], value);
}

/**
 * @brief Destroy the BTree::BTree object
 * 
 */
BTree::~BTree() {
    // TODO: fix memory deallocation, iterate thru every node and its children
    delete nodes;
}


int main() {
    // BTree tree;
    return 0;
}