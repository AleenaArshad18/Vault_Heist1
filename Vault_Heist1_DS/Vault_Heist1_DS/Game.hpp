
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "VisualNode.hpp"
#include "LayoutEngine.hpp"
#include "Renderer.hpp"

using namespace sf;

class Game
{
public:
    Game();
    void run();

private:
    enum class Screen {
        MainMenu,
        CampaignMap,
        LevelSelect,
        LevelIntro,
        Gameplay,
        Sandbox,
        Leaderboard
    };

    void processEvents();
    void update();
    void render();

    void handleMouseClick(Vector2i mousePosition);

    void drawMainMenu();
    void drawCampaignMap();
    void drawLevelSelect();
    void drawPlaceholder(const std::string& title, const std::string& message);
    void drawLevelIntro();
    void drawGameplay();
    void drawButton(const std::string& text, Vector2f position, Vector2f size, bool enabled = true);
    void drawCenteredText(const std::string& text, float y, unsigned int fontSize);

    bool isMouseOver(Vector2i mousePosition, Vector2f position, Vector2f size);

    RenderWindow window;

    LayoutEngine layoutEngine;
    Renderer renderer;

    Font uiFont;
    bool uiFontLoaded = false;

    Screen currentScreen = Screen::MainMenu;
    int selectedLevel = 0;


    VisualNode root{ 50 };
    VisualNode node30{ 30 };
    VisualNode node70{ 70 };
    VisualNode node20{ 20 };
    VisualNode node40{ 40 };
    VisualNode node60{ 60 };
    VisualNode node80{ 80 };
};
