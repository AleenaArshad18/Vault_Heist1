#include "Game.h"

#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <functional>

using namespace sf;
using namespace std;

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
        if (event->is<Event::Closed>())
        {
            window.close();
        }

        if (event->is<sf::Event::TextEntered>())
        {
            if (heapInputActive)
            {
                auto textEvent = event->getIf<sf::Event::TextEntered>();

                if (textEvent->unicode >= '0' && textEvent->unicode <= '9')
                {
                    heapInput += static_cast<char>(textEvent->unicode);
                }
                else if (textEvent->unicode == 8 && !heapInput.empty())
                {
                    heapInput.pop_back();
                }
            }
        }

        if (const auto* resized = event->getIf<Event::Resized>())
        {
            float width = static_cast<float>(resized->size.x);
            float height = static_cast<float>(resized->size.y);

            window.setView(View(FloatRect({ 0.f, 0.f }, { width, height })));


            if (currentScreen == GAMEPLAY)
            {
                if (selectedAct == 1)
                    rebuildBinaryVisualTree();
                else if (selectedAct == 2)
                    positionGameplayTree();
                else if (selectedAct == 3)
                    rebuildAVLVisualTree();
                else if (selectedAct == 4)
                    rebuildHeapVisualTree();
            }
            else
            {
                layoutEngine.calculatePositions(visualRoot, width);
            }
        }

        if (const auto* mouse = event->getIf<Event::MouseButtonPressed>())
        {
            if (mouse->button == Mouse::Button::Left)
            {
                handleMouseClick(mouse->position);
            }
        }

        // Handle keyboard input while search field is active
        if (currentScreen == GAMEPLAY && searchInputActive)
        {
            if (const auto* key = event->getIf<Event::KeyPressed>())
            {
                if (key->code == Keyboard::Key::Backspace)
                {
                    if (!searchInput.empty())
                    {
                        searchInput.pop_back();
                    }
                }
                else if (key->code == Keyboard::Key::Enter)
                {
                    if (selectedAct == 2)
                    {
                        performSearch();
                    }
                }
            }

            if (const auto* text = event->getIf<Event::TextEntered>())
            {
                if (text->unicode >= '0' && text->unicode <= '9')
                {
                    if (searchInput.length() < 9)
                    {
                        searchInput += static_cast<char>(text->unicode);
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
        if (isMouseOver(mousePosition,
            { centerX - 150.f, 210.f }, { 300.f, 60.f }))
        { // act 1
            selectedAct = 1;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition,
            { centerX - 150.f, 290.f }, { 300.f, 60.f }))
        { //act 2 
            selectedAct = 2;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition,
            { centerX - 150.f, 370.f }, { 300.f, 60.f }))
        { //act  3
            selectedAct = 3;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition,
            { centerX - 150.f, 450.f }, { 300.f, 60.f }))
        { // act 4
            selectedAct = 4;
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition,
            { 30.f, 600.f }, { 150.f, 50.f }))
        { // back button
            currentScreen = MAIN_MENU;
        }
    }

    else if (currentScreen == LEVEL_SELECT)
    {
        if (isMouseOver(mousePosition, { centerX - 180.f, 230.f }, { 110.f, 65.f }))
        {
            selectedLevel = 1;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX - 55.f, 230.f }, { 110.f, 65.f }))
        {
            selectedLevel = 2;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX + 70.f, 230.f }, { 110.f, 65.f }))
        {
            selectedLevel = 3;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX - 180.f, 320.f }, { 110.f, 65.f }))
        {
            selectedLevel = 4;
            currentScreen = LEVEL_INTRO;
        }
        else if (isMouseOver(mousePosition, { centerX - 55.f, 320.f }, { 110.f, 65.f }))
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
            // Search input field
            if (isMouseOver(mousePosition, { width - 230.f, 205.f }, { 190.f, 40.f }))
            {
                searchInputActive = true;
            }

            // Search button
            else if (isMouseOver(mousePosition, { width - 230.f, 260.f }, { 190.f, 38.f }))
            {
                searchInputActive = false;
                performSearch();
            }

            // Insert button
            else if (isMouseOver(mousePosition, { width - 230.f, 305.f }, { 190.f, 38.f }))
            {
                searchInputActive = false;
                performInsert();
            }

            // Delete button
            else if (isMouseOver(mousePosition, { width - 230.f, 350.f }, { 190.f, 38.f }))
            {
                searchInputActive = false;
                performDelete();
            }

            // Minimum button
            else if (isMouseOver(mousePosition, { width - 230.f, 395.f }, { 190.f, 38.f }))
            {
                searchInputActive = false;
                performMinimum();
            }

            // Maximum button
            else if (isMouseOver(mousePosition, { width - 230.f, 440.f }, { 190.f, 38.f }))
            {
                searchInputActive = false;
                performMaximum();
            }

            // Successor
            else if (isMouseOver(mousePosition, { width - 230.f, 450.f }, { 190.f, 34.f }))
            {
                searchInputActive = false;
                performSuccessor();
            }

            // Predecessor
            else if (isMouseOver(mousePosition, { width - 230.f, 488.f }, { 190.f, 34.f }))
            {
                searchInputActive = false;
                performPredecessor();
            }

            // Back button
            else if (isMouseOver(mousePosition, { 30.f, 620.f }, { 150.f, 50.f }))
            {
                searchInputActive = false;
                currentScreen = LEVEL_INTRO;
            }

            // Reset button
            else if (isMouseOver(mousePosition, { 600.f, 620.f }, { 150.f, 50.f }))
            {
                searchInput = "";
                searchResult = "";
                searchInputActive = false;
                clearHighlights(visualRoot);
            }
            else
            {
                searchInputActive = false;
            }
        }

        else if (selectedAct == 1)
        {
            // ACT 1 - FIVE BINARY TREE CHALLENGES
            if (selectedLevel == 1)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                    searchResult = "Correct! Preorder: 1 - 2 - 4 - 5 - 3";
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                    searchResult = "Incorrect. Preorder visits Root, Left, Right.";
                else if (isMouseOver(mousePosition, { width - 230.f, 290.f }, { 190.f, 38.f }))
                    searchResult = "Incorrect. Try Root-Left-Right.";
                else if (isMouseOver(mousePosition, { width - 230.f, 340.f }, { 190.f, 38.f }))
                    searchResult = "Incorrect. Try Root-Left-Right.";
            }
            else if (selectedLevel == 2)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    int pre[] = { 1, 2, 4, 5, 3 };
                    int in[] = { 4, 2, 5, 1, 3 };
                    binaryTree.buildFromPreorderInorder(pre, in, 5);
                    rebuildBinaryVisualTree();
                    searchResult = "Reconstructed from Preorder + Inorder.";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    int post[] = { 4, 5, 2, 3, 1 };
                    int in[] = { 4, 2, 5, 1, 3 };
                    binaryTree.buildFromPostorderInorder(post, in, 5);
                    rebuildBinaryVisualTree();
                    searchResult = "Reconstructed from Postorder + Inorder.";
                }
            }
            else if (selectedLevel == 3)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                    searchResult = "Height = " + to_string(binaryTree.height()) + " levels.";
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                    searchResult = "Diameter = " + to_string(binaryTree.diameter()) + " nodes.";
            }
            else if (selectedLevel == 4)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                    searchResult = "LCA(4, 5) = " + to_string(binaryTree.LCA(4, 5));
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                {
                    int path[100], length = 0;
                    if (binaryTree.pathTo(4, path, length))
                    {
                        searchResult = "Path to 4: ";
                        for (int i = 0; i < length; ++i)
                        {
                            if (i) searchResult += " -> ";
                            searchResult += to_string(path[i]);
                        }
                    }
                    else searchResult = "Node not found.";
                }
            }
            else if (selectedLevel == 5)
            {
                if (isMouseOver(mousePosition, { width - 230.f, 190.f }, { 190.f, 38.f }))
                {
                    binaryTree.mirror();
                    rebuildBinaryVisualTree();
                    searchResult = "Tree mirrored successfully.";
                }
                else if (isMouseOver(mousePosition, { width - 230.f, 240.f }, { 190.f, 38.f }))
                    searchResult = string("Symmetric? ") + (binaryTree.isSymmetric() ? "Yes" : "No");
            }

            if (isMouseOver(mousePosition, { 30.f, 620.f }, { 150.f, 50.f }))
            {
                searchResult = "";
                currentScreen = LEVEL_INTRO;
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
    drawCenteredText("Choose an act to continue", 150.f, 22);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    drawButton("ACT 1 - BINARY TREE",
        { centerX - 150.f, 210.f }, { 300.f, 60.f });

    drawButton("ACT 2 - BST",
        { centerX - 150.f, 290.f }, { 300.f, 60.f });

    drawButton("ACT 3 - AVL TREE",
        { centerX - 150.f, 370.f }, { 300.f, 60.f });

    drawButton("ACT 4 - HEAP FORTRESS",
        { centerX - 150.f, 450.f }, { 300.f, 60.f });

    drawButton("BACK", { 30.f, 600.f }, { 150.f, 50.f });
}

// Draw level selection
void Game::drawLevelSelect()
{
    if (selectedAct == 1)
    {
        drawCenteredText("ACT 1 - BINARY TREE", 100.f, 40);
    }
    else if (selectedAct == 3)
    {
        drawCenteredText("ACT 3 - AVL TREE", 100.f, 40);
    }
    if (selectedAct == 2)
    {
        drawCenteredText("ACT 2 - SORTED WING", 100.f, 40);
    }
    else if (selectedAct == 4)
    {
        drawCenteredText("ACT 4 - HEAP FORTRESS", 100.f, 40);
    }

    drawCenteredText("Choose a level", 180.f, 22);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    drawButton("LEVEL 1", { centerX - 180.f, 230.f }, { 110.f, 65.f });
    drawButton("LEVEL 2", { centerX - 55.f, 230.f }, { 110.f, 65.f });
    drawButton("LEVEL 3", { centerX + 70.f, 230.f }, { 110.f, 65.f });

    drawButton("LEVEL 4", { centerX - 180.f, 320.f }, { 110.f, 65.f });
    drawButton("LEVEL 5", { centerX - 55.f, 320.f }, { 110.f, 65.f });

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


// Draw gameplay screen
void Game::drawGameplay()
{
    float width = static_cast<float>(window.getSize().x);

    // Top HUD
    drawPanel({ 20.f, 15.f }, { width - 40.f, 65.f });

    drawText("LEVEL " + to_string(selectedLevel), { 40.f, 35.f }, 22);
    drawText("BST ACT", { width - 180.f, 35.f }, 22);

    // Mission panel
    drawPanel({ 20.f, 100.f }, { 230.f, 470.f });

    drawText("MISSION", { 40.f, 120.f }, 25);

    //drawText("EVENT LOG", { width - 230.f, 430.f }, 20);

    //drawText(searchResult, { width - 230.f, 465.f }, 15);

    if (selectedLevel == 1)
    {
        drawText("Find the target", { 40.f, 175.f }, 18);
        drawText("value in the BST.", { 40.f, 205.f }, 18);
        drawText("Search any value", { 40.f, 270.f }, 16);
    }
    else if (selectedLevel == 2)
    {
        drawText("Insert a value", { 40.f, 175.f }, 18);
        drawText("into the BST.", { 40.f, 205.f }, 18);
    }
    else if (selectedLevel == 3)
    {
        drawText("Delete a value", { 40.f, 175.f }, 18);
        drawText("from the BST.", { 40.f, 205.f }, 18);
    }
    else if (selectedLevel == 4)
    {
        drawText("Follow the", { 40.f, 175.f }, 18);
        drawText("traversal order.", { 40.f, 205.f }, 18);
    }
    else
    {
        drawText("Complete the", { 40.f, 175.f }, 18);
        drawText("BST challenge.", { 40.f, 205.f }, 18);
    }

    // Tree visualization panel
    drawPanel({ 270.f, 100.f }, { width - 540.f, 470.f });

    drawText("TREE VISUALIZATION", { 290.f, 115.f }, 20);

    renderer.drawTree(window, visualRoot);

    // Operations panel
    drawPanel({ width - 250.f, 100.f }, { 230.f, 470.f });

    drawText("OPERATIONS", { width - 230.f, 120.f }, 23);

    drawText("SEARCH VALUE", { width - 230.f, 175.f }, 17);

    // Search input field
    RectangleShape inputBox({ 190.f, 40.f });
    inputBox.setPosition({ width - 230.f, 205.f });
    inputBox.setFillColor(Color(18, 22, 35));

    if (searchInputActive)
    {
        inputBox.setOutlineColor(Color(70, 180, 240));
    }
    else
    {
        inputBox.setOutlineColor(Color(100, 120, 145));
    }

    inputBox.setOutlineThickness(2.f);
    window.draw(inputBox);

    if (searchInput.empty())
    {
        drawText("Enter value...", { width - 220.f, 215.f }, 15);
    }
    else
    {
        drawText(searchInput, { width - 220.f, 215.f }, 18);
    }

    drawButton("SEARCH", { width - 230.f, 260.f }, { 190.f, 34.f });
    drawButton("INSERT", { width - 230.f, 298.f }, { 190.f, 34.f });
    drawButton("DELETE", { width - 230.f, 336.f }, { 190.f, 34.f });
    drawButton("MINIMUM", { width - 230.f, 374.f }, { 190.f, 34.f });
    drawButton("MAXIMUM", { width - 230.f, 412.f }, { 190.f, 34.f });
    drawButton("SUCCESSOR", { width - 230.f, 450.f }, { 190.f, 34.f });
    drawButton("PREDECESSOR", { width - 230.f, 488.f }, { 190.f, 34.f });

    drawText("EVENT LOG", { width - 230.f, 530.f }, 16);

    drawText(searchResult, { width - 230.f, 552.f }, 13);

    if (searchResult.empty())
    {
        drawText("Waiting for action...", { width - 230.f, 430.f }, 14);
    }
    else
    {
        drawText(searchResult, { width - 230.f, 430.f }, 14);
    }

    // Bottom toolbar
    drawButton("BACK", { 30.f, 620.f }, { 150.f, 50.f });
    drawButton("UNDO", { 220.f, 620.f }, { 150.f, 50.f });
    drawButton("HINT", { 410.f, 620.f }, { 150.f, 50.f });
    drawButton("RESET", { 600.f, 620.f }, { 150.f, 50.f });
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
