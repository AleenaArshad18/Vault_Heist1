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
    static const int RESULT_SCREEN = 7;

    //ACT 1
    bool binaryLevelSuccess = false;
    bool binaryHeightShown = false;
    bool binaryDiameterShown = false;
    bool binaryLcaShown = false;
    bool binaryPathShown = false;
    bool binaryMirrorDone = false;
    bool binarySymmetryChecked = false;

    //ACT 2
    // ACT 2 - BST LEVEL 1 SEARCH
    bool bstLevelSuccess = false;
    bool bstSearchStarted = false;

    int bstSearchTarget = -1;
    int bstComparisons = 0;

    // Level 3 - deletion progress
    bool bstLeafDeleted = false;
    bool bstOneChildDeleted = false;
    bool bstTwoChildrenDeleted = false;

    // ACT 2 - LEVEL 3: DELETE INTERACTION
    VisualNode* findClickedNode(VisualNode* node, Vector2i mousePosition);
    VisualNode* bstDeleteSelectedNode = nullptr;

    int bstDeleteCaseSelected = 0;
    // 0 = no case selected
    // 1 = leaf
    // 2 = one child
    // 3 = two children

    bool bstDeleteAwaitingConfirmation = false;

    int bstDeleteSuccessor = -1;

    // Level 5 - sorted insertion progress
    int bstLevel5NextValue = 0;
    int bstLevel5Values[5] = { 10, 20, 30, 40, 50 };

    string rangeLowInput = "";
    string rangeHighInput = "";

    bool rangeLowInputActive = false;
    bool rangeHighInputActive = false;

    // ACT 2 - LEVEL 4: RANGE RAID

    int rangeLow = 0;
    int rangeHigh = 0;

    bool rangeQueryStarted = false;
    bool rangeQueryFinished = false;

    Node* rangeCurrentNode = nullptr;

    // Fixed-size stack for BST traversal
    Node* rangeStack[100];
    int rangeStackTop = -1;

    // Range query results
    int rangeResults[100];
    int rangeResultCount = 0;

    // Statistics
    int rangeNodesVisited = 0;
    int rangeSubtreesPruned = 0;
    int rangeCorrectDecisions = 0;
    int rangeWrongDecisions = 0;

    // Current decision:
    // 1 = node smaller than lower limit
    // 2 = node greater than upper limit
    // 3 = node inside range
    int rangeDecisionType = 0;

    Node* bstCurrentNode = nullptr;

    int bstSearchPath[100];
    int bstSearchPathLength = 0;

    // Act 2 - BST level drawing
    void drawBSTLevel1();
    void drawBSTLevel2();
    void drawBSTLevel3();
    void drawBSTLevel4();
    void drawBSTLevel5();

    // Act 2 - BST level click handlers
    void handleBSTLevel1Click(Vector2i mousePosition);
    void handleBSTLevel2Click(Vector2i mousePosition);
    void handleBSTLevel3Click(Vector2i mousePosition);
    void handleBSTLevel4Click(Vector2i mousePosition);
    void handleBSTLevel5Click(Vector2i mousePosition);


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

    void drawBinaryGameplay();
    void drawAVLGameplay();

    void drawButton(const std::string& text, Vector2f position, Vector2f size, bool enabled = true);
    void drawCenteredText(const std::string& text, float y, unsigned int fontSize);
    void drawText(const std::string& text, Vector2f position, unsigned int fontSize);
    void drawPanel(Vector2f position, Vector2f size);

    void drawHeapGameplay();
    void drawHeapArray();
    void drawHeapOperations();

    void drawResultScreen();
    void completeCurrentLevel();
    void handleResultClick(Vector2i mousePosition);

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

    void startBSTSearch();
    void moveBSTLeft();
    void moveBSTRight();
    void performRangeQuery();

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

    // Campaign progression
    bool actUnlocked[4] = { true, false, false, false };

    bool levelUnlocked[4][5] =
    {
        { true, false, false, false, false },
        { false, false, false, false, false },
        { false, false, false, false, false },
        { false, false, false, false, false }
    };

    bool levelCompleted[4][5] = {};


    void startRangeRaid();
    void prepareNextRangeNode();
    void handleRangeDecision(int decision);
    void finishRangeRaid();
    void resetRangeRaid();
};
