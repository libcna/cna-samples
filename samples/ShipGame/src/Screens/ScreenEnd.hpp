// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenEnd.hpp — C++ port of ShipGame/Screens/ScreenEnd.cs (XNA 4.0 Ship Game
// Starter Kit). The winner's podium.

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Screen.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::Texture2D;

class GameManager;
class LightList;
class ScreenManager;

// Port of ShipGame/Screens/ScreenEnd.cs.
class ScreenEnd : public Screen {
public:
    // constructor
    ScreenEnd(ScreenManager* manager, GameManager* game)
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

    std::optional<Model> shipModel_; // winner player ship model

    std::optional<Model> padModel_;     // model for the ship pad
    std::optional<Model> padHaloModel_; // model for the ship pad halo

    std::shared_ptr<LightList> lights_; // lights for scene

    std::optional<Texture2D> texturePlayerWin_; // texture with winning player number
    std::optional<Texture2D> textureContinue_;  // texture with continue message

    float elapsedTime_ = 0.0f; // elapsed time for rotation animation
};

} // namespace ShipGame
