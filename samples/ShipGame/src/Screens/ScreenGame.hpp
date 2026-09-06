// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenGame.hpp — C++ port of ShipGame/Screens/ScreenGame.cs (XNA 4.0 Ship
// Game Starter Kit). The screen that is the game itself.

#include "Screen.hpp"

namespace ShipGame {

class GameManager;
class ScreenManager;

// Port of ShipGame/Screens/ScreenGame.cs.
class ScreenGame : public Screen {
public:
    // constructor
    ScreenGame(ScreenManager* manager, GameManager* game)
        : screenManager_(manager), gameManager_(game) {}

    // called before screen shows
    void SetFocus(ContentManager* content, bool focus) override;

    // process input
    void ProcessInput(float elapsedTime, InputManager* input) override;

    // update screen
    void Update(float elapsedTime) override;

    // draw 3D scene
    void Draw3D(GraphicsDevice* gd) override;

    // draw 2D gui
    void Draw2D(GraphicsDevice* gd, FontManager* font) override;

private:
    ScreenManager* screenManager_ = nullptr; // screen manager
    GameManager* gameManager_ = nullptr;     // game manager
};

} // namespace ShipGame
