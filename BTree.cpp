#include "BTree.h"

/**
 * @brief Node definition Construct a new Node:: Node object
 * 
 */
Node::Node(int _degree, bool _leaf): degree(_degree), leaf(_leaf) {
    // allocate enough space for a new Node
    values = new int[2*degree + 1];
    // initialize node pointers for the Node's children ptr
    children = new Node*[2*degree];
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
    int left = 0, right = size;
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
 * @brief assuming the leaf node is non full
 * insertion of the dedicated value will be performed
 * 
 */
void Node::NodeInsert(int value) {
    // binary search and insert where the element should be at
    // search the index if exist, otherwise where it should be
    int i = NodeLookup(value);
    if (leaf) {
        // shift all the other values towards the right until it reaches the index to be inserted
        for (int j = size; j > i; j --) {
        values[j] = values[j - 1];
	    }
        values[i] = value;
        size++;
    } else {
        // non leaf	
        // see index to be inserted is full or not
        if (children[i]->size == 2*degree + 1)
        {
            // TODO: uncomment after finishing splitchild
            // SplitChild(i, children[i]);
            if (values[i] < value) {
                i++;		
            }
            children[i]->NodeInsert(value);
	    }
    }
}


/**
 * @brief splitting child when current node is full
 * 
 * @param CurrNode 
 */
// void Node::SplitChild(int i, Node* CurrNode) {
//     //
// }


/**
 * @brief check if there's any children,
 * 
 * @return true if no children
 * @return false there's children
 */
// bool Node::IsLeaf(Node* node) {
//    return node->children == NULL;
// }

/**
 * @brief Destroy the Node:: Node object
 * 
 */
Node::~Node(){
    for (int i = 0; i < size; i++) {
        if (children[i] != nullptr) {
            delete children[i];
        }
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
    // root node is initialize to nullptr
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
    if (i != -1 && nodes->values[i] == value) {
        return true;
    }
    // if it reaches leaf node, it indicates there's no such value
    if (nodes->leaf) {
        return false;
    }
    // int i = 0;
    // while (i < nodes->size && value > nodes->values[i]) {
    //     i++;
    // }
    return Lookup(nodes->children[i], value);
}

/**
 * @brief 
 * 
 */
void BTree::Insert(int value) {

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
