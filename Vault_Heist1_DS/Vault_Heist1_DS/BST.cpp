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