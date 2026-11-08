
#pragma once

#include <SFML/Graphics.hpp>
#include "VisualNode.h"
using namespace sf;

class Renderer
{
public:
    Renderer();
    void drawTree(RenderWindow& window, const VisualNode* root);

private:
    Font font;
    bool fontLoaded = false;
    void drawEdges(RenderWindow& window, const VisualNode* node);

    void drawNodes(RenderWindow& window, const VisualNode* node);

    void drawEdge(RenderWindow& window, Vector2f start, Vector2f end);
};
