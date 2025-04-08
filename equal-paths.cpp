#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool helper(Node* node, int currentDepth, int& leafDepth);


// Recursive helper function to traverse the tree and check paths
bool helper(Node* node, int currentDepth, int& leafDepth) {
    // Base case: if node is null, return true (not a leaf node, valid path)
    if (node == nullptr) {
        return true;
    }

    // If we are at a leaf node (both left and right are null)
    if (node->left == nullptr && node->right == nullptr) {
        // If this is the first leaf, set the leafDepth
        if (leafDepth == -1) {
            leafDepth = currentDepth;  // Set leaf depth to current path depth
        }
        // Check if current leaf depth matches the previously found leaf depth
        return currentDepth == leafDepth;
    }

    // Recursively check both subtrees, increasing depth by 1
    bool leftResult = helper(node->left, currentDepth + 1, leafDepth);
    bool rightResult = helper(node->right, currentDepth + 1, leafDepth);

    // Return true if both subtrees have the same leaf depth, otherwise false
    return leftResult && rightResult;
}


bool equalPaths(Node * root) {
    // Initialize the leaf depth as -1 (invalid depth)
    int leafDepth = -1;
    // Start the recursion with the root node and an initial depth of 0
    return helper(root, 0, leafDepth);
}

