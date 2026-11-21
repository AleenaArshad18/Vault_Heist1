#include "AVLTree.h"

#include <algorithm>

AVLTree::AVLTree() : root(nullptr), lastRotation("") {}

AVLTree::~AVLTree()
{
    clearTree(root);
}

int AVLTree::height(AVLNode* node)
{
    return node ? node->height : 0;
}

int AVLTree::getSize(AVLNode* node)
{
    return node ? node->size : 0;
}

int AVLTree::getBalance(AVLNode* node)
{
    return node ? height(node->left) - height(node->right) : 0;
}

void AVLTree::updateNode(AVLNode* node)
{
    if (!node) return;
    node->height = 1 + std::max(height(node->left), height(node->right));
    node->size = 1 + getSize(node->left) + getSize(node->right);
}

AVLNode* AVLTree::rotateRight(AVLNode* node)
{
    if (!node || !node->left) return node;
    AVLNode* newRoot = node->left;
    AVLNode* transfer = newRoot->right;
    newRoot->right = node;
    node->left = transfer;
    updateNode(node);
    updateNode(newRoot);
    return newRoot;
}

AVLNode* AVLTree::rotateLeft(AVLNode* node)
{
    if (!node || !node->right) return node;
    AVLNode* newRoot = node->right;
    AVLNode* transfer = newRoot->left;
    newRoot->left = node;
    node->right = transfer;
    updateNode(node);
    updateNode(newRoot);
    return newRoot;
}

AVLNode* AVLTree::rebalance(AVLNode* node)
{
    if (!node) return nullptr;
    updateNode(node);
    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0)
    {
        lastRotation = "LL rotation (Right Rotation)";
        return rotateRight(node);
    }
    if (balance > 1 && getBalance(node->left) < 0)
    {
        lastRotation = "LR rotation (Left then Right)";
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)
    {
        lastRotation = "RR rotation (Left Rotation)";
        return rotateLeft(node);
    }
    if (balance < -1 && getBalance(node->right) > 0)
    {
        lastRotation = "RL rotation (Right then Left)";
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

AVLNode* AVLTree::insertNode(AVLNode* node, int value)
{
    if (!node) return new AVLNode(value);
    if (value < node->data) node->left = insertNode(node->left, value);
    else if (value > node->data) node->right = insertNode(node->right, value);
    else return node;
    return rebalance(node);
}

void AVLTree::insert(int value)
{
    lastRotation.clear();
    root = insertNode(root, value);
}

AVLNode* AVLTree::insertPlain(AVLNode* node, int value)
{
    if (!node) return new AVLNode(value);
    if (value < node->data) node->left = insertPlain(node->left, value);
    else if (value > node->data) node->right = insertPlain(node->right, value);
    updateNode(node);
    return node;
}

void AVLTree::insertDemo(int value)
{
    root = insertPlain(root, value);
}

AVLNode* AVLTree::findMin(AVLNode* node)
{
    while (node && node->left) node = node->left;
    return node;
}

AVLNode* AVLTree::deleteNode(AVLNode* node, int value)
{
    if (!node) return nullptr;
    if (value < node->data) node->left = deleteNode(node->left, value);
    else if (value > node->data) node->right = deleteNode(node->right, value);
    else
    {
        if (!node->left || !node->right)
        {
            AVLNode* child = node->left ? node->left : node->right;
            delete node;
            return child;
        }
        AVLNode* successor = findMin(node->right);
        node->data = successor->data;
        node->right = deleteNode(node->right, successor->data);
    }
    return rebalance(node);
}

void AVLTree::remove(int value)
{
    lastRotation.clear();
    root = deleteNode(root, value);
}

void AVLTree::clearTree(AVLNode* node)
{
    if (!node) return;
    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

void AVLTree::reset()
{
    clearTree(root);
    root = nullptr;
    lastRotation.clear();
}

int AVLTree::getHeight() { return height(root); }

int AVLTree::balanceFactor(int value)
{
    AVLNode* current = root;
    while (current)
    {
        if (value == current->data) return getBalance(current);
        current = value < current->data ? current->left : current->right;
    }
    return 999;
}

int AVLTree::kthSmallest(AVLNode* node, int k)
{
    if (!node || k <= 0 || k > getSize(node)) return -1;
    int leftCount = getSize(node->left);
    if (k == leftCount + 1) return node->data;
    if (k <= leftCount) return kthSmallest(node->left, k);
    return kthSmallest(node->right, k - leftCount - 1);
}

int AVLTree::kth(int k) { return kthSmallest(root, k); }

int AVLTree::rankValue(AVLNode* node, int value)
{
    if (!node) return 0;
    if (value < node->data) return rankValue(node->left, value);
    return getSize(node->left) + 1 + rankValue(node->right, value);
}

int AVLTree::rank(int value) { return rankValue(root, value); }

int AVLTree::countRange(AVLNode* node, int low, int high)
{
    if (!node) return 0;
    if (node->data < low) return countRange(node->right, low, high);
    if (node->data > high) return countRange(node->left, low, high);
    return 1 + countRange(node->left, low, high) + countRange(node->right, low, high);
}

int AVLTree::countInRange(int low, int high)
{
    return low > high ? 0 : countRange(root, low, high);
}

AVLNode* AVLTree::rotateAt(AVLNode* node, int value, bool leftDirection, bool& rotated)
{
    if (!node) return nullptr;
    if (value < node->data) node->left = rotateAt(node->left, value, leftDirection, rotated);
    else if (value > node->data) node->right = rotateAt(node->right, value, leftDirection, rotated);
    else
    {
        if (leftDirection && node->right)
        {
            node = rotateLeft(node);
            rotated = true;
        }
        else if (!leftDirection && node->left)
        {
            node = rotateRight(node);
            rotated = true;
        }
    }
    updateNode(node);
    return node;
}

bool AVLTree::rotateLeftAt(int value)
{
    bool rotated = false;
    root = rotateAt(root, value, true, rotated);
    if (rotated) lastRotation = "Manual Left Rotation at " + std::to_string(value);
    return rotated;
}

bool AVLTree::rotateRightAt(int value)
{
    bool rotated = false;
    root = rotateAt(root, value, false, rotated);
    if (rotated) lastRotation = "Manual Right Rotation at " + std::to_string(value);
    return rotated;
}

bool AVLTree::validateNode(AVLNode* node, int& actualHeight, int& actualSize)
{
    if (!node)
    {
        actualHeight = 0;
        actualSize = 0;
        return true;
    }
    int lh = 0, rh = 0, ls = 0, rs = 0;
    if (!validateNode(node->left, lh, ls) || !validateNode(node->right, rh, rs)) return false;
    actualHeight = 1 + std::max(lh, rh);
    actualSize = 1 + ls + rs;
    int balance = lh - rh;
    return node->height == actualHeight && node->size == actualSize && balance >= -1 && balance <= 1;
}

bool AVLTree::isBalanced()
{
    int h = 0, s = 0;
    return validateNode(root, h, s);
}

AVLNode* AVLTree::getRoot() { return root; }
std::string AVLTree::getLastRotation() const { return lastRotation; }
void AVLTree::clearLastRotation() { lastRotation.clear(); }
