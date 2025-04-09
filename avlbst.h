#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
    using BinarySearchTree<Key, Value>::nodeSwap;

protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    void updateBalance(AVLNode<Key, Value>* node);
    void rebalance(AVLNode<Key, Value>* node);

    // Rotation functions
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);

    // Height utility
    int getHeight(AVLNode<Key, Value>* node) const;

    // Remove helpers
    AVLNode<Key, Value>* removeHelper(AVLNode<Key, Value>* node, const Key& key);
    AVLNode<Key, Value>* removeNode(AVLNode<Key, Value>* nodeToRemove);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
     // First do regular BST insert
     AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
     if (this->root_ == nullptr) {
         this->root_ = newNode;
         return;
     }
 
     AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
     AVLNode<Key, Value>* parent = nullptr;
 
     // Find insertion point
     while (current != nullptr) {
         parent = current;
         if (new_item.first < current->getKey()) {
             current = current->getLeft();
         } else if (new_item.first > current->getKey()) {
             current = current->getRight();
         } else {
             // Key already exists, update value
             current->setValue(new_item.second);
             delete newNode;
             return;
         }
     }
 
     // Insert the new node
     newNode->setParent(parent);
     if (new_item.first < parent->getKey()) {
         parent->setLeft(newNode);
     } else {
         parent->setRight(newNode);
     }
 
     // Update balances and rebalance the tree
     updateBalance(newNode);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (nodeToRemove == nullptr) {
        throw KeyError(); //throws error if null node
    }

    // Start removal from the node and go up to rebalance
    AVLNode<Key, Value>* startRebalanceAt = removeNode(nodeToRemove);
    
    // Rebalance up the tree
    while (startRebalanceAt != nullptr) {
        rebalance(startRebalanceAt);
        startRebalanceAt = startRebalanceAt->getParent();
    }
}

// Helper function implementations:

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node) {
    while (node) {
        int leftHeight = getHeight(node->getLeft()); //will take this val for comparison
        int rightHeight = getHeight(node->getRight()); //will take this val for comparison
        node->setBalance(rightHeight - leftHeight);

        if (node->getBalance() < -1 || node->getBalance() > 1) {
            rebalance(node); //rebal when not balanced
            break;
        }

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node) {
    if (node->getBalance() < -1) {
        if (node->getLeft()->getBalance() <= 0) {
            rotateRight(node);
        } else {
            rotateLeft(node->getLeft());
            rotateRight(node);
        }
    } else if (node->getBalance() > 1) {
        if (node->getRight()->getBalance() >= 0) { //many if/else statements to determine what should be rebalanced
            rotateLeft(node);
        } else {
            rotateRight(node->getRight());
            rotateLeft(node);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* newRoot = node->getRight();
    if (!newRoot) return;

    // Update parent's child pointer
    if (node->getParent()) {
        if (node == node->getParent()->getLeft()) {
            node->getParent()->setLeft(newRoot);
        } else {
            node->getParent()->setRight(newRoot);
        }
    } else {
        this->root_ = newRoot;
    }
    newRoot->setParent(node->getParent());

    // Move newRoot's left to node's right
    node->setRight(newRoot->getLeft());
    if (newRoot->getLeft()) {
        newRoot->getLeft()->setParent(node);
    }

    // Make node newRoot's left
    newRoot->setLeft(node);
    node->setParent(newRoot);

    // Update balances
    node->setBalance(node->getBalance() - 1 - std::max<int8_t>(0, newRoot->getBalance()));
    newRoot->setBalance(newRoot->getBalance() - 1 + std::min<int8_t>(0, node->getBalance()));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* newRoot = node->getLeft();
    if (newRoot == nullptr) return;
    
    // Set node's left child to newRoot's right child
    node->setLeft(newRoot->getRight());
    if (newRoot->getRight() != nullptr) {
        newRoot->getRight()->setParent(node);
    }
    
    // Update newRoot's parent
    newRoot->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = newRoot;
    } else {
        if (node == node->getParent()->getRight()) {
            node->getParent()->setRight(newRoot);
        } else {
            node->getParent()->setLeft(newRoot);
        }
    }
    
    // Set node as newRoot's right child
    newRoot->setRight(node);
    node->setParent(newRoot);
    
    // Update balances
    int8_t nodeBalance = node->getBalance();
    int8_t newRootBalance = newRoot->getBalance();
    
    // Calculate min(0, newRootBalance) without ternary
    int8_t minVal;
    if (newRootBalance < 0) {
        minVal = newRootBalance;
    } else {
        minVal = 0;
    }
    node->setBalance(nodeBalance + 1 - minVal);
    
    // Calculate max(0, nodeBalance) without ternary
    int8_t maxVal;
    if (nodeBalance > 0) {
        maxVal = nodeBalance;
    } else {
        maxVal = 0;
    }
    newRoot->setBalance(newRootBalance + 1 + maxVal);
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* node) const {
    if (node == nullptr) return -1;
    return 1 + std::max(getHeight(node->getLeft()), getHeight(node->getRight()));
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::removeNode(AVLNode<Key, Value>* nodeToRemove) {
    AVLNode<Key, Value>* parent = nodeToRemove->getParent();

    // Case 1: Two children
    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        // Find predecessor
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(nodeToRemove));
        nodeSwap(pred, nodeToRemove); // swap in-place
        return removeNode(nodeToRemove); // recursively delete the node (now in pred’s spot)
    }

        // Case 2 and 3: 0 or 1 child
        AVLNode<Key, Value>* child = nullptr;
    if (nodeToRemove->getLeft() != nullptr) {
        child = nodeToRemove->getLeft();
    } else {
        child = nodeToRemove->getRight();
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    if (parent == nullptr) {
        this->root_ = child;
    } else if (parent->getLeft() == nodeToRemove) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }

    delete nodeToRemove;
    return parent; // this is the node to start rebalancing from
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
