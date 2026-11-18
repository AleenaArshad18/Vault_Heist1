
#pragma once

#include "BST.h"

class BinaryTree
{
private:
    Node* root;

    void preorderRecursive(Node* node, int output[], int& index);
    void inorderRecursive(Node* node, int output[], int& index);
    void postorderRecursive(Node* node, int output[], int& index);

    void levelRecursive(Node* node, int level, int output[], int& index);
    int getHeight(Node* node);
    int getDiameter(Node* node);

    Node* buildPreIn(int pre[], int in[], int start, int end, int& index);
    Node* buildPostIn(int post[], int in[], int start, int end, int& index);

    bool findPath(Node* node, int target, int path[], int& length);
    Node* findLCA(Node* node, int a, int b);

    bool sameTree(Node* a, Node* b);
    bool symmetric(Node* a, Node* b);
    void mirrorTree(Node* node);
    void clearTree(Node* node);

public:
    BinaryTree();
    ~BinaryTree();

    Node* getRoot();

    void preorderRecursive(int output[], int& count);
    void inorderRecursive(int output[], int& count);
    void postorderRecursive(int output[], int& count);

    void preorderIterative(int output[], int& count, int capacity);
    void inorderIterative(int output[], int& count, int capacity);
    void postorderIterative(int output[], int& count, int capacity);

    void levelOrderRecursive(int output[], int& count);
    void levelOrderIterative(int output[], int& count, int capacity);

    void buildFromPreorderInorder(int pre[], int in[], int n);
    void buildFromPostorderInorder(int post[], int in[], int n);

    int height();
    int diameter();

    int LCA(int a, int b);
    bool pathTo(int target, int path[], int& length);

    void mirror();
    bool isSameTree(BinaryTree& other);
    bool isSymmetric();
};
