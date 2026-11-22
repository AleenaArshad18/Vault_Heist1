#include "Game.h"
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <functional>
using namespace sf;
using namespace std;
//comment
namespace
{
    string getBinaryTraversal(Node* root, int order)
    {
        vector<int> values;
        if (order == 3)
        {
            if (root)
            {
                queue<Node*> nodes;
                nodes.push(root);
                while (!nodes.empty())
                {
                    Node* current = nodes.front();
                    nodes.pop();
                    values.push_back(current->data);
                    if (current->left) nodes.push(current->left);
                    if (current->right) nodes.push(current->right);
                }
            }
        }
        else
        {
            function<void(Node*)> visit = [&](Node* node)
                {
                    if (!node) return;
                    if (order == 0) values.push_back(node->data);
                    visit(node->left);
                    if (order == 1) values.push_back(node->data);
                    visit(node->right);
                    if (order == 2) values.push_back(node->data);
                };
            visit(root);
        }
        if (values.empty()) return "Tree is empty.";
        string result;
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (i) result += " -> ";
            result += to_string(values[i]);
        }
        return result;
    }
}
// Constructor
Game::Game() : maxHeap(10)
{
    maxHeap.insert(50);
    maxHeap.insert(30);
    maxHeap.insert(40);
    maxHeap.insert(10);
    maxHeap.insert(20);



    // ACT 1 - Binary Tree
    int binaryPre[] = { 1, 2, 4, 5, 3 };
    int binaryIn[] = { 4, 2, 5, 1, 3 };

    binaryTree.buildFromPreorderInorder(binaryPre, binaryIn, 5);

    // ACT 3 - AVL Tree
    avlTree.insert(50);
    avlTree.insert(30);
    avlTree.insert(70);
    avlTree.insert(20);
    avlTree.insert(40);
    avlTree.insert(60);
    avlTree.insert(80);

    window.create(VideoMode({ 1000u, 700u }), "Vault Heist: The Living Vault");
    window.setFramerateLimit(60);

    rebuildHeapVisualTree();

    uiFontLoaded = uiFont.openFromFile("assets/fonts/BrownieStencil.ttf");

    if (!uiFontLoaded)
    {
        cerr << "Error: Could not load BrownieStencil.ttf" << endl;
    }

    // Insert initial values into the actual BST
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);

    // Create visual copies of all trees
    rebuildVisualTree();
    rebuildBinaryVisualTree();
    rebuildAVLVisualTree();

    positionGameplayTree();;
}

// Main game loop
void Game::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

Game::~Game()
{
    delete[] previousHeapState;
    deleteVisualTree(visualRoot);
    deleteHeapVisualTree(heapVisualRoot);
    deleteVisualTree(binaryVisualRoot);
    deleteVisualTree(avlVisualRoot);
}


// Process window events
void Game::processEvents()
{
    while (auto event = window.pollEvent())
    {
        // Close window
        if (event->is<Event::Closed>())
        {
            window.close();
        }

        // ---------------------------------
        // TEXT INPUT
        // ---------------------------------
        if (const auto* text = event->getIf<Event::TextEntered>())
        {
            // MAX HEAP INPUT - ACT 4
            if (currentScreen == GAMEPLAY &&
                selectedAct == 4 && heapInputActive)
            {
                if (text->unicode >= '0' && text->unicode <= '9')
                {
                    heapInput += static_cast<char>(text->unicode);
                }
                else if (text->unicode == 8 && !heapInput.empty())
                {
                    heapInput.pop_back();
                }
            }

            // RANGE INPUT - ACT 2, LEVEL 4
            else if (currentScreen == GAMEPLAY &&
                selectedAct == 2 &&
                (rangeLowInputActive || rangeHighInputActive))
            {
                if (text->unicode >= '0' && text->unicode <= '9')
                {
                    if (rangeLowInputActive && rangeLowInput.length() < 9)
                    {
                        rangeLowInput += static_cast<char>(text->unicode);
                    }
                    else if (rangeHighInputActive && rangeHighInput.length() < 9)
                    {
                        rangeHighInput += static_cast<char>(text->unicode);
                    }
                }
                else if (text->unicode == 8)
                {
                    if (rangeLowInputActive && !rangeLowInput.empty())
                    {
                        rangeLowInput.pop_back();
                    }
                    else if (rangeHighInputActive && !rangeHighInput.empty())
                    {
                        rangeHighInput.pop_back();
                    }
                }
            }

            // NORMAL SEARCH / VALUE INPUT
            else if (currentScreen == GAMEPLAY && searchInputActive)
            {
                if (text->unicode >= '0' && text->unicode <= '9')
                {
                    if (searchInput.length() < 9)
                    {
                        searchInput += static_cast<char>(text->unicode);
                    }
                }
                else if (text->unicode == 8 && !searchInput.empty())
                {
                    searchInput.pop_back();
                }
            }
        }

        // ---------------------------------
        // WINDOW RESIZE
        // ---------------------------------
        if (const auto* resized = event->getIf<Event::Resized>())
        {
            float width = static_cast<float>(resized->size.x);
            float height = static_cast<float>(resized->size.y);

            window.setView(View(FloatRect({ 0.f, 0.f }, { width, height })));

            if (currentScreen == GAMEPLAY)
            {
                if (selectedAct == 1)
                {
                    rebuildBinaryVisualTree();
                }
                else if (selectedAct == 2)
                {
                    positionGameplayTree();
                }
                else if (selectedAct == 3)
                {
                    rebuildAVLVisualTree();
                }
                else if (selectedAct == 4)
                {
                    rebuildHeapVisualTree();
                }
            }
            else
            {
                layoutEngine.calculatePositions(visualRoot, width);
            }
        }

        // ---------------------------------
        // MOUSE CLICK
        // ---------------------------------
        if (const auto* mouse = event->getIf<Event::MouseButtonPressed>())
        {
            if (mouse->button == Mouse::Button::Left)
            {
                handleMouseClick(mouse->position);
            }
        }

        // ---------------------------------
        // KEYBOARD INPUT
        // ---------------------------------
        if (currentScreen == GAMEPLAY)
        {
            if (const auto* key = event->getIf<Event::KeyPressed>())
            {
                // ENTER KEY
                if (key->code == Keyboard::Key::Enter)
                {
                    // Level 1 - Start BST Search
                    if (selectedAct == 2 &&
                        selectedLevel == 1 &&
                        searchInputActive)
                    {
                        searchInputActive = false;
                        startBSTSearch();
                    }
                }
            }
        }
    }
}



// Update function
void Game::update()
{
    // Future game updates will be handled here.
}


// Render current screen
void Game::render()
{
    window.clear(Color(18, 22, 35));

    if (currentScreen == MAIN_MENU)
    {
        drawMainMenu();
    }
    else if (currentScreen == CAMPAIGN_MAP)
    {
        drawCampaignMap();
    }
    else if (currentScreen == LEVEL_SELECT)
    {
        drawLevelSelect();
    }
    else if (currentScreen == LEVEL_INTRO)
    {
        drawLevelIntro();
    }
    else if (currentScreen == GAMEPLAY)
    {
        if (selectedAct == 1)
        {
            drawBinaryGameplay();
        }
        else if (selectedAct == 2)
        {
            drawGameplay();
        }
        else if (selectedAct == 3)
        {
            drawAVLGameplay();
        }
        else if (selectedAct == 4)
        {
            drawHeapGameplay();
        }
    }
    else if (currentScreen == RESULT_SCREEN)
    {
        drawResultScreen();
    }
    else if (currentScreen == SANDBOX)
    {
        drawPlaceholder("SANDBOX", "Sandbox mode is coming soon.");
    }
    else if (currentScreen == LEADERBOARD)
    {
        drawPlaceholder("LEADERBOARD", "Leaderboard is coming soon.");
    }

    window.display();
}



// Handle mouse clicks
void Game::handleMouseClick(Vector2i mousePosition)
{
    float width = static_cast<float>(window.getSize().x);
    float centerX = width / 2.f;

    if (currentScreen == MAIN_MENU)
    {
        if (isMouseOver(mousePosition, { centerX - 150.f, 260.f }, { 300.f, 55.f }))
        {
            currentScreen = CAMPAIGN_MAP;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 335.f }, { 300.f, 55.f }))
        {
            currentScreen = SANDBOX;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 410.f }, { 300.f, 55.f }))
        {
            currentScreen = LEADERBOARD;
        }
    }

    else if (currentScreen == CAMPAIGN_MAP)
    {
        if (isMouseOver(mousePosition, { centerX - 150.f, 210.f }, { 300.f, 60.f })
            && actUnlocked[0])
        {
            selectedAct = 1;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 290.f }, { 300.f, 60.f })
            && actUnlocked[1])
        {
            selectedAct = 2;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 370.f }, { 300.f, 60.f })
            && actUnlocked[2])
        {
            selectedAct = 3;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 450.f }, { 300.f, 60.f })
            && actUnlocked[3])
        {
            selectedAct = 4;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
            currentScreen = MAIN_MENU;
        }
    }

    else if (currentScreen == LEVEL_SELECT)
    {
        int actIndex = selectedAct - 1;

        if (isMouseOver(mousePosition, { centerX - 180.f, 230.f }, { 110.f, 65.f })
            && levelUnlocked[actIndex][0])
        {
            selectedLevel = 1;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX - 55.f, 230.f }, { 110.f, 65.f })
            && levelUnlocked[actIndex][1])
        {
            selectedLevel = 2;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX + 70.f, 230.f }, { 110.f, 65.f })
            && levelUnlocked[actIndex][2])
        {
            selectedLevel = 3;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX - 180.f, 320.f }, { 110.f, 65.f })
            && levelUnlocked[actIndex][3])
        {
            selectedLevel = 4;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX - 55.f, 320.f }, { 110.f, 65.f })
            && levelUnlocked[actIndex][4])
        {
            selectedLevel = 5;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
            currentScreen = CAMPAIGN_MAP;
        }
    }

    else if (currentScreen == LEVEL_INTRO)
    {
        // Start Level button
        if (isMouseOver(mousePosition, { centerX - 150.f, 450.f }, { 300.f, 60.f }))
        {
            currentScreen = GAMEPLAY;

            if (selectedAct == 2)
            {
                searchInputActive = false;
                searchInput = "";
                searchResult = "";

                rangeLowInput = "";
                rangeHighInput = "";
                rangeLowInputActive = false;
                rangeHighInputActive = false;

                bstLevelSuccess = false;
                bstSearchStarted = false;
                bstSearchTarget = -1;
                bstCurrentNode = nullptr;
                bstComparisons = 0;
                bstSearchPathLength = 0;

                // Start with an empty BST
                bst.clear();

                // Levels 1-4 use the original example tree
                if (selectedLevel != 5)
                {
                    bst.insert(50);
                    bst.insert(30);
                    bst.insert(70);
                    bst.insert(20);
                    bst.insert(40);
                    bst.insert(60);
                    bst.insert(80);
                }

                // Reset Level 3 deletion progress
                bstLeafDeleted = false;
                bstOneChildDeleted = false;
                bstTwoChildrenDeleted = false;

                // Reset Level 5 insertion progress
                bstLevel5NextValue = 0;

                rebuildVisualTree();
                clearHighlights(visualRoot);
                positionGameplayTree();
            }

            else if (selectedAct == 4)
            {
                selectedHeapIndex = -1;
                heapInput = "";
                heapResult = "";
                heapInputActive = false;
                heapEventCount = 0;
                canUndo = false;

                loadHeapLevel();
            }
            else if (selectedAct == 1)
            {
                binaryLevelSuccess = false;
                binaryHeightShown = false;
                binaryDiameterShown = false;
                binaryLcaShown = false;
                binaryPathShown = false;
                binaryMirrorDone = false;
                binarySymmetryChecked = false;

                int pre[] = { 1, 2, 4, 5, 3 };
                int in[] = { 4, 2, 5, 1, 3 };
                binaryTree.buildFromPreorderInorder(pre, in, 5);
                searchResult = "";
                searchInput = "";
                searchInputActive = false;
                rebuildBinaryVisualTree();
            }
            else if (selectedAct == 3)
            {
                searchInputActive = false;
                searchInput = "";
                searchResult = "";
                avlTree.reset();

                if (selectedLevel == 2)
                {
                    avlTree.insert(30); avlTree.insert(20);
                    avlTree.insert(40); avlTree.insert(50);
                }
                else if (selectedLevel == 3)
                {
                    // The double-rotation challenge builds its own LR/RL example on demand.
                }
                else if (selectedLevel == 4)
                {
                    avlTree.insert(30); avlTree.insert(20);
                    avlTree.insert(40);
                }
                else
                {
                    avlTree.insert(50); avlTree.insert(30);
                    avlTree.insert(70); avlTree.insert(20);
                    avlTree.insert(40); avlTree.insert(60);
                    avlTree.insert(80);
                }
                avlTree.clearLastRotation();
                clearHighlights(avlVisualRoot);
                rebuildAVLVisualTree();
            }
        }

        // Back button
        else if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
            currentScreen = LEVEL_SELECT;
        }
    }

    else if (currentScreen == GAMEPLAY)
    {
        if (selectedAct == 4)
        {
            // Build Max Heap
            if (isMouseOver(mousePosition, { 760.f, 230.f }, { 200.f, 40.f }))
            {
                saveHeapState();

                maxHeap.buildMaxHeap();
                rebuildHeapVisualTree();

                heapResult = "Max Heap built successfully!";

                addHeapEvent("HEAP_BUILD");
            }

            // Heapify
            else if (isMouseOver(mousePosition, { 760.f, 280.f }, { 200.f, 40.f }))
            {
                saveHeapState();

                maxHeap.buildMaxHeap();
                rebuildHeapVisualTree();

                heapResult = "Heapify completed!";

                addHeapEvent("HEAPIFY_DONE");
            }

            // Heap input field
            else if (isMouseOver(mousePosition, { 760.f, 320.f }, { 200.f, 35.f }))
            {
                heapInputActive = true;
            }

            // Insert
            else if (isMouseOver(mousePosition, { 760.f, 365.f }, { 200.f, 40.f }))
            {
                heapInputActive = false;

                if (heapInput != "")
                {
                    int value = stoi(heapInput);

                    saveHeapState();

                    maxHeap.insert(value);
                    rebuildHeapVisualTree();

                    heapResult = "Value inserted successfully";
                    heapInput = "";

                    addHeapEvent("HEAP_INSERT");
                }
            }

            // Extract Maximum button
            else if (isMouseOver(mousePosition, { 760.f, 415.f }, { 200.f, 40.f }))
            {
                int maximum;

                saveHeapState();

                if (maxHeap.extractMax(&maximum))
                {
                    heapResult = "Extracted maximum: " + to_string(maximum);

                    rebuildHeapVisualTree();

                    addHeapEvent("EXTRACT_MAX");
                }
                else
                {
                    heapResult = "Heap is empty!";
                }
            }

            // Heap Sort button
            else if (isMouseOver(mousePosition, { 760.f, 465.f }, { 200.f, 40.f }))
            {
                saveHeapState();

                maxHeap.heapSort();
                rebuildHeapVisualTree();

                heapResult = "Heap Sort completed!";

                addHeapEvent("HEAP_SORT_COMPLETE");
            }

            // Undo
            else if (isMouseOver(mousePosition, { 760.f, 515.f }, { 95.f, 40.f }))
            {
                undoHeapOperation();

                addHeapEvent("UNDO");
            }

            // Replay
            else if (isMouseOver(mousePosition, { 865.f, 515.f }, { 95.f, 40.f }))
            {
                replayHeapLevel();

                addHeapEvent("REPLAY");
            }

            // Reset Level
            else if (isMouseOver(mousePosition, { 760.f, 565.f }, { 200.f, 40.f }))
            {
                loadHeapLevel();

                heapInput = "";
                heapResult = "";
                heapInputActive = false;
                selectedHeapIndex = -1;
                heapEventCount = 0;
                canUndo = false;
            }

            // Select an array element
            else
            {
                int totalElements = maxHeap.getSize();

                float cellWidth = 70.f;
                float gap = 10.f;

                float totalWidth = totalElements * cellWidth +
                    (totalElements - 1) * gap;

                float startX = (window.getSize().x - totalWidth) / 2.f;

                selectedHeapIndex = -1;

                for (int i = 0; i < totalElements; i++)
                {
                    float x = startX + i * (cellWidth + gap);

                    if (isMouseOver(mousePosition, { x, 505.f }, { cellWidth, 55.f }))
                    {
                        selectedHeapIndex = i;
                        break;
                    }
                }

                if (selectedHeapIndex != -1)
                {
                    clearHighlights(heapVisualRoot);
                    highlightHeapNode(heapVisualRoot, selectedHeapIndex);
                }
                else
                {
                    clearHighlights(heapVisualRoot);
                }
            }
        }


else if (selectedAct == 2)
{
    // COMPLETE LEVEL
    if (isMouseOver(mousePosition, { 800.f, 620.f }, { 170.f, 50.f }))
    {
        if (bstLevelSuccess)
        {
            bstLevelSuccess = false;
            completeCurrentLevel();
        }
    }

    // BACK BUTTON
    else if (isMouseOver(mousePosition, { 30.f, 620.f }, { 150.f, 50.f }))
    {
        searchInputActive = false;
        rangeLowInputActive = false;
        rangeHighInputActive = false;

        currentScreen = LEVEL_INTRO;
    }

    // RESET BUTTON
    // RESET BUTTON
    else if (isMouseOver(mousePosition, { 600.f, 620.f }, { 150.f, 50.f }))
    {
        // LEVEL 2 - STASH LOOT
        if (selectedLevel == 2)
        {
            resetBSTInsertion();
        }

        // LEVEL 4 - RANGE RAID
        else if (selectedLevel == 4)
        {
            resetRangeRaid();
        }

        // OTHER BST LEVELS
        else
        {
            searchInput = "";
            searchResult = "";
            searchInputActive = false;

            rangeLowInput = "";
            rangeHighInput = "";
            rangeLowInputActive = false;
            rangeHighInputActive = false;

            bstLevelSuccess = false;
            bstSearchStarted = false;
            bstSearchTarget = -1;
            bstCurrentNode = nullptr;
            bstComparisons = 0;
            bstSearchPathLength = 0;

            clearHighlights(visualRoot);
        }
    }
    //comment

    // LEVEL 1 - SEARCH HEIST
    else if (selectedLevel == 1)
    {
        handleBSTLevel1Click(mousePosition);
    }

    // LEVEL 2 - STASH LOOT
    else if (selectedLevel == 2)
    {
        handleBSTLevel2Click(mousePosition);
    }

    // LEVEL 3 - STEAL AND COLLAPSE
    else if (selectedLevel == 3)
    {
        handleBSTLevel3Click(mousePosition);
    }

    // LEVEL 4 - RANGE RAID
    else if (selectedLevel == 4)
    {
        handleBSTLevel4Click(mousePosition);
    }

    // LEVEL 5 - THE LAZY ARCHITECT
    else if (selectedLevel == 5)
    {
        handleBSTLevel5Click(mousePosition);
    }
}

        else if (selectedAct == 1)
        {
            if (isMouseOver(mousePosition,
                { 800.f, 620.f }, { 170.f, 50.f }))
            {
                if (binaryLevelSuccess)
                {
                    binaryLevelSuccess = false;
                    completeCurrentLevel();
                }
            }

            // ACT 1 - FIVE BINARY TREE CHALLENGES
            else if (selectedLevel == 1)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    binaryLevelSuccess = true;
                    searchResult = "Correct! Preorder: 1 - 2 - 4 - 5 - 3";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    binaryLevelSuccess = false;
                    searchResult = "Incorrect. Preorder visits Root, Left, Right.";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 290.f }, { 190.f, 38.f }))
                {
                    binaryLevelSuccess = false;
                    searchResult = "Incorrect. Try Root-Left-Right.";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 340.f }, { 190.f, 38.f }))
                {
                    binaryLevelSuccess = false;
                    searchResult = "Incorrect. Try Root-Left-Right.";
                }
            }

            

            else if (selectedLevel == 2)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    int pre[] = { 1, 2, 4, 5, 3 };
                    int in[] = { 4, 2, 5, 1, 3 };

                    binaryTree.buildFromPreorderInorder(pre, in, 5);
                    rebuildBinaryVisualTree();

                    binaryLevelSuccess = true;
                    searchResult = "Reconstructed from Preorder + Inorder.";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    int post[] = { 4, 5, 2, 3, 1 };
                    int in[] = { 4, 2, 5, 1, 3 };

                    binaryTree.buildFromPostorderInorder(post, in, 5);
                    rebuildBinaryVisualTree();

                    binaryLevelSuccess = true;
                    searchResult = "Reconstructed from Postorder + Inorder.";
                }
            }


            else if (selectedLevel == 3)
            {
                // SHOW HEIGHT
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    searchResult = "Height = " + to_string(binaryTree.height()) + " levels.";

                    binaryHeightShown = true;
                }

                // SHOW DIAMETER
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    searchResult = "Diameter = " + to_string(binaryTree.diameter()) + " nodes.";

                    binaryDiameterShown = true;
                }

                // CHECK WHETHER BOTH TASKS ARE COMPLETED
                binaryLevelSuccess = binaryHeightShown && binaryDiameterShown;
            }



            else if (selectedLevel == 4)
            {
                // FIND LOWEST COMMON ANCESTOR
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    int ancestor = binaryTree.LCA(4, 5);

                    if (ancestor != -1)
                    {
                        searchResult = "LCA(4, 5) = " + to_string(ancestor);
                        binaryLcaShown = true;
                    }
                    else
                    {
                        searchResult = "One or both nodes not found.";
                    }
                }

                // FIND PATH TO NODE 4
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    int path[100];
                    int length = 0;

                    if (binaryTree.pathTo(4, path, length))
                    {
                        searchResult = "Path to 4: ";

                        for (int i = 0; i < length; ++i)
                        {
                            if (i > 0)
                                searchResult += " -> ";

                            searchResult += to_string(path[i]);
                        }

                        binaryPathShown = true;
                    }
                    else
                    {
                        searchResult = "Node not found.";
                    }
                }

                // CHECK COMPLETION
                binaryLevelSuccess = binaryLcaShown && binaryPathShown;
            }



            else if (selectedLevel == 5)
            {
                // MIRROR TREE
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    binaryTree.mirror();
                    rebuildBinaryVisualTree();

                    binaryMirrorDone = true;
                    searchResult = "Tree mirrored successfully.";
                }

                // CHECK SYMMETRY
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    bool symmetric = binaryTree.isSymmetric();

                    searchResult = string("Symmetric? ") +
                        (symmetric ? "Yes" : "No");

                    binarySymmetryChecked = true;
                }

                // CHECK COMPLETION
                binaryLevelSuccess = binaryMirrorDone && binarySymmetryChecked;
                }
        }

        else if (selectedAct == 3)
        {
            if (isMouseOver(mousePosition, { width - 230.f, 185.f }, { 190.f, 35.f }))
            {
                searchInputActive = true;
            }
            else if (selectedLevel == 1)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 235.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter a node value.";
                    else
                    {
                        int bf = avlTree.balanceFactor(stoi(searchInput));
                        searchResult = bf == 999 ? "Node not found." : "Balance factor = " + to_string(bf);
                    }
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 280.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    searchResult = avlTree.isBalanced() ? "All nodes are AVL balanced." : "Tree is NOT AVL balanced.";
                }
            }
            else if (selectedLevel == 2)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 235.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter pivot value for LEFT rotation.";
                    else
                    {
                        int pivot = stoi(searchInput);
                        avlTree.reset();
                        avlTree.insertDemo(pivot);
                        avlTree.insertDemo(pivot + 10);
                        avlTree.insertDemo(pivot + 20);
                        if (avlTree.rotateLeftAt(pivot))
                        {
                            rebuildAVLVisualTree();
                            searchResult = "RR example repaired by a manual left rotation.";
                        }
                        else searchResult = "Cannot rotate left at that pivot.";
                    }
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 280.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter pivot value for RIGHT rotation.";
                    else
                    {
                        int pivot = stoi(searchInput);
                        avlTree.reset();
                        avlTree.insertDemo(pivot);
                        avlTree.insertDemo(pivot - 10);
                        avlTree.insertDemo(pivot - 20);
                        if (avlTree.rotateRightAt(pivot))
                        {
                            rebuildAVLVisualTree();
                            searchResult = "LL example repaired by a manual right rotation.";
                        }
                        else searchResult = "Cannot rotate right at that pivot.";
                    }
                }
            }
            else if (selectedLevel == 3)
            {
                searchInputActive = false;
                if (isMouseOver(mousePosition, { width - 230.f, 235.f }, { 190.f, 38.f }))
                {
                    avlTree.reset();
                    avlTree.insertDemo(30); avlTree.insertDemo(10); avlTree.insertDemo(20);
                    bool first = avlTree.rotateLeftAt(10);
                    bool second = first && avlTree.rotateRightAt(30);
                    rebuildAVLVisualTree();
                    searchResult = second ? "LR double rotation completed: 20 is the new root." : "LR rotation failed.";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 280.f }, { 190.f, 38.f }))
                {
                    avlTree.reset();
                    avlTree.insertDemo(30); avlTree.insertDemo(50); avlTree.insertDemo(40);
                    bool first = avlTree.rotateRightAt(50);
                    bool second = first && avlTree.rotateLeftAt(30);
                    rebuildAVLVisualTree();
                    searchResult = second ? "RL double rotation completed: 40 is the new root." : "RL rotation failed.";
                }
            }
            else if (selectedLevel == 4)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 235.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter a value to insert.";
                    else
                    {
                        avlTree.insert(stoi(searchInput));
                        rebuildAVLVisualTree();
                        searchResult = "Inserted. " + avlTree.getLastRotation();
                        if (avlTree.getLastRotation().empty()) searchResult = "Inserted; no rotation was needed.";
                    }
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 280.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter a value to delete.";
                    else
                    {
                        avlTree.remove(stoi(searchInput));
                        rebuildAVLVisualTree();
                        searchResult = "Deleted. " + avlTree.getLastRotation();
                        if (avlTree.getLastRotation().empty()) searchResult = "Delete completed; no rotation was needed.";
                    }
                }
            }
            else if (selectedLevel == 5)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 235.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter k first.";
                    else
                    {
                        int value = avlTree.kth(stoi(searchInput));
                        searchResult = value == -1 ? "Invalid k." : "kth smallest = " + to_string(value);
                    }
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 280.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    if (searchInput.empty()) searchResult = "Enter x first.";
                    else searchResult = "Rank (values <= x) = " + to_string(avlTree.rank(stoi(searchInput)));
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 325.f }, { 190.f, 38.f }))
                {
                    searchInputActive = false;
                    searchResult = "Count in [20, 60] = " + to_string(avlTree.countInRange(20, 60));
                }
            }

            if (isMouseOver(mousePosition, { 30.f, 620.f }, { 150.f, 50.f }))
            {
                searchInputActive = false;
                searchResult.clear();
                currentScreen = LEVEL_INTRO;
            }
        }
        else if (isMouseOver(mousePosition, { 760.f, 620.f }, { 200.f, 40.f }))
        {
        completeCurrentLevel();
        }   
    }
    else if (currentScreen == RESULT_SCREEN)
    {
        handleResultClick(mousePosition);
    }

    else if (currentScreen == SANDBOX)
    {
        if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
            currentScreen = MAIN_MENU;
        }
    }

    else if (currentScreen == LEADERBOARD)
    {
        if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
            currentScreen = MAIN_MENU;
        }
    }
}

void Game::completeCurrentLevel()
{
    int actIndex = selectedAct - 1;
    int levelIndex = selectedLevel - 1;

    // Mark current level as completed
    levelCompleted[actIndex][levelIndex] = true;

    // Unlock the next level
    if (selectedLevel < 5)
    {
        levelUnlocked[actIndex][levelIndex + 1] = true;
    }
    // Unlock the next act after completing Level 5
    else if (selectedAct < 4)
    {
        actUnlocked[actIndex + 1] = true;
        levelUnlocked[actIndex + 1][0] = true;
    }

    currentScreen = RESULT_SCREEN;
}

// Check whether mouse is inside a button
bool Game::isMouseOver(Vector2i mousePosition, Vector2f position, Vector2f size)
{
    return mousePosition.x >= position.x &&
        mousePosition.x <= position.x + size.x &&
        mousePosition.y >= position.y &&
        mousePosition.y <= position.y + size.y;
}


// Draw main menu
void Game::drawMainMenu()
{
    drawCenteredText("VAULT HEIST", 100.f, 55);
    drawCenteredText("THE LIVING VAULT", 165.f, 25);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    drawButton("CAMPAIGN", { centerX - 150.f, 260.f }, { 300.f, 55.f });
    drawButton("SANDBOX", { centerX - 150.f, 335.f }, { 300.f, 55.f });
    drawButton("LEADERBOARD", { centerX - 150.f, 410.f }, { 300.f, 55.f });
}


// Draw campaign map
void Game::drawCampaignMap()
{
    drawCenteredText("CAMPAIGN MAP", 90.f, 45);
    drawCenteredText("Complete each act to unlock the next one", 150.f, 22);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    drawButton("ACT 1 - BINARY TREE",
        { centerX - 150.f, 210.f }, { 300.f, 60.f }, actUnlocked[0]);

    drawButton("ACT 2 - BST",
        { centerX - 150.f, 290.f }, { 300.f, 60.f }, actUnlocked[1]);

    drawButton("ACT 3 - AVL TREE",
        { centerX - 150.f, 370.f }, { 300.f, 60.f }, actUnlocked[2]);

    drawButton("ACT 4 - HEAP FORTRESS",
        { centerX - 150.f, 450.f }, { 300.f, 60.f }, actUnlocked[3]);

    drawButton("BACK", { 30.f, 600.f }, { 150.f, 50.f });
}

// Draw level selection
void Game::drawLevelSelect()
{
    if (selectedAct == 1)
    {
        drawCenteredText("ACT 1 - BINARY TREE", 100.f, 40);
    }
    else if (selectedAct == 2)
    {
        drawCenteredText("ACT 2 - SORTED WING", 100.f, 40);
    }
    else if (selectedAct == 3)
    {
        drawCenteredText("ACT 3 - AVL TREE", 100.f, 40);
    }
    else if (selectedAct == 4)
    {
        drawCenteredText("ACT 4 - HEAP FORTRESS", 100.f, 40);
    }

    drawCenteredText("Choose a level", 180.f, 22);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    int actIndex = selectedAct - 1;

    drawButton("LEVEL 1",
        { centerX - 180.f, 230.f }, { 110.f, 65.f },
        levelUnlocked[actIndex][0]);

    drawButton("LEVEL 2",
        { centerX - 55.f, 230.f }, { 110.f, 65.f },
        levelUnlocked[actIndex][1]);

    drawButton("LEVEL 3",
        { centerX + 70.f, 230.f }, { 110.f, 65.f },
        levelUnlocked[actIndex][2]);

    drawButton("LEVEL 4",
        { centerX - 180.f, 320.f }, { 110.f, 65.f },
        levelUnlocked[actIndex][3]);

    drawButton("LEVEL 5",
        { centerX - 55.f, 320.f }, { 110.f, 65.f },
        levelUnlocked[actIndex][4]);

    drawButton("BACK", { 30.f, 600.f }, { 150.f, 50.f });
}


// Draw level introduction
void Game::drawLevelIntro()
{
    float centerX = static_cast<float>(window.getSize().x) / 2.f;
    if (selectedAct == 1)
    {
        drawCenteredText("ACT 1 - BINARY TREE", 100.f, 38);
        drawCenteredText("LEVEL " + to_string(selectedLevel), 200.f, 28);
        drawCenteredText("Explore binary tree traversals and operations.", 270.f, 20);
        drawCenteredText("Understand how nodes are connected.", 310.f, 18);
    }
    else if (selectedAct == 3)
    {
        drawCenteredText("ACT 3 - AVL TREE", 100.f, 38);
        drawCenteredText("LEVEL " + to_string(selectedLevel), 200.f, 28);
        drawCenteredText("Maintain a height-balanced search tree.", 270.f, 20);
        drawCenteredText("Explore rotations, insertion and deletion.", 310.f, 18);
    }
    if (selectedAct == 4)
    {
        drawCenteredText("ACT 4 - HEAP FORTRESS", 100.f, 38);

        if (selectedLevel == 1)
        {
            drawCenteredText("LEVEL 1 - ACTIVATE THE VAULT", 200.f, 28);
            drawCenteredText("Build a Max Heap from the given array.", 270.f, 20);
            drawCenteredText("Every parent must be greater than or equal to its children.", 310.f, 18);
        }
        else if (selectedLevel == 2)
        {
            drawCenteredText("LEVEL 2 - REPAIR THE SECURITY LOCKS", 200.f, 28);
            drawCenteredText("Restore the Max Heap property.", 270.f, 20);
            drawCenteredText("Use heapify to repair the disturbed heap.", 310.f, 18);
        }
        else if (selectedLevel == 3)
        {
            drawCenteredText("LEVEL 3 - EXTRACT GREATEST TREASURE", 200.f, 28);
            drawCenteredText("Repeatedly extract the maximum element.", 270.f, 20);
            drawCenteredText("Restore the heap after every extraction.", 310.f, 18);
        }
        else if (selectedLevel == 4)
        {
            drawCenteredText("LEVEL 4 - RACE AGAINST GUARDS", 200.f, 28);
            drawCenteredText("Perform heap operations within the allowed limits.", 270.f, 20);
            drawCenteredText("Complete the challenge efficiently.", 310.f, 18);
        }
        else if (selectedLevel == 5)
        {
            drawCenteredText("LEVEL 5 - FINAL VAULT", 200.f, 28);
            drawCenteredText("Complete Heap Sort.", 270.f, 20);
            drawCenteredText("Arrange the final array in ascending order.", 310.f, 18);
        }
    }
    else if (selectedAct == 2)
    {
        // Keep your existing Act 2 BST introduction here.
    }

    drawButton("START LEVEL", { centerX - 150.f, 450.f }, { 300.f, 60.f });
    drawButton("BACK", { 30.f, 600.f }, { 150.f, 50.f });
}


void Game::drawGameplay()
{
    // ACT 1 - BINARY TREE
    if (selectedAct == 1)
    {
        drawBinaryGameplay();
    }

    // ACT 2 - BST
    else if (selectedAct == 2)
    {
        float width = static_cast<float>(window.getSize().x);

        // Top HUD
        drawPanel({ 20.f, 15.f }, { width - 40.f, 65.f });
        drawText("LEVEL " + to_string(selectedLevel), { 40.f, 35.f }, 22);
        drawText("BST ACT", { width - 180.f, 35.f }, 22);

        // Mission panel
        drawPanel({ 20.f, 100.f }, { 230.f, 470.f });
        drawText("MISSION", { 40.f, 120.f }, 25);

        if (selectedLevel == 1)
        {
            drawText("SEARCH HEIST", { 40.f, 170.f }, 18);
            drawText("Find the target", { 40.f, 210.f }, 16);
            drawText("using BST rules.", { 40.f, 235.f }, 16);
            drawText("Choose LEFT or", { 40.f, 280.f }, 16);
            drawText("RIGHT at each", { 40.f, 305.f }, 16);
            drawText("comparison.", { 40.f, 330.f }, 16);
        }
        else if (selectedLevel == 2)
        {
            drawText("STASH LOOT", { 40.f, 170.f }, 18);
            drawText("Insert a new value", { 40.f, 210.f }, 16);
            drawText("in its correct BST", { 40.f, 235.f }, 16);
            drawText("position.", { 40.f, 260.f }, 16);
        }
        else if (selectedLevel == 3)
        {
            drawText("STEAL AND", { 40.f, 170.f }, 18);
            drawText("COLLAPSE", { 40.f, 195.f }, 18);
            drawText("Learn the three", { 40.f, 245.f }, 16);
            drawText("BST deletion cases.", { 40.f, 270.f }, 16);
        }
        else if (selectedLevel == 4)
        {
            drawText("RANGE RAID", { 40.f, 170.f }, 18);
            drawText("Find all values", { 40.f, 210.f }, 16);
            drawText("inside the given", { 40.f, 235.f }, 16);
            drawText("range.", { 40.f, 260.f }, 16);
            drawText("Skip unnecessary", { 40.f, 305.f }, 16);
            drawText("subtrees.", { 40.f, 330.f }, 16);
        }
        else if (selectedLevel == 5)
        {
            drawText("LAZY ARCHITECT", { 40.f, 170.f }, 18);
            drawText("Insert values in", { 40.f, 210.f }, 16);
            drawText("sorted order and", { 40.f, 235.f }, 16);
            drawText("observe how the", { 40.f, 260.f }, 16);
            drawText("BST becomes", { 40.f, 285.f }, 16);
            drawText("unbalanced.", { 40.f, 310.f }, 16);
        }

        // Tree visualization panel
        drawPanel({ 270.f, 100.f }, { width - 540.f, 470.f });
        drawText("TREE VISUALIZATION", { 290.f, 115.f }, 20);

        renderer.drawTree(window, visualRoot);

        // Operations panel
        drawPanel({ width - 250.f, 100.f }, { 230.f, 470.f });

        // Draw the interface for the selected BST level
        if (selectedLevel == 1)
        {
            drawBSTLevel1();
        }
        else if (selectedLevel == 2)
        {
            drawBSTLevel2();
        }
        else if (selectedLevel == 3)
        {
            drawBSTLevel3();
        }
        else if (selectedLevel == 4)
        {
            drawBSTLevel4();
        }
        else if (selectedLevel == 5)
        {
            drawBSTLevel5();
        }

        // Bottom toolbar
        drawButton("BACK", { 30.f, 620.f }, { 150.f, 50.f });
        drawButton("UNDO", { 220.f, 620.f }, { 150.f, 50.f });
        drawButton("HINT", { 410.f, 620.f }, { 150.f, 50.f });
        drawButton("RESET", { 600.f, 620.f }, { 150.f, 50.f });
        drawButton("COMPLETE LEVEL", { 800.f, 620.f }, { 170.f, 50.f },
            bstLevelSuccess);
    }

    // ACT 3 - AVL TREE
    else if (selectedAct == 3)
    {
        drawAVLGameplay();
    }

    // ACT 4 - MAX HEAP
    else if (selectedAct == 4)
    {
        drawHeapGameplay();
    }
}


// Draw placeholder screens
void Game::drawPlaceholder(const string& title, const string& message)
{
    drawCenteredText(title, 150.f, 45);
    drawCenteredText(message, 260.f, 22);

    drawButton("BACK", { 30.f, 600.f }, { 150.f, 50.f });
}


// Draw a button
void Game::drawButton(const string& text, Vector2f position, Vector2f size, bool enabled)
{
    RectangleShape button(size);

    button.setPosition(position);
    button.setFillColor(enabled ? Color(45, 65, 95) : Color(65, 65, 65));
    button.setOutlineColor(Color(100, 160, 210));
    button.setOutlineThickness(2.f);

    window.draw(button);

    if (uiFontLoaded)
    {
        Text label(uiFont, text, 20);

        FloatRect bounds = label.getLocalBounds();

        label.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        label.setPosition({
            position.x + size.x / 2.f,
            position.y + size.y / 2.f
            });

        label.setFillColor(Color::White);

        window.draw(label);
    }
}


// Draw centered text
void Game::drawCenteredText(const string& text, float y, unsigned int fontSize)
{
    if (!uiFontLoaded)
    {
        return;
    }

    Text label(uiFont, text, fontSize);

    FloatRect bounds = label.getLocalBounds();

    label.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
        });

    label.setPosition({
        static_cast<float>(window.getSize().x) / 2.f,
        y
        });

    label.setFillColor(Color::White);

    window.draw(label);
}


// Draw text at a specific position
void Game::drawText(const string& text, Vector2f position, unsigned int fontSize)
{
    if (!uiFontLoaded)
    {
        return;
    }

    Text label(uiFont, text, fontSize);

    label.setPosition(position);
    label.setFillColor(Color::White);

    window.draw(label);
}


// Draw panel
void Game::drawPanel(Vector2f position, Vector2f size)
{
    RectangleShape panel(size);

    panel.setPosition(position);
    panel.setFillColor(Color(28, 35, 52));
    panel.setOutlineColor(Color(65, 85, 115));
    panel.setOutlineThickness(1.5f);

    window.draw(panel);
}


// Calculate visual tree positions
void Game::positionGameplayTree()
{
    float width = static_cast<float>(window.getSize().x);

    layoutEngine.calculatePositions(visualRoot, width * 0.5f, 200.f, 90.f);

    shiftTree(visualRoot, width * 0.25f);
}


// Shift visual tree horizontally
void Game::shiftTree(VisualNode* node, float offsetX)
{
    if (node == nullptr)
    {
        return;
    }

    node->position.x += offsetX;

    shiftTree(node->left, offsetX);
    shiftTree(node->right, offsetX);
}


// Convert a BST Node into a VisualNode
VisualNode* Game::convertToVisualTree(Node* node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    VisualNode* visualNode = new VisualNode;

    visualNode->value = node->data;
    visualNode->left = convertToVisualTree(node->left);
    visualNode->right = convertToVisualTree(node->right);

    return visualNode;
}


// ACT 1 - Binary Tree
VisualNode* Game::convertBinaryToVisualTree(Node* node)
{
    if (node == nullptr)
        return nullptr;

    VisualNode* visualNode = new VisualNode;
    visualNode->value = node->data;

    visualNode->left = convertBinaryToVisualTree(node->left);
    visualNode->right = convertBinaryToVisualTree(node->right);

    return visualNode;
}


// ACT 3 - AVL Tree
VisualNode* Game::convertAVLToVisualTree(AVLNode* node)
{
    if (node == nullptr)
        return nullptr;

    VisualNode* visualNode = new VisualNode;
    visualNode->value = node->data;

    visualNode->left = convertAVLToVisualTree(node->left);
    visualNode->right = convertAVLToVisualTree(node->right);

    return visualNode;
}

// Delete visual tree to release its allocated memory
void Game::deleteVisualTree(VisualNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    deleteVisualTree(node->left);
    deleteVisualTree(node->right);

    delete node;
}


// Rebuild visual tree from the actual BST
void Game::rebuildVisualTree()
{
    deleteVisualTree(visualRoot);

    visualRoot = convertToVisualTree(bst.getRoot());

    positionGameplayTree();
}



void Game::rebuildBinaryVisualTree()
{
    deleteVisualTree(binaryVisualRoot);

    binaryVisualRoot = convertBinaryToVisualTree(binaryTree.getRoot());

    float width = (float)window.getSize().x;

    layoutEngine.calculatePositions(binaryVisualRoot, width * 0.5f, 200.f, 90.f);
    shiftTree(binaryVisualRoot, width * 0.25f);
}


void Game::rebuildAVLVisualTree()
{
    deleteVisualTree(avlVisualRoot);

    avlVisualRoot = convertAVLToVisualTree(avlTree.getRoot());

    float width = (float)window.getSize().x;

    layoutEngine.calculatePositions(avlVisualRoot, width * 0.5f, 200.f, 90.f);
    shiftTree(avlVisualRoot, width * 0.25f);
}
// Clear all highlighted nodes
void Game::clearHighlights(VisualNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    node->highlighted = false;

    clearHighlights(node->left);
    clearHighlights(node->right);
}


// Highlight a node with the given value
void Game::highlightNode(VisualNode* node, int value)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->value == value)
    {
        node->highlighted = true;
    }

    highlightNode(node->left, value);
    highlightNode(node->right, value);
}


// Perform BST search
void Game::performSearch()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int target = stoi(searchInput);

    int path[100];
    int pathLength = 0;

    clearHighlights(visualRoot);

    // Call your BST search function
    bool found = bst.search(target, path, &pathLength);

    // Highlight every visited node
    for (int i = 0; i < pathLength; i++)
    {
        highlightNode(visualRoot, path[i]);
    }

    // Build the search path text
    string pathText = "Path: ";

    for (int i = 0; i < pathLength; i++)
    {
        pathText += to_string(path[i]);

        if (i < pathLength - 1)
        {
            pathText += " -> ";
        }
    }

    if (found)
    {
        searchResult = "Found! " + pathText;
    }
    else
    {
        searchResult = "Not found. " + pathText;
    }
}

void Game::performInsert()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int value = stoi(searchInput);

    int path[100];
    int pathLength = 0;

    // Check whether the value already exists
    bool exists = bst.search(value, path, &pathLength);

    clearHighlights(visualRoot);

    if (exists)
    {
        // Highlight the existing value
        highlightNode(visualRoot, value);

        searchResult = "Duplicate! Value exists.";
        return;
    }

    // Insert the new value into the actual BST
    bst.insert(value);

    // Rebuild the visual tree to show the new node
    rebuildVisualTree();

    // Highlight the newly inserted node
    highlightNode(visualRoot, value);

    searchResult = "Successfully inserted!";
}

void Game::performDelete()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int value = stoi(searchInput);

    clearHighlights(visualRoot);

    bool deleted = bst.deleteValue(value);

    if (deleted)
    {
        rebuildVisualTree();
        searchResult = "Successfully deleted!";
    }
    else
    {
        searchResult = "Value not found!";
    }
}

void Game::performMinimum()
{
    int minimumValue;

    clearHighlights(visualRoot);

    if (bst.findMinimum(&minimumValue))
    {
        highlightNode(visualRoot, minimumValue);

        searchResult = "Minimum: " + to_string(minimumValue);
    }
    else
    {
        searchResult = "Tree is empty!";
    }
}

void Game::performMaximum()
{
    int maximumValue;

    clearHighlights(visualRoot);

    if (bst.findMaximum(&maximumValue))
    {
        highlightNode(visualRoot, maximumValue);

        searchResult = "Maximum: " + to_string(maximumValue);
    }
    else
    {
        searchResult = "Tree is empty!";
    }
}

void Game::performSuccessor()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int value = stoi(searchInput);
    int successorValue;

    clearHighlights(visualRoot);

    if (bst.findSuccessor(value, &successorValue))
    {
        highlightNode(visualRoot, successorValue);

        searchResult = "Successor: " + to_string(successorValue);
    }
    else
    {
        searchResult = "No successor found!";
    }
}

void Game::performPredecessor()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int value = stoi(searchInput);
    int predecessorValue;

    clearHighlights(visualRoot);

    if (bst.findPredecessor(value, &predecessorValue))
    {
        highlightNode(visualRoot, predecessorValue);

        searchResult = "Predecessor: " + to_string(predecessorValue);
    }
    else
    {
        searchResult = "No predecessor found!";
    }
}

// ACT 3 - AVL INSERT
void Game::performAVLInsert()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int value = stoi(searchInput);

    avlTree.insert(value);
    rebuildAVLVisualTree();

    clearHighlights(avlVisualRoot);
    highlightNode(avlVisualRoot, value);

    searchResult = "AVL value inserted successfully!";
    searchInput = "";
}

// ACT 3 - AVL DELETE
void Game::performAVLDelete()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    int value = stoi(searchInput);

    avlTree.remove(value);
    rebuildAVLVisualTree();

    clearHighlights(avlVisualRoot);

    searchResult = "AVL delete operation completed!";
    searchInput = "";
}

VisualNode* Game::convertHeapToVisualTree(int index)
{
    int value;

    if (!maxHeap.getValueAt(index, &value))
    {
        return nullptr;
    }

    VisualNode* node = new VisualNode;

    node->value = value;

    node->left = convertHeapToVisualTree(2 * index + 1);
    node->right = convertHeapToVisualTree(2 * index + 2);

    return node;
}

void Game::rebuildHeapVisualTree()
{
    deleteHeapVisualTree(heapVisualRoot);

    heapVisualRoot = convertHeapToVisualTree(0);

    float width = static_cast<float>(window.getSize().x);

    layoutEngine.calculatePositions(heapVisualRoot, width * 0.5f, 200.f, 90.f);

    shiftTree(heapVisualRoot, width * 0.25f);
}

void Game::deleteHeapVisualTree(VisualNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    deleteHeapVisualTree(node->left);
    deleteHeapVisualTree(node->right);

    delete node;
}

void Game::drawHeapArray()
{
    drawText("Heap Array", Vector2f(420.f, 465.f), 24);

    int totalElements = maxHeap.getSize();

    float cellWidth = 70.f;
    float cellHeight = 55.f;
    float gap = 10.f;

    float totalWidth = totalElements * cellWidth +
        (totalElements - 1) * gap;

    float startX = (window.getSize().x - totalWidth) / 2.f;

    for (int i = 0; i < totalElements; i++)
    {
        int value;

        if (maxHeap.getValueAt(i, &value))
        {
            float x = startX + i * (cellWidth + gap);

            RectangleShape cell(Vector2f(cellWidth, cellHeight));

            cell.setPosition(Vector2f(x, 505.f));
            if (selectedHeapIndex == i)
            {
                cell.setFillColor(Color(180, 120, 40));
            }
            else
            {
                cell.setFillColor(Color(45, 55, 75));
            }
            cell.setOutlineColor(Color::White);
            cell.setOutlineThickness(2.f);

            window.draw(cell);

            drawText(to_string(value), Vector2f(x + 20.f, 515.f), 22);

            drawText(to_string(i), Vector2f(x + 28.f, 565.f), 16);
        }
    }
}

void Game::drawHeapGameplay()
{
    if (heapResult != "Heap Sort completed!" && heapVisualRoot != nullptr)
    {
        renderer.drawTree(window, heapVisualRoot);
    }

    drawHeapArray();
    drawHeapOperations();
    drawHeapEventLog();
}

void Game::highlightHeapNode(VisualNode* node, int targetIndex, int currentIndex)
{
    if (node == nullptr)
    {
        return;
    }

    if (currentIndex == targetIndex)
    {
        node->highlighted = true;
        return;
    }

    node->highlighted = false;

    highlightHeapNode(node->left, targetIndex, 2 * currentIndex + 1);
    highlightHeapNode(node->right, targetIndex, 2 * currentIndex + 2);
}

void Game::drawHeapOperations()
{
    drawText("Heap Operations", Vector2f(760.f, 180.f), 24);

    if (selectedLevel == 1)
    {
        drawButton("Build Max Heap", { 760.f, 230.f }, { 200.f, 40.f });
        drawButton("Undo", { 760.f, 280.f }, { 95.f, 40.f });
        drawButton("Replay", { 865.f, 280.f }, { 95.f, 40.f });
    }
    else if (selectedLevel == 2)
    {
        drawButton("Heapify", { 760.f, 230.f }, { 200.f, 40.f });
        drawButton("Undo", { 760.f, 280.f }, { 95.f, 40.f });
        drawButton("Replay", { 865.f, 280.f }, { 95.f, 40.f });
    }
    else if (selectedLevel == 3)
    {
        drawButton("Extract Maximum", { 760.f, 230.f }, { 200.f, 40.f });
        drawButton("Undo", { 760.f, 280.f }, { 95.f, 40.f });
        drawButton("Replay", { 865.f, 280.f }, { 95.f, 40.f });
    }
    else if (selectedLevel == 4)
    {
        drawButton("Extract Maximum", { 760.f, 230.f }, { 200.f, 40.f });
        drawButton("Undo", { 760.f, 280.f }, { 95.f, 40.f });
        drawButton("Replay", { 865.f, 280.f }, { 95.f, 40.f });
    }
    else if (selectedLevel == 5)
    {
        drawButton("Heap Sort", { 760.f, 230.f }, { 200.f, 40.f });
        drawButton("Undo", { 760.f, 280.f }, { 95.f, 40.f });
        drawButton("Replay", { 865.f, 280.f }, { 95.f, 40.f });
    }

    drawText(heapResult, Vector2f(760.f, 350.f), 16);

    drawText("Enter value:", Vector2f(760.f, 410.f), 16);
    drawPanel(Vector2f(760.f, 435.f), Vector2f(200.f, 35.f));
    drawText(heapInput, Vector2f(770.f, 440.f), 18);

    drawButton("Insert", { 760.f, 480.f }, { 200.f, 40.f });

    drawButton("Reset Level", { 760.f, 535.f }, { 200.f, 40.f });
}

void Game::loadHeapLevel()
{
    if (selectedLevel == 1)
    {
        int values[] = { 10, 40, 30, 5, 20, 15 };
        maxHeap.loadArray(values, 6);
    }
    else if (selectedLevel == 2)
    {
        int values[] = { 50, 30, 40, 60, 20, 10 };
        maxHeap.loadArray(values, 6);
    }
    else if (selectedLevel == 3)
    {
        int values[] = { 90, 70, 80, 20, 50, 60 };
        maxHeap.loadArray(values, 6);
    }
    else if (selectedLevel == 4)
    {
        int values[] = { 100, 60, 90, 20, 50, 70 };
        maxHeap.loadArray(values, 6);
    }
    else if (selectedLevel == 5)
    {
        int values[] = { 45, 20, 35, 10, 15, 30 };
        maxHeap.loadArray(values, 6);
    }

    rebuildHeapVisualTree();
}

void Game::saveHeapState()
{
    delete[] previousHeapState;

    previousHeapSize = maxHeap.getSize();

    previousHeapState = new int[previousHeapSize];

    for (int i = 0; i < previousHeapSize; i++)
    {
        maxHeap.getValueAt(i, &previousHeapState[i]);
    }

    canUndo = true;
}

void Game::undoHeapOperation()
{
    if (!canUndo)
    {
        heapResult = "Nothing to undo!";
        return;
    }

    maxHeap.loadArray(previousHeapState, previousHeapSize);

    rebuildHeapVisualTree();

    selectedHeapIndex = -1;

    heapResult = "Operation undone!";

    canUndo = false;
}

void Game::replayHeapLevel()
{
    loadHeapLevel();

    selectedHeapIndex = -1;

    heapInput = "";
    heapResult = "";
    heapInputActive = false;

    heapEventCount = 0;
    canUndo = false;
}

void Game::addHeapEvent(string event)
{
    if (heapEventCount < 6)
    {
        heapEvents[heapEventCount] = event;
        heapEventCount++;
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            heapEvents[i] = heapEvents[i + 1];
        }

        heapEvents[5] = event;
    }
}

void Game::drawHeapEventLog()
{
    drawText("EVENT LOG", Vector2f(30.f, 590.f), 20);

    drawPanel(Vector2f(30.f, 620.f), Vector2f(680.f, 65.f));

    for (int i = 0; i < heapEventCount; i++)
    {
        drawText(heapEvents[i], Vector2f(40.f + i * 110.f, 642.f), 13);
    }
}

// ACT 1 - BINARY TREE GAMEPLAY
void Game::drawBinaryGameplay()
{
    float width = static_cast<float>(window.getSize().x);
    drawPanel({ 20.f, 15.f }, { width - 40.f, 65.f });
    drawText("LEVEL " + to_string(selectedLevel), { 40.f, 35.f }, 22);
    drawText("ACT 1 - BINARY TREE", { width - 300.f, 35.f }, 22);

    drawPanel({ 20.f, 100.f }, { 230.f, 470.f });
    drawText("MISSION", { 40.f, 120.f }, 24);
    if (selectedLevel == 1)
    {
        drawText("Choose the correct", { 40.f, 180.f }, 16);
        drawText("PREORDER traversal.", { 40.f, 210.f }, 16);
        drawText("Root - Left - Right", { 40.f, 260.f }, 15);
    }
    else if (selectedLevel == 2)
    {
        drawText("Reconstruct the", { 40.f, 180.f }, 17);
        drawText("tree using traversal", { 40.f, 210.f }, 16);
        drawText("sequences.", { 40.f, 240.f }, 16);
    }
    else if (selectedLevel == 3)
    {
        drawText("Explore tree height", { 40.f, 180.f }, 16);
        drawText("and diameter.", { 40.f, 210.f }, 16);
    }
    else if (selectedLevel == 4)
    {
        drawText("Find the Lowest", { 40.f, 180.f }, 16);
        drawText("Common Ancestor", { 40.f, 210.f }, 16);
        drawText("and a root path.", { 40.f, 240.f }, 16);
    }
    else
    {
        drawText("Mirror the tree", { 40.f, 180.f }, 17);
        drawText("and inspect its", { 40.f, 210.f }, 16);
        drawText("symmetry.", { 40.f, 240.f }, 16);
    }

    drawPanel({ 270.f, 100.f }, { width - 540.f, 470.f });
    drawText("TREE VISUALIZATION", { 290.f, 115.f }, 20);
    renderer.drawTree(window, binaryVisualRoot);

    drawPanel({ width - 250.f, 100.f }, { 230.f, 470.f });
    if (selectedLevel == 1)
    {
        drawText("SELECT PREORDER", { width - 230.f, 135.f }, 16);
        drawButton("1 - 2 - 4 - 5 - 3", { width - 230.f, 190.f }, { 190.f, 38.f });
        drawButton("4 - 2 - 5 - 1 - 3", { width - 230.f, 240.f }, { 190.f, 38.f });
        drawButton("4 - 5 - 2 - 3 - 1", { width - 230.f, 290.f }, { 190.f, 38.f });
        drawButton("1 - 2 - 3 - 4 - 5", { width - 230.f, 340.f }, { 190.f, 38.f });
    }
    else if (selectedLevel == 2)
    {
        drawText("RECONSTRUCTION", { width - 230.f, 135.f }, 17);
        drawButton("BUILD PRE + IN", { width - 230.f, 190.f }, { 190.f, 38.f });
        drawButton("BUILD POST + IN", { width - 230.f, 240.f }, { 190.f, 38.f });
    }
    else if (selectedLevel == 3)
    {
        drawText("TREE MEASUREMENTS", { width - 230.f, 135.f }, 16);
        drawButton("SHOW HEIGHT", { width - 230.f, 190.f }, { 190.f, 38.f });
        drawButton("SHOW DIAMETER", { width - 230.f, 240.f }, { 190.f, 38.f });
    }
    else if (selectedLevel == 4)
    {
        drawText("ANCESTOR & PATH", { width - 230.f, 135.f }, 16);
        drawButton("LCA OF 4 AND 5", { width - 230.f, 190.f }, { 190.f, 38.f });
        drawButton("PATH TO NODE 4", { width - 230.f, 240.f }, { 190.f, 38.f });
    }
    else
    {
        drawText("MIRROR CHALLENGE", { width - 230.f, 135.f }, 16);
        drawButton("MIRROR TREE", { width - 230.f, 190.f }, { 190.f, 38.f });
        drawButton("CHECK SYMMETRY", { width - 230.f, 240.f }, { 190.f, 38.f });
    }
    drawText(searchResult, { width - 230.f, 330.f }, 13);
    drawButton("COMPLETE LEVEL", { 800.f, 620.f },
        { 170.f, 50.f }, binaryLevelSuccess);
    drawButton("BACK", { 30.f, 620.f }, { 150.f, 50.f });
}


// ACT 3 - AVL TREE GAMEPLAY
void Game::drawAVLGameplay()
{
    float width = static_cast<float>(window.getSize().x);
    drawPanel({ 20.f, 15.f }, { width - 40.f, 65.f });
    drawText("LEVEL " + to_string(selectedLevel), { 40.f, 35.f }, 22);
    drawText("ACT 3 - AVL TREE", { width - 250.f, 35.f }, 22);

    drawPanel({ 20.f, 100.f }, { 230.f, 470.f });
    drawText("MISSION", { 40.f, 120.f }, 24);
    if (selectedLevel == 1)
    {
        drawText("Inspect node balance", { 40.f, 180.f }, 15);
        drawText("factors and verify", { 40.f, 210.f }, 15);
        drawText("AVL balance.", { 40.f, 240.f }, 15);
    }
    else if (selectedLevel == 2)
    {
        drawText("Perform a single", { 40.f, 180.f }, 16);
        drawText("left or right", { 40.f, 210.f }, 16);
        drawText("rotation manually.", { 40.f, 240.f }, 16);
    }
    else if (selectedLevel == 3)
    {
        drawText("Perform LR and RL", { 40.f, 180.f }, 16);
        drawText("double rotations.", { 40.f, 210.f }, 16);
        drawText("Reset to retry.", { 40.f, 250.f }, 15);
    }
    else if (selectedLevel == 4)
    {
        drawText("Insert or delete", { 40.f, 180.f }, 16);
        drawText("values and observe", { 40.f, 210.f }, 16);
        drawText("automatic rotations.", { 40.f, 240.f }, 16);
    }
    else
    {
        drawText("Use subtree sizes", { 40.f, 180.f }, 16);
        drawText("to find kth, rank", { 40.f, 210.f }, 16);
        drawText("and range count.", { 40.f, 240.f }, 16);
    }

    drawPanel({ 270.f, 100.f }, { width - 540.f, 470.f });
    drawText("TREE VISUALIZATION", { 290.f, 115.f }, 20);
    renderer.drawTree(window, avlVisualRoot);

    drawPanel({ width - 250.f, 100.f }, { 230.f, 470.f });
    drawText("AVL CHALLENGE", { width - 230.f, 120.f }, 18);
    drawText("Value / k:", { width - 230.f, 155.f }, 15);

    RectangleShape inputBox({ 190.f, 35.f });
    inputBox.setPosition({ width - 230.f, 175.f });
    inputBox.setFillColor(Color(18, 22, 35));
    inputBox.setOutlineColor(searchInputActive ? Color(70, 180, 240) : Color(100, 120, 145));
    inputBox.setOutlineThickness(2.f);
    window.draw(inputBox);
    drawText(searchInput.empty() ? "Enter value..." : searchInput,
        { width - 220.f, 182.f }, 16);

    if (selectedLevel == 1)
    {
        drawButton("CHECK BALANCE FACTOR", { width - 230.f, 235.f }, { 190.f, 38.f });
        drawButton("VERIFY AVL TREE", { width - 230.f, 280.f }, { 190.f, 38.f });
    }
    else if (selectedLevel == 2)
    {
        drawButton("ROTATE LEFT AT x", { width - 230.f, 235.f }, { 190.f, 38.f });
        drawButton("ROTATE RIGHT AT x", { width - 230.f, 280.f }, { 190.f, 38.f });
    }
    else if (selectedLevel == 3)
    {
        drawButton("LR DOUBLE ROTATION", { width - 230.f, 235.f }, { 190.f, 38.f });
        drawButton("RL DOUBLE ROTATION", { width - 230.f, 280.f }, { 190.f, 38.f });
    }
    else if (selectedLevel == 4)
    {
        drawButton("INSERT VALUE", { width - 230.f, 235.f }, { 190.f, 38.f });
        drawButton("DELETE VALUE", { width - 230.f, 280.f }, { 190.f, 38.f });
    }
    else
    {
        drawButton("KTH SMALLEST", { width - 230.f, 235.f }, { 190.f, 38.f });
        drawButton("RANK OF x", { width - 230.f, 280.f }, { 190.f, 38.f });
        drawButton("COUNT RANGE 20-60", { width - 230.f, 325.f }, { 190.f, 38.f });
    }

    drawText("Height: " + to_string(avlTree.getHeight()) +
        (avlTree.isBalanced() ? " | AVL balanced" : " | Not balanced"),
        { width - 230.f, 390.f }, 12);
    drawText(searchResult, { width - 230.f, 425.f }, 12);
    drawButton("BACK", { 30.f, 620.f }, { 150.f, 50.f });
}

void Game::drawResultScreen()
{
    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    if (selectedAct == 4 && selectedLevel == 5)
    {
        drawCenteredText("CAMPAIGN COMPLETED!", 100.f, 42);
        drawCenteredText("You have completed all four Acts!", 180.f, 24);
    }
    else
    {
        drawCenteredText("LEVEL COMPLETED!", 100.f, 42);
        drawCenteredText("Well done! You completed the level.", 180.f, 24);
    }

    drawCenteredText(
        "ACT " + to_string(selectedAct) +
        " - LEVEL " + to_string(selectedLevel),
        250.f, 25
    );

    drawButton(
        "RETRY LEVEL",
        { centerX - 150.f, 350.f },
        { 300.f, 50.f }
    );

    if (selectedLevel < 5 || selectedAct < 4)
    {
        drawButton(
            "NEXT LEVEL",
            { centerX - 150.f, 420.f },
            { 300.f, 50.f }
        );
    }

    drawButton(
        "CAMPAIGN MAP",
        { centerX - 150.f, 490.f },
        { 300.f, 50.f }
    );
}

void Game::handleResultClick(Vector2i mousePosition)
{
    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    // Retry current level
    if (isMouseOver(mousePosition,
        { centerX - 150.f, 350.f }, { 300.f, 50.f }))
    {
        currentScreen = LEVEL_INTRO;
    }

    // Move to the next level
    else if (isMouseOver(mousePosition,
        { centerX - 150.f, 420.f }, { 300.f, 50.f }))
    {
        if (selectedLevel < 5)
        {
            selectedLevel++;
        }
        else if (selectedAct < 4)
        {
            selectedAct++;
            selectedLevel = 1;
        }

        currentScreen = LEVEL_INTRO;
    }

    // Return to campaign map
    else if (isMouseOver(mousePosition,
        { centerX - 150.f, 490.f }, { 300.f, 50.f }))
    {
        currentScreen = CAMPAIGN_MAP;
    }
}


void Game::startBSTSearch()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a target value first.";
        return;
    }

    bstSearchTarget = stoi(searchInput);

    bstCurrentNode = bst.getRoot();

    bstComparisons = 0;
    bstSearchPathLength = 0;

    bstLevelSuccess = false;
    bstSearchStarted = true;

    clearHighlights(visualRoot);

    if (bstCurrentNode == nullptr)
    {
        searchResult = "BST is empty!";
        bstSearchStarted = false;
        return;
    }

    // Examine the root node
    bstComparisons++;

    bstSearchPath[bstSearchPathLength] = bstCurrentNode->data;
    bstSearchPathLength++;

    highlightNode(visualRoot, bstCurrentNode->data);

    if (bstCurrentNode->data == bstSearchTarget)
    {
        searchResult = "Target found!";
        bstLevelSuccess = true;
        bstSearchStarted = false;
    }
    else
    {
        searchResult = "At " + to_string(bstCurrentNode->data) +
            ". Choose LEFT or RIGHT.";
    }
}


void Game::moveBSTLeft()
{
    if (!bstSearchStarted || bstCurrentNode == nullptr)
    {
        return;
    }

    // Left is correct only if target is smaller
    if (bstSearchTarget >= bstCurrentNode->data)
    {
        searchResult = "Wrong direction! Choose RIGHT.";
        return;
    }

    // Move to left child
    bstCurrentNode = bstCurrentNode->left;

    if (bstCurrentNode == nullptr)
    {
        searchResult = "Target not found! Left child is empty.";
        bstSearchStarted = false;
        return;
    }

    bstComparisons++;

    bstSearchPath[bstSearchPathLength] = bstCurrentNode->data;
    bstSearchPathLength++;

    highlightNode(visualRoot, bstCurrentNode->data);

    if (bstCurrentNode->data == bstSearchTarget)
    {
        searchResult = "Target found!";
        bstLevelSuccess = true;
        bstSearchStarted = false;
    }
    else
    {
        searchResult = "At " + to_string(bstCurrentNode->data) +
            ". Choose LEFT or RIGHT.";
    }
}


void Game::moveBSTRight()
{
    if (!bstSearchStarted || bstCurrentNode == nullptr)
    {
        return;
    }

    // Right is correct only if target is larger
    if (bstSearchTarget <= bstCurrentNode->data)
    {
        searchResult = "Wrong direction! Choose LEFT.";
        return;
    }

    // Move to right child
    bstCurrentNode = bstCurrentNode->right;

    if (bstCurrentNode == nullptr)
    {
        searchResult = "Target not found! Right child is empty.";
        bstSearchStarted = false;
        return;
    }

    bstComparisons++;

    bstSearchPath[bstSearchPathLength] = bstCurrentNode->data;
    bstSearchPathLength++;

    highlightNode(visualRoot, bstCurrentNode->data);

    if (bstCurrentNode->data == bstSearchTarget)
    {
        searchResult = "Target found!";
        bstLevelSuccess = true;
        bstSearchStarted = false;
    }
    else
    {
        searchResult = "At " + to_string(bstCurrentNode->data) +
            ". Choose LEFT or RIGHT.";
    }
}

void Game::performRangeQuery()
{
    if (rangeLowInput.empty() || rangeHighInput.empty())
    {
        searchResult = "Enter both range limits.";
        return;
    }

    int low = stoi(rangeLowInput);
    int high = stoi(rangeHighInput);

    if (low > high)
    {
        searchResult = "Invalid range!";
        return;
    }

    int output[100];
    int count = 0;

    bst.rangeQuery(low, high, output, count);

    searchResult = "Range: ";

    for (int i = 0; i < count; i++)
    {
        searchResult += to_string(output[i]);

        if (i < count - 1)
            searchResult += ", ";
    }

    if (count == 0)
        searchResult = "No values in this range.";
}

void Game::startRangeRaid()
{
    if (rangeLowInput.empty() || rangeHighInput.empty())
    {
        searchResult = "Enter both range limits.";
        return;
    }

    rangeLow = stoi(rangeLowInput);
    rangeHigh = stoi(rangeHighInput);

    if (rangeLow > rangeHigh)
    {
        searchResult = "Lower limit must be smaller.";
        return;
    }

    // Reset traversal information.
    rangeStackTop = -1;
    rangeResultCount = 0;

    rangeNodesVisited = 0;
    rangeSubtreesPruned = 0;
    rangeCorrectDecisions = 0;
    rangeWrongDecisions = 0;

    rangeCurrentNode = nullptr;
    rangeDecisionType = 0;

    rangeQueryFinished = false;
    rangeQueryStarted = true;

    // Start traversal from the root.
    if (bst.getRoot() != nullptr)
    {
        rangeStack[++rangeStackTop] = bst.getRoot();
    }

    searchResult = "Range raid started!";

    prepareNextRangeNode();
}

void Game::prepareNextRangeNode()
{
    rangeCurrentNode = nullptr;

    if (rangeStackTop < 0)
    {
        finishRangeRaid();
        return;
    }

    rangeCurrentNode = rangeStack[rangeStackTop--];

    rangeNodesVisited++;

    if (rangeCurrentNode->data < rangeLow)
    {
        rangeDecisionType = 1;
    }
    else if (rangeCurrentNode->data > rangeHigh)
    {
        rangeDecisionType = 2;
    }
    else
    {
        rangeDecisionType = 3;
    }
}

void Game::handleRangeDecision(int decision)
{
    if (!rangeQueryStarted || rangeQueryFinished ||
        rangeCurrentNode == nullptr)
    {
        return;
    }

    if (decision != rangeDecisionType)
    {
        rangeWrongDecisions++;
        searchResult = "Incorrect decision! Try again.";
        return;
    }

    rangeCorrectDecisions++;

    if (rangeDecisionType == 1)
    {
        // Current node is smaller than the lower limit.
        // Its entire left subtree can be skipped.

        rangeSubtreesPruned++;

        if (rangeCurrentNode->right != nullptr)
        {
            rangeStack[++rangeStackTop] = rangeCurrentNode->right;
        }

        searchResult = "Correct! Skip the left subtree.";
    }
    else if (rangeDecisionType == 2)
    {
        // Current node is greater than the upper limit.
        // Its entire right subtree can be skipped.

        rangeSubtreesPruned++;

        if (rangeCurrentNode->left != nullptr)
        {
            rangeStack[++rangeStackTop] = rangeCurrentNode->left;
        }

        searchResult = "Correct! Skip the right subtree.";
    }
    else
    {
        // Current node is inside the requested range.

        rangeResults[rangeResultCount++] = rangeCurrentNode->data;

        // Push right first so that left is processed first.
        if (rangeCurrentNode->right != nullptr)
        {
            rangeStack[++rangeStackTop] = rangeCurrentNode->right;
        }

        if (rangeCurrentNode->left != nullptr)
        {
            rangeStack[++rangeStackTop] = rangeCurrentNode->left;
        }

        searchResult = "Correct! Explore both subtrees.";
    }

    prepareNextRangeNode();
}

void Game::finishRangeRaid()
{
    rangeCurrentNode = nullptr;
    rangeQueryFinished = true;

    // Sort results in ascending order.
    for (int i = 1; i < rangeResultCount; i++)
    {
        int key = rangeResults[i];
        int j = i - 1;

        while (j >= 0 && rangeResults[j] > key)
        {
            rangeResults[j + 1] = rangeResults[j];
            j--;
        }

        rangeResults[j + 1] = key;
    }

    searchResult = "Range raid completed!";

    bstLevelSuccess = true;
}

void Game::resetRangeRaid()
{
    rangeLowInput = "";
    rangeHighInput = "";

    rangeLowInputActive = false;
    rangeHighInputActive = false;

    rangeLow = 0;
    rangeHigh = 0;

    rangeQueryStarted = false;
    rangeQueryFinished = false;

    rangeCurrentNode = nullptr;

    rangeStackTop = -1;
    rangeResultCount = 0;

    rangeNodesVisited = 0;
    rangeSubtreesPruned = 0;
    rangeCorrectDecisions = 0;
    rangeWrongDecisions = 0;

    rangeDecisionType = 0;

    bstLevelSuccess = false;
    searchResult = "";
}


void Game::drawBSTLevel1()
{
    float width = static_cast<float>(window.getSize().x);

    drawText("SEARCH HEIST", { width - 230.f, 120.f }, 21);
    drawText("TARGET VALUE", { width - 230.f, 175.f }, 16);

    RectangleShape inputBox({ 190.f, 40.f });
    inputBox.setPosition({ width - 230.f, 205.f });
    inputBox.setFillColor(Color(18, 22, 35));
    inputBox.setOutlineColor(searchInputActive ?
        Color(70, 180, 240) : Color(100, 120, 145));
    inputBox.setOutlineThickness(2.f);
    window.draw(inputBox);

    drawText(searchInput.empty() ? "Enter value..." : searchInput,
        { width - 220.f, 215.f }, 18);

    drawButton("START SEARCH",
        { width - 230.f, 260.f }, { 190.f, 38.f });

    drawButton("GO LEFT",
        { width - 230.f, 310.f }, { 190.f, 38.f });

    drawButton("GO RIGHT",
        { width - 230.f, 355.f }, { 190.f, 38.f });

    drawText("COMPARISONS: " + to_string(bstComparisons),
        { width - 230.f, 410.f }, 15);

    string pathText = "PATH: ";

    for (int i = 0; i < bstSearchPathLength; i++)
    {
        pathText += to_string(bstSearchPath[i]);

        if (i < bstSearchPathLength - 1)
            pathText += " -> ";
    }

    drawText(pathText, { width - 230.f, 440.f }, 13);
    drawText(searchResult, { width - 230.f, 475.f }, 13);
}

void Game::drawBSTLevel2()
{
    float width = static_cast<float>(window.getSize().x);

    drawText("STASH LOOT", { width - 230.f, 120.f }, 20);

    // BEFORE INSERTION STARTS
    if (!bstInsertStarted)
    {
        drawText("VALUE TO INSERT:", { width - 230.f, 175.f }, 15);

        drawPanel({ width - 230.f, 205.f }, { 190.f, 40.f });
        drawText(searchInput.empty() ? "ENTER VALUE..." : searchInput,
            { width - 220.f, 213.f }, 16);

        drawButton("START INSERTION",
            { width - 230.f, 265.f }, { 190.f, 40.f });
    }
    else
    {
        drawText("INSERTING: " + to_string(bstInsertValue),
            { width - 230.f, 175.f }, 15);

        drawText("CURRENT NODE:",
            { width - 230.f, 215.f }, 15);

        if (bstInsertCurrentNode != nullptr)
        {
            drawText(to_string(bstInsertCurrentNode->data),
                { width - 230.f, 240.f }, 23);
        }

        drawText("Choose direction:",
            { width - 230.f, 290.f }, 15);

        if (!bstInsertReadyToInsert)
        {
            drawButton("GO LEFT",
                { width - 230.f, 325.f }, { 190.f, 40.f });

            drawButton("GO RIGHT",
                { width - 230.f, 375.f }, { 190.f, 40.f });
        }
        else
        {
            drawText("Empty position found!",
                { width - 230.f, 325.f }, 14);

            drawButton("INSERT HERE",
                { width - 230.f, 375.f }, { 190.f, 40.f });
        }

        drawText("Comparisons: " +
            to_string(bstInsertComparisons),
            { width - 230.f, 445.f }, 15);
    }

    drawText(searchResult,
        { width - 230.f, 500.f }, 13);

    drawText("BST RULE:",
        { width - 230.f, 545.f }, 14);

    drawText("Smaller = LEFT",
        { width - 230.f, 570.f }, 13);

    drawText("Larger = RIGHT",
        { width - 230.f, 590.f }, 13);
}


void Game::drawBSTLevel3()
{
    float width = static_cast<float>(window.getSize().x);

    int target = 20;

    if (bstLeafDeleted)
        target = 30;

    if (bstOneChildDeleted)
        target = 50;

    drawText("STEAL AND COLLAPSE", { width - 230.f, 120.f }, 19);

    drawText("TARGET NODE:", { width - 230.f, 165.f }, 16);
    drawText(to_string(target), { width - 230.f, 190.f }, 23);

    drawText("1. Select the target", { width - 230.f, 230.f }, 15);

    if (bstDeleteSelectedNode == nullptr)
    {
        drawText("Click the target", { width - 230.f, 255.f }, 14);
        drawText("node in the tree.", { width - 230.f, 275.f }, 14);
    }
    else
    {
        drawText("Selected: " + to_string(bstDeleteSelectedNode->value),
            { width - 230.f, 255.f }, 15);

        drawText("2. Choose its case:", { width - 230.f, 290.f }, 15);

        drawButton("LEAF (0 CHILDREN)",
            { width - 230.f, 315.f }, { 190.f, 35.f });

        drawButton("ONE CHILD",
            { width - 230.f, 355.f }, { 190.f, 35.f });

        drawButton("TWO CHILDREN",
            { width - 230.f, 395.f }, { 190.f, 35.f });
    }

    if (bstDeleteAwaitingConfirmation)
    {
        drawText("Successor: " + to_string(bstDeleteSuccessor),
            { width - 230.f, 440.f }, 14);

        drawButton("CONFIRM DELETE",
            { width - 230.f, 475.f }, { 190.f, 35.f });
    }

    drawText(searchResult, { width - 230.f, 525.f }, 13);

    int completed = 0;

    if (bstLeafDeleted)
        completed++;

    if (bstOneChildDeleted)
        completed++;

    if (bstTwoChildrenDeleted)
        completed++;

    drawText("Missions: " + to_string(completed) + "/3",
        { width - 230.f, 555.f }, 15);
}

void Game::drawBSTLevel4()
{
    float width = static_cast<float>(window.getSize().x);

    drawText("RANGE RAID", { width - 230.f, 120.f }, 20);

    if (!rangeQueryStarted)
    {
        drawText("Enter lower limit:", { width - 230.f, 175.f }, 15);

        drawPanel({ width - 230.f, 200.f }, { 190.f, 35.f });
        drawText(rangeLowInput, { width - 220.f, 207.f }, 17);

        drawText("Enter upper limit:", { width - 230.f, 250.f }, 15);

        drawPanel({ width - 230.f, 280.f }, { 190.f, 35.f });
        drawText(rangeHighInput, { width - 220.f, 287.f }, 17);

        drawButton("START RAID",
            { width - 230.f, 335.f }, { 190.f, 40.f });
    }
    else
    {
        drawText("Range: [" + to_string(rangeLow) + ", " +
            to_string(rangeHigh) + "]",
            { width - 230.f, 170.f }, 16);

        if (!rangeQueryFinished && rangeCurrentNode != nullptr)
        {
            drawText("Current node:",
                { width - 230.f, 210.f }, 15);

            drawText(to_string(rangeCurrentNode->data),
                { width - 230.f, 235.f }, 22);

            drawText("Choose an action:",
                { width - 230.f, 280.f }, 15);

            drawButton("EXPLORE BOTH",
                { width - 230.f, 315.f }, { 190.f, 35.f });

            drawButton("SKIP LEFT",
                { width - 230.f, 355.f }, { 190.f, 35.f });

            drawButton("SKIP RIGHT",
                { width - 230.f, 395.f }, { 190.f, 35.f });
        }
        else if (rangeQueryFinished)
        {
            drawText("RAID COMPLETE!",
                { width - 230.f, 220.f }, 18);

            string values = "Values: ";

            for (int i = 0; i < rangeResultCount; i++)
            {
                values += to_string(rangeResults[i]);

                if (i < rangeResultCount - 1)
                    values += ", ";
            }

            if (rangeResultCount == 0)
                values += "None";

            drawText(values, { width - 230.f, 260.f }, 14);
        }

        drawText("Visited: " + to_string(rangeNodesVisited),
            { width - 230.f, 455.f }, 14);

        drawText("Pruned: " + to_string(rangeSubtreesPruned),
            { width - 230.f, 480.f }, 14);

        drawText("Correct: " + to_string(rangeCorrectDecisions),
            { width - 230.f, 505.f }, 14);

        drawText("Mistakes: " + to_string(rangeWrongDecisions),
            { width - 230.f, 530.f }, 14);
    }

    drawText(searchResult, { width - 230.f, 560.f }, 12);
}

void Game::drawBSTLevel5()
{
    float width = static_cast<float>(window.getSize().x);

    drawText("THE LAZY", { width - 230.f, 120.f }, 21);
    drawText("ARCHITECT", { width - 230.f, 145.f }, 21);

    drawText("INSERT IN ORDER", { width - 230.f, 200.f }, 16);

    RectangleShape inputBox({ 190.f, 40.f });
    inputBox.setPosition({ width - 230.f, 230.f });
    inputBox.setFillColor(Color(18, 22, 35));
    inputBox.setOutlineColor(searchInputActive ?
        Color(70, 180, 240) : Color(100, 120, 145));
    inputBox.setOutlineThickness(2.f);
    window.draw(inputBox);

    drawText(searchInput.empty() ? "Enter value..." : searchInput,
        { width - 220.f, 240.f }, 18);

    drawButton("INSERT VALUE",
        { width - 230.f, 290.f }, { 190.f, 40.f });

    drawText("Build a degenerate", { width - 230.f, 365.f }, 14);
    drawText("BST using sorted", { width - 230.f, 390.f }, 14);
    drawText("insertion order.", { width - 230.f, 415.f }, 14);

    drawText(searchResult, { width - 230.f, 465.f }, 13);
}

void Game::handleBSTLevel1Click(Vector2i mousePosition)
{
    float width = static_cast<float>(window.getSize().x);

    // Target input field
    if (isMouseOver(mousePosition, { width - 230.f, 205.f }, { 190.f, 40.f }))
    {
        searchInputActive = true;
    }

    // Start Search
    else if (isMouseOver(mousePosition, { width - 230.f, 260.f }, { 190.f, 38.f }))
    {
        searchInputActive = false;
        startBSTSearch();
    }

    // Go Left
    else if (isMouseOver(mousePosition, { width - 230.f, 310.f }, { 190.f, 38.f }))
    {
        searchInputActive = false;
        moveBSTLeft();
    }

    // Go Right
    else if (isMouseOver(mousePosition, { width - 230.f, 355.f }, { 190.f, 38.f }))
    {
        searchInputActive = false;
        moveBSTRight();
    }
}

void Game::handleBSTLevel2Click(Vector2i mousePosition)
{
    float width = static_cast<float>(window.getSize().x);

    // BEFORE INSERTION STARTS
    if (!bstInsertStarted)
    {
        if (isMouseOver(mousePosition,
            { width - 230.f, 205.f }, { 190.f, 40.f }))
        {
            searchInputActive = true;
        }
        else if (isMouseOver(mousePosition,
            { width - 230.f, 265.f }, { 190.f, 40.f }))
        {
            searchInputActive = false;
            startBSTInsertion();
        }

        return;
    }

    // AFTER INSERTION STARTS

    if (bstInsertReadyToInsert)
    {
        if (isMouseOver(mousePosition,
            { width - 230.f, 375.f }, { 190.f, 40.f }))
        {
            confirmBSTInsertion();
        }
    }
    else
    {
        if (isMouseOver(mousePosition,
            { width - 230.f, 325.f }, { 190.f, 40.f }))
        {
            moveBSTInsertLeft();
        }
        else if (isMouseOver(mousePosition,
            { width - 230.f, 375.f }, { 190.f, 40.f }))
        {
            moveBSTInsertRight();
        }
    }
}

void Game::handleBSTLevel3Click(Vector2i mousePosition)
{
    float width = static_cast<float>(window.getSize().x);

    // STEP 1: Select the target node from the tree.
    if (!bstDeleteAwaitingConfirmation &&
        isMouseOver(mousePosition, { 270.f, 100.f },
            { width - 540.f, 470.f }))
    {
        VisualNode* clickedNode = findClickedNode(
            visualRoot, mousePosition);

        if (clickedNode == nullptr)
            return;

        int target = 20;

        if (bstLeafDeleted)
            target = 30;

        if (bstOneChildDeleted)
            target = 50;

        if (clickedNode->value != target)
        {
            searchResult = "Select target " + to_string(target);
            bstDeleteSelectedNode = nullptr;
            return;
        }

        bstDeleteSelectedNode = clickedNode;
        bstDeleteCaseSelected = 0;
        searchResult = "Target selected. Choose its case.";
        return;
    }

    // Do not allow deletion if no target is selected.
    if (bstDeleteSelectedNode == nullptr)
        return;

    int leftChildren = (bstDeleteSelectedNode->left != nullptr) ? 1 : 0;
    int rightChildren = (bstDeleteSelectedNode->right != nullptr) ? 1 : 0;

    int actualCase = leftChildren + rightChildren;

    // STEP 2: Choose the deletion case.
    if (!bstDeleteAwaitingConfirmation)
    {
        if (isMouseOver(mousePosition,
            { width - 230.f, 315.f }, { 190.f, 35.f }))
        {
            bstDeleteCaseSelected = 1;
        }
        else if (isMouseOver(mousePosition,
            { width - 230.f, 355.f }, { 190.f, 35.f }))
        {
            bstDeleteCaseSelected = 2;
        }
        else if (isMouseOver(mousePosition,
            { width - 230.f, 395.f }, { 190.f, 35.f }))
        {
            bstDeleteCaseSelected = 3;
        }
        else
        {
            return;
        }

        if (bstDeleteCaseSelected != actualCase + 1)
        {
            searchResult = "Incorrect deletion case. Try again.";
            bstDeleteCaseSelected = 0;
            return;
        }

        // Correct case selected.
        bstDeleteAwaitingConfirmation = true;

        if (bstDeleteCaseSelected == 1)
        {
            searchResult = "Correct! This is a leaf node.";
        }
        else if (bstDeleteCaseSelected == 2)
        {
            searchResult = "Correct! This node has one child.";
        }
        else if (bstDeleteCaseSelected == 3)
        {
            // Find inorder successor: leftmost node
            // in the selected node's right subtree.
            VisualNode* successor = bstDeleteSelectedNode->right;

            while (successor->left != nullptr)
            {
                successor = successor->left;
            }

            bstDeleteSuccessor = successor->value;

            searchResult = "Correct! Find the successor.";
        }

        return;
    }

    // STEP 3: Confirm deletion.
    if (isMouseOver(mousePosition,
        { width - 230.f, 475.f }, { 190.f, 35.f }))
    {
        int deletedValue = bstDeleteSelectedNode->value;

        if (bst.deleteValue(deletedValue))
        {
            if (bstDeleteCaseSelected == 1)
            {
                bstLeafDeleted = true;
                searchResult = "Leaf node deleted!";
            }
            else if (bstDeleteCaseSelected == 2)
            {
                bstOneChildDeleted = true;
                searchResult = "One-child node deleted!";
            }
            else if (bstDeleteCaseSelected == 3)
            {
                bstTwoChildrenDeleted = true;

                searchResult = "Deleted! Successor was " +
                    to_string(bstDeleteSuccessor);
            }

            rebuildVisualTree();
            positionGameplayTree();

            bstDeleteSelectedNode = nullptr;
            bstDeleteCaseSelected = 0;
            bstDeleteAwaitingConfirmation = false;
            bstDeleteSuccessor = -1;

            bstLevelSuccess = bstLeafDeleted &&
                bstOneChildDeleted &&
                bstTwoChildrenDeleted;
        }
    }
}

void Game::handleBSTLevel4Click(Vector2i mousePosition)
{
    float width = static_cast<float>(window.getSize().x);

    // BEFORE STARTING THE RAID
    if (!rangeQueryStarted)
    {
        if (isMouseOver(mousePosition,
            { width - 230.f, 200.f }, { 190.f, 35.f }))
        {
            rangeLowInputActive = true;
            rangeHighInputActive = false;
        }
        else if (isMouseOver(mousePosition,
            { width - 230.f, 280.f }, { 190.f, 35.f }))
        {
            rangeHighInputActive = true;
            rangeLowInputActive = false;
        }
        else if (isMouseOver(mousePosition,
            { width - 230.f, 335.f }, { 190.f, 40.f }))
        {
            rangeLowInputActive = false;
            rangeHighInputActive = false;

            startRangeRaid();
        }

        return;
    }

    // AFTER STARTING THE RAID
    if (rangeQueryFinished)
        return;

    if (isMouseOver(mousePosition,
        { width - 230.f, 315.f }, { 190.f, 35.f }))
    {
        handleRangeDecision(3);
    }
    else if (isMouseOver(mousePosition,
        { width - 230.f, 355.f }, { 190.f, 35.f }))
    {
        handleRangeDecision(1);
    }
    else if (isMouseOver(mousePosition,
        { width - 230.f, 395.f }, { 190.f, 35.f }))
    {
        handleRangeDecision(2);
    }
}

void Game::handleBSTLevel5Click(Vector2i mousePosition)
{
    float width = static_cast<float>(window.getSize().x);

    // Value input field
    if (isMouseOver(mousePosition, { width - 230.f, 230.f }, { 190.f, 40.f }))
    {
        searchInputActive = true;
    }

    // Insert Value
    else if (isMouseOver(mousePosition, { width - 230.f, 290.f }, { 190.f, 40.f }))
    {
        searchInputActive = false;

        if (searchInput.empty())
        {
            searchResult = "Enter the next value.";
            return;
        }

        if (bstLevel5NextValue >= 5)
        {
            searchResult = "All five values inserted!";
            return;
        }

        int value = stoi(searchInput);
        int expected = bstLevel5Values[bstLevel5NextValue];

        if (value != expected)
        {
            searchResult = "Next required value: " + to_string(expected);
            searchInput = "";
            return;
        }

        bst.insert(value);

        bstLevel5NextValue++;

        rebuildVisualTree();
        positionGameplayTree();

        searchResult = "Inserted " + to_string(value);

        if (bstLevel5NextValue == 5)
        {
            bstLevelSuccess = true;
            searchResult += ". Degenerate BST completed!";
        }

        searchInput = "";
    }
}

VisualNode* Game::findClickedNode(VisualNode* node, Vector2i mousePosition)
{
    if (node == nullptr)
        return nullptr;

    float dx = node->position.x - mousePosition.x;
    float dy = node->position.y - mousePosition.y;

    // Check whether the mouse clicked this node.
    if (dx * dx + dy * dy <= 25.f * 25.f)
        return node;

    // Search the left subtree.
    VisualNode* found = findClickedNode(node->left, mousePosition);

    if (found != nullptr)
        return found;

    // Search the right subtree.
    return findClickedNode(node->right, mousePosition);
}

void Game::startBSTInsertion()
{
    if (searchInput.empty())
    {
        searchResult = "Enter a value first.";
        return;
    }

    bstInsertValue = stoi(searchInput);

    // Check for duplicates.
    int path[100];
    int pathLength = 0;

    if (bst.search(bstInsertValue, path, &pathLength))
    {
        searchResult = "Duplicates are not allowed!";
        searchInput = "";
        return;
    }

    // Reset insertion progress.
    bstInsertCurrentNode = bst.getRoot();
    bstInsertParentNode = nullptr;

    bstInsertComparisons = 0;
    bstInsertStarted = true;
    bstInsertReadyToInsert = false;
    bstInsertLeft = false;

    searchInput = "";

    searchResult = "Compare with the root. Choose LEFT or RIGHT.";
}

void Game::moveBSTInsertLeft()
{
    if (!bstInsertStarted || bstInsertReadyToInsert ||
        bstInsertCurrentNode == nullptr)
    {
        return;
    }

    // The value must be smaller to go left.
    if (bstInsertValue >= bstInsertCurrentNode->data)
    {
        searchResult = "Wrong direction! Choose RIGHT.";
        return;
    }

    bstInsertComparisons++;

    bstInsertParentNode = bstInsertCurrentNode;
    bstInsertLeft = true;

    if (bstInsertCurrentNode->left == nullptr)
    {
        bstInsertReadyToInsert = true;
        searchResult = "Correct! Empty left position found.";
    }
    else
    {
        bstInsertCurrentNode = bstInsertCurrentNode->left;
        searchResult = "Correct! Now compare with " +
            to_string(bstInsertCurrentNode->data);
    }
}

void Game::moveBSTInsertRight()
{
    if (!bstInsertStarted || bstInsertReadyToInsert ||
        bstInsertCurrentNode == nullptr)
    {
        return;
    }

    // The value must be greater to go right.
    if (bstInsertValue <= bstInsertCurrentNode->data)
    {
        searchResult = "Wrong direction! Choose LEFT.";
        return;
    }

    bstInsertComparisons++;

    bstInsertParentNode = bstInsertCurrentNode;
    bstInsertLeft = false;

    if (bstInsertCurrentNode->right == nullptr)
    {
        bstInsertReadyToInsert = true;
        searchResult = "Correct! Empty right position found.";
    }
    else
    {
        bstInsertCurrentNode = bstInsertCurrentNode->right;
        searchResult = "Correct! Now compare with " +
            to_string(bstInsertCurrentNode->data);
    }
}

void Game::confirmBSTInsertion()
{
    if (!bstInsertStarted || !bstInsertReadyToInsert ||
        bstInsertParentNode == nullptr)
    {
        return;
    }

    Node* newNode = new Node(bstInsertValue);

    if (bstInsertLeft)
    {
        bstInsertParentNode->left = newNode;
    }
    else
    {
        bstInsertParentNode->right = newNode;
    }

    rebuildVisualTree();
    positionGameplayTree();

    searchResult = "Successfully inserted " +
        to_string(bstInsertValue) + "!";

    bstLevelSuccess = true;

    bstInsertStarted = false;
    bstInsertReadyToInsert = false;
    bstInsertCurrentNode = nullptr;
    bstInsertParentNode = nullptr;
}

void Game::resetBSTInsertion()
{
    searchInput = "";
    searchResult = "";

    searchInputActive = false;

    bstInsertCurrentNode = nullptr;
    bstInsertParentNode = nullptr;

    bstInsertValue = -1;
    bstInsertComparisons = 0;

    bstInsertStarted = false;
    bstInsertReadyToInsert = false;
    bstInsertLeft = false;

    bstLevelSuccess = false;
}

