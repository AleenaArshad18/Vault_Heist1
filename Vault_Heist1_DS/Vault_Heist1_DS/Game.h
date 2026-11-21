#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "BinaryTree.h"
#include "AVLTree.h"
#include "VisualNode.h"
#include "LayoutEngine.h"
#include "Renderer.h"
#include "BST.h"
#include "MaxHeap.h"

using namespace sf;
using namespace std;

class Game
{
public:
    Game();
    void run();
    ~Game();
    //
private:
    static const int MAIN_MENU = 0;
    static const int CAMPAIGN_MAP = 1;
    static const int LEVEL_SELECT = 2;
    static const int LEVEL_INTRO = 3;
    static const int GAMEPLAY = 4;
    static const int SANDBOX = 5;
    static const int LEADERBOARD = 6;

    void processEvents();
    //comment
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

    void drawBinaryGameplay();
    void drawAVLGameplay();

    void drawButton(const std::string& text, Vector2f position, Vector2f size, bool enabled = true);
    void drawCenteredText(const std::string& text, float y, unsigned int fontSize);
    void drawText(const std::string& text, Vector2f position, unsigned int fontSize);
    void drawPanel(Vector2f position, Vector2f size);

    void drawHeapGameplay();
    void drawHeapArray();
    void drawHeapOperations();

    void positionGameplayTree();
    void shiftTree(VisualNode* node, float offsetX);

    // BST and visual tree connection
    VisualNode* convertToVisualTree(Node* node);
    void rebuildVisualTree();
    void deleteVisualTree(VisualNode* node);

    VisualNode* convertHeapToVisualTree(int index);
    void rebuildHeapVisualTree();
    void deleteHeapVisualTree(VisualNode* node);

    // Binary Tree visual connection
    VisualNode* convertBinaryToVisualTree(Node* node);
    void rebuildBinaryVisualTree();

    // AVL Tree visual connection
    VisualNode* convertAVLToVisualTree(AVLNode* node);
    void rebuildAVLVisualTree();

    // Search functionality
    void performSearch();
    void performInsert();
    void performDelete();
    void performMinimum();
    void performMaximum();
    void performSuccessor();
    void performPredecessor();

    // AVL operations
    void performAVLInsert();
    void performAVLDelete();

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
    int selectedAct = 2;

    // Actual BST
    BST bst;

    // Visual copy of BST
    VisualNode* visualRoot = nullptr;
    // ACT 1 - Binary Tree
    BinaryTree binaryTree;
    VisualNode* binaryVisualRoot = nullptr;
    string binaryTraversalResult = "";

    // ACT 3 - AVL Tree
    AVLTree avlTree;
    VisualNode* avlVisualRoot = nullptr;

    // Search input and result
    string searchInput = "";
    string searchResult = "";
    bool searchInputActive = false;

    // Actual Max Heap
    MaxHeap maxHeap;

    // Visual copy of Max Heap
    VisualNode* heapVisualRoot = nullptr;

    // Heap array display
    int selectedHeapIndex = -1;

    void highlightHeapNode(VisualNode* node, int targetIndex, int currentIndex = 0);
    void loadHeapLevel();

    string heapInput = "";
    string heapResult = "";
    bool heapInputActive = false;

    int* previousHeapState = nullptr;
    int previousHeapSize = 0;
    bool canUndo = false;

    void saveHeapState();
    void undoHeapOperation();
    void replayHeapLevel();

    string heapEvents[6];
    int heapEventCount = 0;

    void addHeapEvent(string event);
    void drawHeapEventLog();
};
