#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

struct VisualNode {
    int value;
    VisualNode* left = nullptr;
    VisualNode* right = nullptr;
    Vector2f position;
};
 
