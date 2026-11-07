#include "LayoutEngine.hpp"

void LayoutEngine::calculatePositions(
    VisualNode* root,
    float windowWidth,
    float topY,
    float levelGap
) {
    if (root == nullptr)
        return;

    float margin = 60.f;

    assignPositions(
        root,
        margin,
        windowWidth - margin,
        topY,
        levelGap,
        0
    );
}

void LayoutEngine::assignPositions(
    VisualNode* node,
    float leftX,
    float rightX,
    float topY,
    float levelGap,
    int depth
) {
    if (node == nullptr)
        return;

    // Find the centre of the available horizontal space
    float middleX = (leftX + rightX) / 2.f;

    // Assign the node's position
    node->position = {
        middleX,
        topY + depth * levelGap
    };

    // Place left subtree in the left half
    assignPositions(
        node->left,
        leftX,
        middleX,
        topY,
        levelGap,
        depth + 1
    );

    // Place right subtree in the right half
    assignPositions(
        node->right,
        middleX,
        rightX,
        topY,
        levelGap,
        depth + 1
    );
}