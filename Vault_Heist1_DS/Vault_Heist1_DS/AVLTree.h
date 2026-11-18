

#pragma once

#include "AVLNode.h"

class AVLTree
{
private:
    AVLNode* root;

    // Basic helper functions
    int height(AVLNode* node);
    int getSize(AVLNode* node);
    int getBalance(AVLNode* node);

    // Update height and size
    void updateNode(AVLNode* node);

    // Rotations
    AVLNode* rotateLeft(AVLNode* node);
    AVLNode* rotateRight(AVLNode* node);

    // Balance the tree
    AVLNode* rebalance(AVLNode* node);

    // Recursive insertion and deletion
    AVLNode* insertNode(AVLNode* node, int value);
    AVLNode* deleteNode(AVLNode* node, int value);

    // Find minimum value
    AVLNode* findMin(AVLNode* node);

    // Order statistic helpers
    int kthSmallest(AVLNode* node, int k);
    int rankValue(AVLNode* node, int value);

    // Delete all nodes
    void clearTree(AVLNode* node);
    bool checkBalanced(AVLNode* node);
    int countRange(AVLNode* node, int low, int high);

public:
    AVLTree();

    ~AVLTree();

    void insert(int value);
    void remove(int value);

    int getHeight();
    int balanceFactor(int value);

    int kth(int k);
    int rank(int value);

    AVLNode* getRoot();
    bool isBalanced();
    int countInRange(int low, int high);
};
