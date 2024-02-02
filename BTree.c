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
#include <stdlib.h>
#include <iostream>

/* Size of Node. */
#define NODESIZE 5

typedef struct Btree {
    Node* nodes;
    int cntNodes;
    int root;
    int cntValues;
} Btree;

int iniNode(Btree* tree) {
    // Implementation of iniNode should be provided here
    // This function should initialize a node and return its index
}

int createLeaf(Btree* tree) {
    // Implementation of createLeaf should be provided here
    // This function should create a leaf node and return its index
}

void Btree_init(Btree* tree) {
    tree->nodes = (Node*)malloc(sizeof(Node)); // Assuming only one Node is needed initially
    tree->cntNodes = 0;
    tree->root = iniNode(tree);
    tree->nodes[tree->root].children[0] = createLeaf(tree);
    tree->cntValues = 0;
}

/*
 * Node data structure.
 *   - This is the simplest structure for nodes used in B-tree
 *   - This will be used for both internal and leaf nodes.
 */
typedef struct Node {
    int* values;
    int* children;
    int size;
} Node;