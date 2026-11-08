#include "Game.h"

#include <iostream>
#include <string>

using namespace sf;
using namespace std;


// Constructor
Game::Game()
{
    window.create(VideoMode({ 1000u, 700u }), "Vault Heist: The Living Vault");
    window.setFramerateLimit(60);

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

    // Create visual copy of the BST
    rebuildVisualTree();

    positionGameplayTree();
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


// Process window events
void Game::processEvents()
{
    while (auto event = window.pollEvent())
    {
        if (event->is<Event::Closed>())
        {
            window.close();
        }

        if (const auto* resized = event->getIf<Event::Resized>())
        {
            float width = static_cast<float>(resized->size.x);
            float height = static_cast<float>(resized->size.y);

            window.setView(View(FloatRect({ 0.f, 0.f }, { width, height })));

            if (currentScreen == GAMEPLAY)
            {
                positionGameplayTree();
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
                    performSearch();
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
    else if (currentScreen == GAMEPLAY)//
    {
        drawGameplay();
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
        if (isMouseOver(mousePosition, { centerX - 150.f, 300.f }, { 300.f, 65.f }))
        {
            currentScreen = LEVEL_SELECT;
        }
        else if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
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
        if (isMouseOver(mousePosition, { centerX - 150.f, 450.f }, { 300.f, 60.f }))
        {
            currentScreen = GAMEPLAY;
            searchInputActive = false;
            searchInput = "";
            searchResult = "";
            clearHighlights(visualRoot);
            positionGameplayTree();
        }
        else if (isMouseOver(mousePosition, { 30.f, 600.f }, { 150.f, 50.f }))
        {
            currentScreen = LEVEL_SELECT;
        }
    }

    else if (currentScreen == GAMEPLAY)
    {
        // Search input field
        if (isMouseOver(mousePosition, { width - 230.f, 205.f }, { 190.f, 40.f }))
        {
            searchInputActive = true;
        }

        // Search button
        else if (isMouseOver(mousePosition, { width - 230.f, 260.f }, { 190.f, 45.f }))
        {
            searchInputActive = false;
            performSearch();
        }

        // Insert button
        else if (isMouseOver(mousePosition, { width - 230.f, 315.f }, { 190.f, 45.f }))
        {
            searchInputActive = false;
            performInsert();
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
    drawCenteredText("CAMPAIGN MAP", 100.f, 45);
    drawCenteredText("Choose an act to continue", 180.f, 22);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

    drawButton("ACT 2 - BST", { centerX - 150.f, 300.f }, { 300.f, 65.f });
    drawButton("BACK", { 30.f, 600.f }, { 150.f, 50.f });
}


// Draw level selection
void Game::drawLevelSelect()
{
    drawCenteredText("BINARY SEARCH TREE", 90.f, 40);
    drawCenteredText("SELECT A LEVEL", 155.f, 25);

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
    drawCenteredText("LEVEL INTRODUCTION", 80.f, 40);

    string mission;
    string description;

    if (selectedLevel == 1)
    {
        mission = "THE FIRST SEARCH";
        description = "Find the target value in the BST.";
    }
    else if (selectedLevel == 2)
    {
        mission = "INSERTION";
        description = "Insert the required value into the BST.";
    }
    else if (selectedLevel == 3)
    {
        mission = "DELETION";
        description = "Remove the required value from the BST.";
    }
    else if (selectedLevel == 4)
    {
        mission = "TREE TRAVERSAL";
        description = "Follow the correct traversal order.";
    }
    else if (selectedLevel == 5)
    {
        mission = "BST CHALLENGE";
        description = "Complete the binary search tree challenge.";
    }
    else
    {
        mission = "UNKNOWN LEVEL";
        description = "No level information available.";
    }

    drawCenteredText("LEVEL " + to_string(selectedLevel), 180.f, 30);
    drawCenteredText(mission, 250.f, 28);
    drawCenteredText(description, 320.f, 21);

    float centerX = static_cast<float>(window.getSize().x) / 2.f;

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

    drawButton("SEARCH", { width - 230.f, 260.f }, { 190.f, 45.f });
    drawButton("INSERT", { width - 230.f, 315.f }, { 190.f, 45.f });

    // Event log
    drawText("EVENT LOG", { width - 230.f, 390.f }, 21);

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