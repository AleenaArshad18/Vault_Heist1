


#include "AVLTree.h"

// Constructor
AVLTree::AVLTree()
{
    root = nullptr;
}

// Destructor
AVLTree::~AVLTree()
{
    clearTree(root);
}

// Get height of a node
int AVLTree::height(AVLNode* node)
{
    if (node == nullptr)
        return 0;

    return node->height;
}

// Get size of a subtree
int AVLTree::getSize(AVLNode* node)
{
    if (node == nullptr)
        return 0;

    return node->size;
}

// Calculate balance factor
int AVLTree::getBalance(AVLNode* node)
{
    if (node == nullptr)
        return 0;

    return height(node->left) - height(node->right);
}

// Update height and size
void AVLTree::updateNode(AVLNode* node)
{
    if (node == nullptr)
        return;

    int leftHeight = height(node->left);
    int rightHeight = height(node->right);

    if (leftHeight > rightHeight)
        node->height = leftHeight + 1;
    else
        node->height = rightHeight + 1;

    node->size = 1 + getSize(node->left) + getSize(node->right);
}

// Right Rotation (LL case)
AVLNode* AVLTree::rotateRight(AVLNode* node)
{
    AVLNode* newRoot = node->left;
    AVLNode* temp = newRoot->right;

    newRoot->right = node;
    node->left = temp;

    updateNode(node);
    updateNode(newRoot);

    return newRoot;
}

// Left Rotation (RR case)
AVLNode* AVLTree::rotateLeft(AVLNode* node)
{
    AVLNode* newRoot = node->right;
    AVLNode* temp = newRoot->left;

    newRoot->left = node;
    node->right = temp;

    updateNode(node);
    updateNode(newRoot);

    return newRoot;
}

// Rebalance the tree
AVLNode* AVLTree::rebalance(AVLNode* node)
{
    if (node == nullptr)
        return nullptr;

    updateNode(node);

    int balance = getBalance(node);

    // LL Case
    if (balance > 1 && getBalance(node->left) >= 0)
        return rotateRight(node);

    // LR Case
    if (balance > 1 && getBalance(node->left) < 0)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // RR Case
    if (balance < -1 && getBalance(node->right) <= 0)
        return rotateLeft(node);

    // RL Case
    if (balance < -1 && getBalance(node->right) > 0)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Insert a value
AVLNode* AVLTree::insertNode(AVLNode* node, int value)
{
    if (node == nullptr)
        return new AVLNode(value);

    if (value < node->data)
        node->left = insertNode(node->left, value);

    else if (value > node->data)
        node->right = insertNode(node->right, value);

    else
        return node;  // Duplicate values not allowed

    return rebalance(node);
}

// Public insertion function
void AVLTree::insert(int value)
{
    root = insertNode(root, value);
}

// Find minimum node
AVLNode* AVLTree::findMin(AVLNode* node)
{
    while (node != nullptr && node->left != nullptr)
        node = node->left;

    return node;
}

// Delete a value
AVLNode* AVLTree::deleteNode(AVLNode* node, int value)
{
    if (node == nullptr)
        return nullptr;

    if (value < node->data)
        node->left = deleteNode(node->left, value);

    else if (value > node->data)
        node->right = deleteNode(node->right, value);

    else
    {
        // Case 1 and 2: No child or one child
        if (node->left == nullptr || node->right == nullptr)
        {
            AVLNode* child;

            if (node->left != nullptr)
                child = node->left;
            else
                child = node->right;

            delete node;
            return child;
        }

        // Case 3: Two children
        AVLNode* successor = findMin(node->right);

        node->data = successor->data;

        node->right = deleteNode(node->right, successor->data);
    }

    return rebalance(node);
}

// Public deletion function
void AVLTree::remove(int value)
{
    root = deleteNode(root, value);
}

// Get overall tree height
int AVLTree::getHeight()
{
    return height(root);
}

// Get balance factor of a particular value
int AVLTree::balanceFactor(int value)
{
    AVLNode* temp = root;

    while (temp != nullptr)
    {
        if (value == temp->data)
            return getBalance(temp);

        else if (value < temp->data)
            temp = temp->left;

        else
            temp = temp->right;
    }

    return 999; // Value not found
}

// Find kth smallest value
int AVLTree::kthSmallest(AVLNode* node, int k)
{
    if (node == nullptr || k <= 0 || k > getSize(node))
        return -1;

    int leftSize = getSize(node->left);

    if (k == leftSize + 1)
        return node->data;

    else if (k <= leftSize)
        return kthSmallest(node->left, k);

    else
        return kthSmallest(node->right, k - leftSize - 1);
}

// Public kth function
int AVLTree::kth(int k)
{
    return kthSmallest(root, k);
}

// Count values smaller than or equal to given value
int AVLTree::rankValue(AVLNode* node, int value)
{
    if (node == nullptr)
        return 0;

    if (value < node->data)
        return rankValue(node->left, value);

    return getSize(node->left) + 1 +
        rankValue(node->right, value);
}

// Public rank function
int AVLTree::rank(int value)
{
    return rankValue(root, value);
}

// Return root
AVLNode* AVLTree::getRoot()
{
    return root;
}

// Delete all nodes from memory
void AVLTree::clearTree(AVLNode* node)
{
    if (node == nullptr)
        return;

    clearTree(node->left);
    clearTree(node->right);

    delete node;
}

// Check whether the entire AVL tree is balanced
bool AVLTree::checkBalanced(AVLNode* node)
{
    if (node == nullptr)
        return true;

    // Check left and right subtrees first
    if (!checkBalanced(node->left))
        return false;

    if (!checkBalanced(node->right))
        return false;

    int leftHeight = height(node->left);
    int rightHeight = height(node->right);

    int expectedHeight = 1;

    if (leftHeight > rightHeight)
        expectedHeight += leftHeight;
    else
        expectedHeight += rightHeight;

    int expectedSize = 1 +
        getSize(node->left) +
        getSize(node->right);

    // Check stored height and size
    if (node->height != expectedHeight)
        return false;

    if (node->size != expectedSize)
        return false;

    // Check balance factor
    int balance = leftHeight - rightHeight;

    if (balance < -1 || balance > 1)
        return false;

    return true;
}

// Public balance checking function
bool AVLTree::isBalanced()
{
    return checkBalanced(root);
}

// Count values inside a given range
int AVLTree::countRange(AVLNode* node, int low, int high)
{
    if (node == nullptr)
        return 0;

    // Value is smaller than the range
    if (node->data < low)
        return countRange(node->right, low, high);

    // Value is greater than the range
    if (node->data > high)
        return countRange(node->left, low, high);

    // Value is inside the range
    return 1 +
        countRange(node->left, low, high) +
        countRange(node->right, low, high);
}

// Public range counting function
int AVLTree::countInRange(int low, int high)
{
    if (low > high)
        return 0;

    return countRange(root, low, high);
}

