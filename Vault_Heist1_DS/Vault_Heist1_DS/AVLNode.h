#pragma once

struct AVLNode
{
    int data;
    AVLNode* left;
    AVLNode* right;
    int height;
    int size;

    AVLNode(int value)
        : data(value), left(nullptr), right(nullptr), height(1), size(1)
    {
    }
};
