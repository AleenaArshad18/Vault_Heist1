#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "VisualNode.h"
#include "LayoutEngine.h"
#include "Renderer.h"
#include "BST.h"

using namespace sf;

class Game
{
public:
    Game();
    void run();

private:
    static const int MAIN_MENU = 0;
    static const int CAMPAIGN_MAP = 1;
    static const int LEVEL_SELECT = 2;
    static const int LEVEL_INTRO = 3;
    static const int GAMEPLAY = 4;
    static const int SANDBOX = 5;
    static const int LEADERBOARD = 6;

    void processEvents();
    void update();
    void render();

    void handleMouseClick(Vector2i mousePosition);
    bool isMouseOver(Vector2i mousePosition, Vector2f position, Vector2f size);

    void drawMainMenu();
    void drawCampaignMap();
    void drawLevelSelect();
    void drawLevelIntro();
    void drawGameplay();
    void drawPlaceholder(const std::string& title, const std::string& message);

    void drawButton(const std::string& text, Vector2f position, Vector2f size, bool enabled = true);
    void drawCenteredText(const std::string& text, float y, unsigned int fontSize);
    void drawText(const std::string& text, Vector2f position, unsigned int fontSize);
    void drawPanel(Vector2f position, Vector2f size);

    void positionGameplayTree();
    void shiftTree(VisualNode* node, float offsetX);

    // BST and visual tree connection
    VisualNode* convertToVisualTree(Node* node);
    void rebuildVisualTree();
    void deleteVisualTree(VisualNode* node);

    // Search functionality
    void performSearch();
    void clearHighlights(VisualNode* node);
    void highlightNode(VisualNode* node, int value);

    // Window and rendering objects
    RenderWindow window;

    LayoutEngine layoutEngine;
    Renderer renderer;

    Font uiFont;
    bool uiFontLoaded = false;

    // Screen navigation
    int currentScreen = MAIN_MENU;
    int selectedLevel = 0;

    // Actual BST
    BST bst;

    // Visual copy of BST
    VisualNode* visualRoot = nullptr;

    // Search input and result
    std::string searchInput = "";
    std::string searchResult = "";
    bool searchInputActive = false;
};