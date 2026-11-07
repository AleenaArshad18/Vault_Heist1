#include "Game.hpp"
#include <optional>
#include <iostream>

using namespace sf;
using namespace std;

Game::Game()
    : window(VideoMode({ 1000u, 700u }), "Vault Heist: The Living Vault")
{
    window.setFramerateLimit(60);

    uiFontLoaded = uiFont.openFromFile("assets/fonts/BrownieStencil.ttf");

    if (!uiFontLoaded) {
        std::cout << "Error: Could not load game font." << std::endl;
    }

    // Create the sample binary tree
    root.left = &node30;
    root.right = &node70;

    node30.left = &node20;
    node30.right = &node40;

    node70.left = &node60;
    node70.right = &node80;

    layoutEngine.calculatePositions(&root, static_cast<float>(window.getSize().x));
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {

        if (event->is<Event::Closed>()) {
            window.close();
        }

        if (const auto* mouse = event->getIf<Event::MouseButtonPressed>()) {
            if (mouse->button == Mouse::Button::Left) {
                handleMouseClick(mouse->position);
            }
        }

        if (const auto* resized = event->getIf<Event::Resized>()) {
            float width = static_cast<float>(resized->size.x);
            float height = static_cast<float>(resized->size.y);

            window.setView(View(FloatRect({ 0.f, 0.f }, { width, height })));
            layoutEngine.calculatePositions(&root, width);
        }
    }
}

void Game::update() {
    // Game logic will be added here later.
}

void Game::render() {
    window.clear(Color(20, 25, 40));

    switch (currentScreen) {

    case Screen::MainMenu:
        drawMainMenu();
        break;

    case Screen::CampaignMap:
        drawCampaignMap();
        break;

    case Screen::LevelSelect:
        drawLevelSelect();
        break;

    case Screen::LevelIntro:
        drawLevelIntro();
        break;

    case Screen::Gameplay:
        drawGameplay();
        break;

    case Screen::Sandbox:
        drawPlaceholder("SANDBOX", "Sandbox mode will be implemented later.");
        break;

    case Screen::Leaderboard:
        drawPlaceholder("LEADERBOARD", "Your scores will appear here.");
        break;
    }

    window.display();
}

void Game::drawCenteredText(const std::string& text, float y, unsigned int fontSize) {
    if (!uiFontLoaded)
        return;

    Text label(uiFont, text, fontSize);
    label.setFillColor(Color::White);

    FloatRect bounds = label.getLocalBounds();

    label.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
        });

    label.setPosition({
        static_cast<float>(window.getSize().x) / 2.f,
        y
        });

    window.draw(label);
}

void Game::drawButton(const std::string& text, Vector2f position, Vector2f size, bool enabled) {
    RectangleShape button(size);

    button.setPosition(position);
    button.setFillColor(enabled ? Color(45, 100, 160) : Color(65, 65, 75));
    button.setOutlineThickness(2.f);
    button.setOutlineColor(Color(180, 190, 210));

    window.draw(button);

    if (uiFontLoaded) {
        Text label(uiFont, text, 22);
        label.setFillColor(enabled ? Color::White : Color(150, 150, 150));

        FloatRect bounds = label.getLocalBounds();

        label.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });

        label.setPosition({
            position.x + size.x / 2.f,
            position.y + size.y / 2.f
            });

        window.draw(label);
    }
}

bool Game::isMouseOver(Vector2i mousePosition, Vector2f position, Vector2f size) {
    return mousePosition.x >= position.x &&
        mousePosition.x <= position.x + size.x &&
        mousePosition.y >= position.y &&
        mousePosition.y <= position.y + size.y;
}

// MAIN MENU

void Game::drawMainMenu() {
    float centerX = window.getSize().x / 2.f;

    drawCenteredText("VAULT HEIST", 150.f, 48);
    drawCenteredText("THE LIVING VAULT", 205.f, 25);

    drawButton("PLAY", { centerX - 150.f, 300.f }, { 300.f, 55.f });
    drawButton("SANDBOX", { centerX - 150.f, 375.f }, { 300.f, 55.f });
    drawButton("LEADERBOARD", { centerX - 150.f, 450.f }, { 300.f, 55.f });
    drawButton("EXIT", { centerX - 150.f, 525.f }, { 300.f, 55.f });
}

// CAMPAIGN MAP

void Game::drawCampaignMap() {
    float centerX = window.getSize().x / 2.f;

    drawCenteredText("CAMPAIGN MAP", 100.f, 40);

    drawButton("ACT 1 - BINARY TREE", { centerX - 200.f, 200.f }, { 400.f, 55.f }, false);
    drawButton("ACT 2 - BST", { centerX - 200.f, 275.f }, { 400.f, 55.f });
    drawButton("ACT 3 - AVL TREE", { centerX - 200.f, 350.f }, { 400.f, 55.f }, false);
    drawButton("ACT 4 - MAX HEAP", { centerX - 200.f, 425.f }, { 400.f, 55.f }, false);

    drawButton("BACK", { centerX - 100.f, 550.f }, { 200.f, 50.f });
}

// LEVEL SELECT

void Game::drawLevelSelect() {
    float centerX = window.getSize().x / 2.f;

    drawCenteredText("ACT 2 - BST", 100.f, 40);
    drawCenteredText("SELECT A LEVEL", 155.f, 22);

    drawButton("LEVEL 1 - SEARCH", { centerX - 180.f, 210.f }, { 360.f, 50.f });
    drawButton("LEVEL 2 - INSERTION", { centerX - 180.f, 275.f }, { 360.f, 50.f });
    drawButton("LEVEL 3 - DELETION", { centerX - 180.f, 340.f }, { 360.f, 50.f });
    drawButton("LEVEL 4 - RANGE QUERY", { centerX - 180.f, 405.f }, { 360.f, 50.f });
    drawButton("LEVEL 5 - BOSS", { centerX - 180.f, 470.f }, { 360.f, 50.f });

    drawButton("BACK", { centerX - 100.f, 565.f }, { 200.f, 45.f });
}

// PLACEHOLDER SCREENS

void Game::drawPlaceholder(const std::string& title, const std::string& message) {
    float centerX = window.getSize().x / 2.f;

    drawCenteredText(title, 200.f, 40);
    drawCenteredText(message, 300.f, 20);

    drawButton("BACK", { centerX - 100.f, 450.f }, { 200.f, 55.f });
}

// BUTTON CLICK HANDLING

void Game::handleMouseClick(Vector2i mousePosition) {
    float centerX = window.getSize().x / 2.f;

    if (currentScreen == Screen::MainMenu) {

        if (isMouseOver(mousePosition, { centerX - 150.f, 300.f }, { 300.f, 55.f })) {
            currentScreen = Screen::CampaignMap;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 375.f }, { 300.f, 55.f })) {
            currentScreen = Screen::Sandbox;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 450.f }, { 300.f, 55.f })) {
            currentScreen = Screen::Leaderboard;
        }
        else if (isMouseOver(mousePosition, { centerX - 150.f, 525.f }, { 300.f, 55.f })) {
            window.close();
        }
    }

    else if (currentScreen == Screen::CampaignMap) {

        if (isMouseOver(mousePosition, { centerX - 200.f, 275.f }, { 400.f, 55.f })) {
            currentScreen = Screen::LevelSelect;
        }
        else if (isMouseOver(mousePosition, { centerX - 100.f, 550.f }, { 200.f, 50.f })) {
            currentScreen = Screen::MainMenu;
        }
    }

    else if (currentScreen == Screen::LevelSelect) {

        for (int i = 0; i < 5; i++) {
            float y = 210.f + i * 65.f;

            if (isMouseOver(mousePosition, { centerX - 180.f, y }, { 360.f, 50.f })) {
                selectedLevel = i + 1;
                currentScreen = Screen::LevelIntro;
                break;
            }
        }

        if (isMouseOver(mousePosition, { centerX - 100.f, 565.f }, { 200.f, 45.f })) {
            currentScreen = Screen::CampaignMap;
        }
    }

    else if (currentScreen == Screen::LevelIntro) {

        if (isMouseOver(mousePosition, { centerX - 150.f, 500.f }, { 300.f, 55.f })) {
            currentScreen = Screen::Gameplay;
        }
        else if (isMouseOver(mousePosition, { centerX - 100.f, 580.f }, { 200.f, 45.f })) {
            currentScreen = Screen::LevelSelect;
        }
    }

    else if (currentScreen == Screen::Gameplay) {

        if (isMouseOver(mousePosition, { centerX - 100.f, 620.f }, { 200.f, 45.f })) {
            currentScreen = Screen::LevelIntro;
        }
    }

    else if (currentScreen == Screen::Sandbox || currentScreen == Screen::Leaderboard) {

        if (isMouseOver(mousePosition, { centerX - 100.f, 450.f }, { 200.f, 55.f })) {
            currentScreen = Screen::MainMenu;
        }
    }

}

void Game::drawLevelIntro() {
    std::string levelName;
    std::string story;
    std::string objective;
    std::string instructions;

    switch (selectedLevel) {

    case 1:
        levelName = "BST SEARCH";
        story = "A hidden artifact is locked inside the vault.";
        objective = "Objective: Find the required value in the BST.";
        instructions = "Follow the tree and choose the correct path.";
        break;

    case 2:
        levelName = "BST INSERTION";
        story = "A new artifact needs to be added to the vault.";
        objective = "Objective: Insert a new value into the BST.";
        instructions = "Find the correct position while maintaining BST order.";
        break;

    case 3:
        levelName = "BST DELETION";
        story = "An outdated artifact must be removed.";
        objective = "Objective: Delete the specified value.";
        instructions = "Maintain the BST structure after deletion.";
        break;

    case 4:
        levelName = "BST RANGE QUERY";
        story = "The vault contains artifacts with different values.";
        objective = "Objective: Find values within the given range.";
        instructions = "Identify all nodes that satisfy the range.";
        break;

    case 5:
        levelName = "BST BOSS LEVEL";
        story = "The final vault challenge is waiting.";
        objective = "Objective: Complete the BST challenge.";
        instructions = "Use your BST knowledge to solve the challenge.";
        break;

    default:
        levelName = "UNKNOWN LEVEL";
        story = "";
        objective = "";
        instructions = "";
        break;
    }

    drawCenteredText("LEVEL " + std::to_string(selectedLevel), 75.f, 25);
    drawCenteredText(levelName, 135.f, 40);

    drawCenteredText("MISSION BRIEFING", 235.f, 25);
    drawCenteredText(story, 290.f, 19);

    drawCenteredText(objective, 370.f, 19);
    drawCenteredText(instructions, 420.f, 18);

    float centerX = window.getSize().x / 2.f;

    drawButton("START LEVEL", { centerX - 150.f, 500.f }, { 300.f, 55.f });
    drawButton("BACK", { centerX - 100.f, 580.f }, { 200.f, 45.f });
}

void Game::drawGameplay() {
    drawCenteredText("BST LEVEL " + std::to_string(selectedLevel), 40.f, 30);

    drawCenteredText("ALARM: 0", 100.f, 20);
    drawCenteredText("SCORE: 1000", 140.f, 20);

    renderer.drawTree(window, &root);

    float centerX = window.getSize().x / 2.f;

    drawButton("UNDO", { centerX - 300.f, 550.f }, { 130.f, 45.f });
    drawButton("RESET", { centerX - 65.f, 550.f }, { 130.f, 45.f });
    drawButton("HINT", { centerX + 170.f, 550.f }, { 130.f, 45.f });

    drawButton("BACK", { centerX - 100.f, 620.f }, { 200.f, 45.f });
}