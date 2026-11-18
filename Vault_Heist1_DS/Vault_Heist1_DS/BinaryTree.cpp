
#include "BinaryTree.h"
#include <iostream>
using namespace std;

BinaryTree::BinaryTree()
{
    root = nullptr;
}

Node* BinaryTree::getRoot()
{
    return root;
}

// Delete all nodes from memory
void BinaryTree::clearTree(Node* node)
{
    if (node == nullptr)
        return;

    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

BinaryTree::~BinaryTree()
{
    clearTree(root);
}

// RECURSIVE TRAVERSALS

void BinaryTree::preorderRecursive(Node* node, int output[], int& index)
{
    if (node == nullptr)
        return;

    output[index++] = node->data;
    preorderRecursive(node->left, output, index);
    preorderRecursive(node->right, output, index);
}

void BinaryTree::preorderRecursive(int output[], int& count)
{
    count = 0;
    preorderRecursive(root, output, count);
}

void BinaryTree::inorderRecursive(Node* node, int output[], int& index)
{
    if (node == nullptr)
        return;

    inorderRecursive(node->left, output, index);
    output[index++] = node->data;
    inorderRecursive(node->right, output, index);
}

void BinaryTree::inorderRecursive(int output[], int& count)
{
    count = 0;
    inorderRecursive(root, output, count);
}

void BinaryTree::postorderRecursive(Node* node, int output[], int& index)
{
    if (node == nullptr)
        return;

    postorderRecursive(node->left, output, index);
    postorderRecursive(node->right, output, index);
    output[index++] = node->data;
}

void BinaryTree::postorderRecursive(int output[], int& count)
{
    count = 0;
    postorderRecursive(root, output, count);
}

// ITERATIVE PREORDER
void BinaryTree::preorderIterative(int output[], int& count, int capacity)
{
    count = 0;
    if (root == nullptr || capacity <= 0)
        return;

    Node* stack[1000];
    int top = -1;

    stack[++top] = root;

    while (top >= 0 && count < capacity)
    {
        Node* current = stack[top--];
        output[count++] = current->data;

        if (current->right != nullptr && top < 999)
            stack[++top] = current->right;

        if (current->left != nullptr && top < 999)
            stack[++top] = current->left;
    }
}

// ITERATIVE INORDER
void BinaryTree::inorderIterative(int output[], int& count, int capacity)
{
    count = 0;
    Node* stack[1000];
    int top = -1;
    Node* current = root;

    while ((current != nullptr || top >= 0) && count < capacity)
    {
        while (current != nullptr && top < 999)
        {
            stack[++top] = current;
            current = current->left;
        }

        if (top >= 0)
        {
            current = stack[top--];
            output[count++] = current->data;
            current = current->right;
        }
    }
}

// ITERATIVE POSTORDER (TWO STACKS)
void BinaryTree::postorderIterative(int output[], int& count, int capacity)
{
    count = 0;
    if (root == nullptr || capacity <= 0)
        return;

    Node* stack1[1000];
    Node* stack2[1000];

    int top1 = -1;
    int top2 = -1;

    stack1[++top1] = root;

    while (top1 >= 0 && top2 < 999)
    {
        Node* current = stack1[top1--];
        stack2[++top2] = current;

        if (current->left != nullptr && top1 < 999)
            stack1[++top1] = current->left;

        if (current->right != nullptr && top1 < 999)
            stack1[++top1] = current->right;
    }

    while (top2 >= 0 && count < capacity)
        output[count++] = stack2[top2--]->data;
}

// RECURSIVE LEVEL ORDER
void BinaryTree::levelRecursive(
    Node* node, int level, int output[], int& index)
{
    if (node == nullptr)
        return;

    if (level == 1)
    {
        output[index++] = node->data;
        return;
    }

    levelRecursive(node->left, level - 1, output, index);
    levelRecursive(node->right, level - 1, output, index);
}

void BinaryTree::levelOrderRecursive(int output[], int& count)
{
    count = 0;
    int h = getHeight(root);

    for (int i = 1; i <= h; i++)
        levelRecursive(root, i, output, count);
}

// ITERATIVE LEVEL ORDER (QUEUE)
void BinaryTree::levelOrderIterative(
    int output[], int& count, int capacity)
{
    count = 0;
    if (root == nullptr || capacity <= 0)
        return;

    Node* queue[1000];
    int front = 0;
    int rear = 0;

    queue[rear++] = root;

    while (front < rear && count < capacity)
    {
        Node* current = queue[front++];
        output[count++] = current->data;

        if (current->left != nullptr && rear < 1000)
            queue[rear++] = current->left;

        if (current->right != nullptr && rear < 1000)
            queue[rear++] = current->right;
    }
}

// HEIGHT OF TREE
int BinaryTree::getHeight(Node* node)
{
    if (node == nullptr)
        return 0;

    int leftHeight = getHeight(node->left);
    int rightHeight = getHeight(node->right);

    if (leftHeight > rightHeight)
        return leftHeight + 1;

    return rightHeight + 1;
}

int BinaryTree::height()
{
    return getHeight(root);
}

// DIAMETER OF TREE
int BinaryTree::getDiameter(Node* node)
{
    if (node == nullptr)
        return 0;

    int leftHeight = getHeight(node->left);
    int rightHeight = getHeight(node->right);

    int leftDiameter = getDiameter(node->left);
    int rightDiameter = getDiameter(node->right);

    int throughRoot = leftHeight + rightHeight + 1;

    if (leftDiameter > throughRoot)
        throughRoot = leftDiameter;

    if (rightDiameter > throughRoot)
        throughRoot = rightDiameter;

    return throughRoot;
}

int BinaryTree::diameter()
{
    return getDiameter(root);
}

// BUILD TREE FROM PREORDER AND INORDER
Node* BinaryTree::buildPreIn(
    int pre[], int in[], int start, int end, int& index)
{
    if (start > end)
        return nullptr;

    Node* node = new Node(pre[index++]);

    if (start == end)
        return node;

    int position = start;

    while (position <= end && in[position] != node->data)
        position++;

    node->left = buildPreIn(pre, in, start, position - 1, index);
    node->right = buildPreIn(pre, in, position + 1, end, index);

    return node;
}

void BinaryTree::buildFromPreorderInorder(int pre[], int in[], int n)
{
    clearTree(root);
    root = nullptr;

    if (n <= 0)
        return;

    int index = 0;
    root = buildPreIn(pre, in, 0, n - 1, index);
}

// BUILD TREE FROM POSTORDER AND INORDER
Node* BinaryTree::buildPostIn(
    int post[], int in[], int start, int end, int& index)
{
    if (start > end)
        return nullptr;

    Node* node = new Node(post[index--]);

    if (start == end)
        return node;

    int position = start;

    while (position <= end && in[position] != node->data)
        position++;

    node->right = buildPostIn(post, in, position + 1, end, index);
    node->left = buildPostIn(post, in, start, position - 1, index);

    return node;
}

void BinaryTree::buildFromPostorderInorder(int post[], int in[], int n)
{
    clearTree(root);
    root = nullptr;

    if (n <= 0)
        return;

    int index = n - 1;
    root = buildPostIn(post, in, 0, n - 1, index);
}

// FIND PATH TO A NODE
bool BinaryTree::findPath(
    Node* node, int target, int path[], int& length)
{
    if (node == nullptr)
        return false;

    path[length++] = node->data;

    if (node->data == target)
        return true;

    if (findPath(node->left, target, path, length))
        return true;

    if (findPath(node->right, target, path, length))
        return true;

    length--;
    return false;
}

bool BinaryTree::pathTo(int target, int path[], int& length)
{
    length = 0;
    return findPath(root, target, path, length);
}

// LOWEST COMMON ANCESTOR
Node* BinaryTree::findLCA(Node* node, int a, int b)
{
    if (node == nullptr)
        return nullptr;

    if (node->data == a || node->data == b)
        return node;

    Node* left = findLCA(node->left, a, b);
    Node* right = findLCA(node->right, a, b);

    if (left != nullptr && right != nullptr)
        return node;

    if (left != nullptr)
        return left;

    return right;
}

int BinaryTree::LCA(int a, int b)
{
    int path[1000];
    int length = 0;

    if (!pathTo(a, path, length))
        return -1;

    if (!pathTo(b, path, length))
        return -1;

    Node* answer = findLCA(root, a, b);

    if (answer == nullptr)
        return -1;

    return answer->data;
}

// MIRROR TREE
void BinaryTree::mirrorTree(Node* node)
{
    if (node == nullptr)
        return;

    Node* temp = node->left;
    node->left = node->right;
    node->right = temp;

    mirrorTree(node->left);
    mirrorTree(node->right);
}

void BinaryTree::mirror()
{
    mirrorTree(root);
}

// CHECK WHETHER TWO TREES ARE IDENTICAL
bool BinaryTree::sameTree(Node* a, Node* b)
{
    if (a == nullptr && b == nullptr)
        return true;

    if (a == nullptr || b == nullptr)
        return false;

    if (a->data != b->data)
        return false;

    return sameTree(a->left, b->left) &&
        sameTree(a->right, b->right);
}

bool BinaryTree::isSameTree(BinaryTree& other)
{
    return sameTree(root, other.root);
}

// CHECK WHETHER TREE IS SYMMETRIC
bool BinaryTree::symmetric(Node* a, Node* b)
{
    if (a == nullptr && b == nullptr)
        return true;

    if (a == nullptr || b == nullptr)
        return false;

    if (a->data != b->data)
        return false;

    return symmetric(a->left, b->right) &&
        symmetric(a->right, b->left);
}

bool BinaryTree::isSymmetric()
{
    if (root == nullptr)
        return true;

    return symmetric(root->left, root->right);
}
