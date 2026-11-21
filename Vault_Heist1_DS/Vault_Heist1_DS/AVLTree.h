#pragma once

#include "AVLNode.h"
#include <string>

class AVLTree
{
private:
    AVLNode* root;
    std::string lastRotation;

    int height(AVLNode* node);
    int getSize(AVLNode* node);
    int getBalance(AVLNode* node);
    void updateNode(AVLNode* node);

    AVLNode* rotateLeft(AVLNode* node);
    AVLNode* rotateRight(AVLNode* node);
    AVLNode* rebalance(AVLNode* node);

    AVLNode* insertNode(AVLNode* node, int value);
    AVLNode* insertPlain(AVLNode* node, int value);
    AVLNode* deleteNode(AVLNode* node, int value);
    AVLNode* findMin(AVLNode* node);

    AVLNode* rotateAt(AVLNode* node, int value, bool rotateLeftDirection, bool& rotated);

    int kthSmallest(AVLNode* node, int k);
    int rankValue(AVLNode* node, int value);
    int countRange(AVLNode* node, int low, int high);

    void clearTree(AVLNode* node);
    bool validateNode(AVLNode* node, int& actualHeight, int& actualSize);

public:
    AVLTree();
    ~AVLTree();

    void insert(int value);
    void insertDemo(int value);
    void remove(int value);
    void reset();

    int getHeight();
    int balanceFactor(int value);
    int kth(int k);
    int rank(int value);
    int countInRange(int low, int high);

    bool rotateLeftAt(int value);
    bool rotateRightAt(int value);
    bool isBalanced();

    AVLNode* getRoot();
    std::string getLastRotation() const;
    void clearLastRotation();
};
