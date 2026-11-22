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
#include "AnimationController.h"

using namespace sf;
using namespace std;
class Game
{
public:
    Game();
    void run();
    ~Game();
private:
    static const int MAIN_MENU = 0;
    static const int CAMPAIGN_MAP = 1;
    static const int LEVEL_SELECT = 2;
    static const int LEVEL_INTRO = 3;
    static const int GAMEPLAY = 4;
    static const int SANDBOX = 5;
    static const int LEADERBOARD = 6;
    static const int RESULT_SCREEN = 7;

    // ACT 1 - Binary Tree challenge state
    bool binaryLevelSuccess = false;
    bool binaryHeightShown = false;
    bool binaryDiameterShown = false;
    bool binaryLcaShown = false;
    bool binaryPathShown = false;
    bool binaryMirrorDone = false;

    // ACT 3 - AVL level completion and challenge progress
    bool avlLevelSuccess = false;
    bool avlBalanceFactorChecked = false;
    bool avlBalanceVerified = false;
    bool avlSingleRotationDone = false;
    bool avlLRDone = false;
    bool avlRLDone = false;
    bool avlInsertDone = false;
    int avlInsertedValue = -1;
    bool avlDeleteDone = false;
    bool avlKthDone = false;
    bool avlRankDone = false;
    bool avlRangeDone = false;
    bool binarySymmetryChecked = false;
    bool binaryMirrored = false;
    bool binaryUsePostorder = false;
    int binaryInputField = 0;
    string binaryAnswerInput = "";
    string binaryHeightInput = "";
    string binaryDiameterInput = "";
    string binaryLcaInput = "";
    string binaryPathInput = "";
    string binaryMirrorInput = "";

    // SANDBOX - independent instances so campaign data is not changed
    BinaryTree sandboxBinaryTree;
    BST sandboxBST;
    AVLTree sandboxAVLTree;
    MaxHeap sandboxHeap;
    int sandboxStructure = 1; // 1 Binary Tree, 2 BST, 3 AVL, 4 Max Heap
    string sandboxInput = "";
    string sandboxOutput = "Choose a data structure to begin.";
    bool sandboxInputActive = false;
    void resetSandbox();
    void drawSandbox();
    void handleSandboxClick(Vector2i mousePosition);
    void runSandboxOperation(int operation);

    // LEADERBOARD - fixed-size array ranked by score
    static const int LEADERBOARD_CAPACITY = 10;
    string leaderboardNames[LEADERBOARD_CAPACITY];
    int leaderboardScores[LEADERBOARD_CAPACITY] = {};
    int leaderboardCount = 0;
    int playerScore = 0;
    string leaderboardName = "";
    bool leaderboardNameActive = false;
    string leaderboardMessage = "Complete campaign levels to earn points.";
    void drawLeaderboard();
    void handleLeaderboardClick(Vector2i mousePosition);
    void submitLeaderboardScore();
    void loadLeaderboard();
    bool saveLeaderboard();

    // ACT 2 - BST
    bool bstLevelSuccess = false;
    bool bstSearchStarted = false;
    int bstSearchTarget = -1;
    int bstComparisons = 0;
    bool bstLeafDeleted = false;
    bool bstOneChildDeleted = false;
    bool bstTwoChildrenDeleted = false;
    VisualNode* findClickedNode(VisualNode* node, Vector2i mousePosition);
    VisualNode* bstDeleteSelectedNode = nullptr;
    int bstDeleteCaseSelected = 0;
    bool bstDeleteAwaitingConfirmation = false;
    int bstDeleteSuccessor = -1;
    int bstLevel5NextValue = 0;
    int bstLevel5Values[5] = { 10, 20, 30, 40, 50 };
    bool bstLevel5BuildComplete = false;
    bool bstLevel5SearchComplete = false;
    string rangeLowInput = "";
    string rangeHighInput = "";
    bool rangeLowInputActive = false;
    bool rangeHighInputActive = false;
    int rangeLow = 0;
    int rangeHigh = 0;
    bool rangeQueryStarted = false;
    bool rangeQueryFinished = false;
    Node* rangeCurrentNode = nullptr;
    Node* rangeStack[100];
    int rangeStackTop = -1;
    int rangeResults[100];
    int rangeResultCount = 0;
    int rangeNodesVisited = 0;
    int rangeSubtreesPruned = 0;
    int rangeCorrectDecisions = 0;
    int rangeWrongDecisions = 0;
    int rangeDecisionType = 0;
    Node* bstCurrentNode = nullptr;
    int bstSearchPath[100];
    int bstSearchPathLength = 0;
    Node* bstInsertCurrentNode = nullptr;
    Node* bstInsertParentNode = nullptr;
    int bstInsertValue = -1;
    int bstInsertComparisons = 0;
    bool bstInsertStarted = false;
    bool bstInsertReadyToInsert = false;
    bool bstInsertLeft = false;

    void startBSTInsertion();
    void moveBSTInsertLeft();
    void moveBSTInsertRight();
    void confirmBSTInsertion();
    void resetBSTInsertion();
    void drawBSTLevel1();
    void drawBSTLevel2();
    void drawBSTLevel3();
    void drawBSTLevel4();
    void drawBSTLevel5();
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
    VisualNode* convertToVisualTree(Node* node);
    void rebuildVisualTree();
    void deleteVisualTree(VisualNode* node);
    VisualNode* convertHeapToVisualTree(int index);
    void rebuildHeapVisualTree();
    void deleteHeapVisualTree(VisualNode* node);
    VisualNode* convertBinaryToVisualTree(Node* node);
    void rebuildBinaryVisualTree();
    VisualNode* convertAVLToVisualTree(AVLNode* node);
    void rebuildAVLVisualTree();
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
    void performAVLInsert();
    void performAVLDelete();
    void clearHighlights(VisualNode* node);
    void highlightNode(VisualNode* node, int value);
    RenderWindow window;
    AnimationController animationController;
    Clock animationClock;
    LayoutEngine layoutEngine;
    Renderer renderer;
    Font uiFont;
    bool uiFontLoaded = false;
    int currentScreen = MAIN_MENU;
    int selectedLevel = 0;
    int selectedAct = 2;
    BST bst;
    VisualNode* visualRoot = nullptr;
    BinaryTree binaryTree;
    VisualNode* binaryVisualRoot = nullptr;
    string binaryTraversalResult = "";
    AVLTree avlTree;
    VisualNode* avlVisualRoot = nullptr;
    string searchInput = "";
    string searchResult = "";
    bool searchInputActive = false;
    MaxHeap maxHeap;
    VisualNode* heapVisualRoot = nullptr;
    int selectedHeapIndex = -1;

    int heapFirstSelected = -1;

    bool heapAwaitingRemove = false;
    bool heapNeedsHeapify = false;
    bool heapSortBuilding = false;

    bool previousHeapAwaitingRemove = false;
    bool previousHeapNeedsHeapify = false;
    bool previousHeapSortBuilding = false;

    bool heapLevelSuccess = false;
    bool heapLevelFailed = false;
    bool previousHeapLevelFailed = false;

    int heapExtractedValues[20] = {};
    int heapExtractedCount = 0;
    int heapSteps = 0;
    int heapAlarmLimit = 8;
    int previousHeapExtractedCount = 0;
    int previousHeapSteps = 0;
    bool previousHeapLevelSuccess = false;
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

    static const int MAX_HEAP_ANIMATION_STEPS = 100;
    static const int MAX_HEAP_ELEMENTS = 20;

    string heapEvents[MAX_HEAP_ANIMATION_STEPS];
    int heapEventCount = 0;

    // Saved heap arrays for playback.
    // Snapshot 0 is the initial state.
    // Snapshot 1 is the state after the first event.
    int heapAnimationStates[MAX_HEAP_ANIMATION_STEPS + 1][MAX_HEAP_ELEMENTS] = {};
    int heapAnimationSizes[MAX_HEAP_ANIMATION_STEPS + 1] = {};

    int heapAnimationExtracted[MAX_HEAP_ANIMATION_STEPS + 1][MAX_HEAP_ELEMENTS] = {};
    int heapAnimationExtractedCounts[MAX_HEAP_ANIMATION_STEPS + 1] = {};

    int heapAnimationStateCount = 0;

    bool heapPlaybackMode = false;
    int heapDisplayedAnimationIndex = -1;

    void captureHeapAnimationSnapshot(int index);
    VisualNode* convertHeapSnapshotToVisualTree(int values[], int count, int index);

    void addHeapEvent(string event);
    void drawHeapEventLog();

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

    // PART D - SCORING, ALARM AND PROGRESSION
    int alarmSteps = 0;
    int optimalSteps = 1;
    int alarmCapacity = 4;

    int penaltyCount = 0;
    int penaltyPoints = 0;
    int bonusPoints = 0;

    int starCap = 3;
    int currentLevelStars = 0;
    int currentLevelScore = 0;

    int levelStars[4][5] = {};
    int levelScores[4][5] = {};

    bool levelFailedByAlarm = false;
    bool finalVaultUnlocked = false;

    void resetLevelScoring();
    int calculateOptimalSteps();
    void registerAlarmStep(int amount);
    void registerPenalty(int additionalAlarm, int pointDeduction);
    void drawScoringHUD();
};
