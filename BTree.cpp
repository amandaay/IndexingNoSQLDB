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

    public:
        // Node Constructor: initiates a new node 
        // @return the node pointer
        Node(){}
        /*
         * nodeLookup(int value, int pointer)
         *    - True if the value was found in the specified node.
        */
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

    // allow access of Node class for BTree
    friend class BTree;
};


class BTree {
    private:
        // size of node
        // static const int NODESIZE = 5;
        // Node array, initialized with length = 1, i.e. root node
        Node* nodes;  
        //  Number of currently used nodes
        int CntNodes;
        // pointer to the root node
        Node* root;
        // number of currently used values
        int CntValues;

    public:
        BTree() {
            root = NULL;
        }
        // Lookup(int value)
        // - True if the value was found.
        bool Lookup(int value);
        // Insert(int value)
        // - If -1 is returned, the value is inserted and increase cntValues.
        // - If -2 is returned, the value already exists.
        void Insert(int value);
        // CntValues()- Returns the number of used values.
        int CntValues();
        
};


/**
 * @brief Construct a new Node:: Node object
 * 
 */
Node::Node() {
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
    // binary search
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

