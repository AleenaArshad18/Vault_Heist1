
#include "Renderer.hpp"
#include <string>
#include <iostream>
#include <cmath>

using namespace std;
using namespace sf;

Renderer::Renderer()
{
    fontLoaded = font.openFromFile("assets/fonts/BrownieStencil.ttf");

    if (!fontLoaded)
    {
        cout<< "Error: Could not load game_font.ttf" << endl;
    }
}

void Renderer::drawTree(RenderWindow& window, const VisualNode* root)
{
    if (root == nullptr)
    {
        return;
    }

    drawEdges(window, root);

    drawNodes(window, root);
}

void Renderer::drawEdges(RenderWindow& window, const VisualNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->left != nullptr)
    {
        drawEdge(window, node->position, node->left->position);
    }

    if (node->right != nullptr)
    {
        drawEdge(window, node->position, node->right->position);
    }

    drawEdges(window, node->left);
    drawEdges(window, node->right);
}

void Renderer::drawNodes(RenderWindow& window, const VisualNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    CircleShape circle(25.f);

    circle.setOrigin({ 25.f, 25.f });
    circle.setPosition(node->position);
    circle.setFillColor(Color(40, 120, 190));
    circle.setOutlineThickness(3.f);
    circle.setOutlineColor(Color::White);

    window.draw(circle);

    if (fontLoaded)
    {
        Text label(font, to_string(node->value), 18);

        label.setFillColor(Color::White);

        FloatRect bounds = label.getLocalBounds();

        label.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        label.setPosition(node->position);

        window.draw(label);
    }

    drawNodes(window, node->left);
    drawNodes(window, node->right);
}

void Renderer::drawEdge(RenderWindow& window, Vector2f start, Vector2f end)
{
    Vector2f difference = end - start;

    float length = sqrt(
        difference.x * difference.x +
        difference.y * difference.y
    );

    float angle = atan2(difference.y, difference.x)
        * 180.f / 3.14159265f;

    RectangleShape line({ length, 3.f });

    line.setOrigin({ 0.f, 1.5f });
    line.setPosition(start);
    line.setRotation(degrees(angle));
    line.setFillColor(Color(180, 180, 190));

    window.draw(line);
}
