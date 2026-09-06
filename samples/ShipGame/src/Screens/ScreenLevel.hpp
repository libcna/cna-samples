// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenLevel.hpp — C++ port of ShipGame/Screens/ScreenLevel.cs (XNA 4.0 Ship
// Game Starter Kit). The level picker.

#include <array>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Screen.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Graphics::Texture2D;

class GameManager;
class ScreenManager;

// Port of ShipGame/Screens/ScreenLevel.cs.
class ScreenLevel : public Screen {
public:
    // constructor
    ScreenLevel(ScreenManager* manager, GameManager* game)
        : screenManager_(manager), gameManager_(game) {}

    // called before screen shows
    void SetFocus(ContentManager* content, bool focus) override;

    void ProcessInput(float elapsedTime, InputManager* input) override;

    void Update(float elapsedTime) override;

    void Draw3D(GraphicsDevice* gd) override;

    void Draw2D(GraphicsDevice* gd, FontManager* font) override;

private:
    static constexpr int NumberLevels = 2; // number of available levels to choose from

    ScreenManager* screenManager_ = nullptr; // screen manager
    GameManager* gameManager_ = nullptr;     // game manager

    // name for each level
    std::array<std::string, NumberLevels> levels_{"level1", "level2"};

    // screen shot for each level
    std::array<std::optional<Texture2D>, NumberLevels> levelShots_;

    std::optional<Texture2D> selectBack_;  // select and back texture
    std::optional<Texture2D> changeLevel_; // change level texture

    int selection_ = 0;
};

} // namespace ShipGame
