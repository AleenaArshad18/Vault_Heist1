#include "BST.h"

BST::BST() {
    root = nullptr;
}

void BST::insert(int value) {
    Node* newNode = new Node(value);

    if (root == nullptr) {
        root = newNode;
    }
    else {
        Node* temp = root;

        while (temp != nullptr) {

            if (value < temp->data) {
                if (temp->left != nullptr) {
                    temp = temp->left;
                }
                else {
                    temp->left = newNode;
                    break;
                }
            }

            else if (value > temp->data) {
                if (temp->right != nullptr) {
                    temp = temp->right;
                }
                else {
                    temp->right = newNode;
                    break;
                }
            }

            else {
                // Duplicate value: do not insert it
                delete newNode;
                return;
            }
        }
    }
}

bool BST::search(int target, int path[], int* pathlength) {
    Node* temp = root;

    *pathlength = 0;

    while (temp != nullptr) {

        path[*pathlength] = temp->data;
        (*pathlength)++;

        if (temp->data == target) {
            return true;
        }
        else if (target < temp->data) {
            temp = temp->left;
        }
        else {
            temp = temp->right;
        }
    }

    return false;
}

Node* BST::getRoot() {
    return root;
}

bool BST::deleteValue(int value) {

    Node* current = root;
    Node* previous = nullptr;

    // Find the node to delete
    while (current != nullptr && current->data != value) {

        previous = current;

        if (value < current->data) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    // Value does not exist
    if (current == nullptr) {
        return false;
    }

    // CASE 1: Node has no children (leaf node)
    if (current->left == nullptr && current->right == nullptr) {

        if (previous == nullptr) {
            root = nullptr;
        }
        else if (current == previous->left) {
            previous->left = nullptr;
        }
        else {
            previous->right = nullptr;
        }

        delete current;
    }

    // CASE 2: Node has only one child
    else if (current->left == nullptr || current->right == nullptr) {

        Node* grandchild;

        if (current->right == nullptr) {
            grandchild = current->left;
        }
        else {
            grandchild = current->right;
        }

        if (previous == nullptr) {
            root = grandchild;
        }
        else if (current == previous->left) {
            previous->left = grandchild;
        }
        else {
            previous->right = grandchild;
        }

        delete current;
    }

    // CASE 3: Node has two children
    else {

        Node* successor = current->right;
        Node* successorPrevious = current;

        // Find the inorder successor
        while (successor->left != nullptr) {

            successorPrevious = successor;
            successor = successor->left;
        }

        // Copy successor's value into current node
        current->data = successor->data;

        // Remove the successor from its original position
        if (successorPrevious->left == successor) {
            successorPrevious->left = successor->right;
        }
        else {
            successorPrevious->right = successor->right;
        }

        delete successor;
    }

    return true;
}

bool BST::findMinimum(int* minimumValue)
{
    if (root == nullptr)
    {
        return false;
    }

    Node* temp = root;

    while (temp->left != nullptr)
    {
        temp = temp->left;
    }

    *minimumValue = temp->data;

    return true;
}

bool BST::findMaximum(int* maximumValue)
{
    if (root == nullptr)
    {
        return false;
    }

    Node* temp = root;

    while (temp->right != nullptr)
    {
        temp = temp->right;
    }

    *maximumValue = temp->data;

    return true;
}

bool BST::findSuccessor(int value, int* successorValue)
{
    Node* current = root;
    Node* successor = nullptr;

    // Find the target node
    while (current != nullptr && current->data != value)
    {
        if (value < current->data)
        {
            successor = current;
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }

    // Target value does not exist
    if (current == nullptr)
    {
        return false;
    }

    // If right subtree exists, find its smallest value
    if (current->right != nullptr)
    {
        successor = current->right;

        while (successor->left != nullptr)
        {
            successor = successor->left;
        }
    }

    // No successor exists
    if (successor == nullptr)
    {
        return false;
    }

    *successorValue = successor->data;

    return true;
}

bool BST::findPredecessor(int value, int* predecessorValue)
{
    Node* current = root;
    Node* predecessor = nullptr;

    // Find the target node
    while (current != nullptr && current->data != value)
    {
        if (value > current->data)
        {
            predecessor = current;
            current = current->right;
        }
        else
        {
            current = current->left;
        }
    }

    // Target value does not exist
    if (current == nullptr)
    {
        return false;
    }

    // If left subtree exists, find its largest value
    if (current->left != nullptr)
    {
        predecessor = current->left;

        while (predecessor->right != nullptr)
        {
            predecessor = predecessor->right;
        }
    }

    // No predecessor exists
    if (predecessor == nullptr)
    {
        return false;
    }

    *predecessorValue = predecessor->data;

    return true;
}

void BST::rangeQuery(Node* node, int low, int high, int output[], int& count)
{
    if (node == nullptr)
        return;

    if (node->data > low)
        rangeQuery(node->left, low, high, output, count);

    if (node->data >= low && node->data <= high)
        output[count++] = node->data;

    if (node->data < high)
        rangeQuery(node->right, low, high, output, count);
}

void BST::rangeQuery(int low, int high, int output[], int& count)
{
    count = 0;

    if (low > high)
        return;

    rangeQuery(root, low, high, output, count);
}

void BST::clear()
{
    // Delete all nodes in the current tree
    Node* nodes[1000];
    int count = 0;

    if (root != nullptr)
        nodes[count++] = root;

    while (count > 0)
    {
        Node* current = nodes[--count];

        if (current->left != nullptr)
            nodes[count++] = current->left;

        if (current->right != nullptr)
            nodes[count++] = current->right;

        delete current;
    }

    root = nullptr;
}