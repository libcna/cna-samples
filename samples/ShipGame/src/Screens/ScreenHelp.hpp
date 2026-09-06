// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenHelp.hpp — C++ port of ShipGame/Screens/ScreenHelp.cs (XNA 4.0 Ship
// Game Starter Kit). The controls page.

#include <optional>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Screen.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Graphics::Texture2D;

class GameManager;
class ScreenManager;

// Port of ShipGame/Screens/ScreenHelp.cs.
class ScreenHelp : public Screen {
public:
    // constructor
    ScreenHelp(ScreenManager* manager, GameManager* game)
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

    std::optional<Texture2D> textureControls_; // controlls text texture
    std::optional<Texture2D> textureDisplay_;  // controller texture
    std::optional<Texture2D> textureContinue_; // continue text texture
};

} // namespace ShipGame
