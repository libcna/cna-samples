// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenIntro.hpp — C++ port of ShipGame/Screens/ScreenIntro.cs (XNA 4.0 Ship
// Game Starter Kit). The main menu.

#include <array>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Screen.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Graphics::Texture2D;

class GameManager;
class ScreenManager;

// Port of ShipGame/Screens/ScreenIntro.cs.
class ScreenIntro : public Screen {
public:
    // constructor
    ScreenIntro(ScreenManager* manager, GameManager* game)
        : screenManager_(manager), gameManager_(game) {}

    // called before screen shows or stops showing
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
    // draw the animated cursor
    void DrawCursor(int x, int y);

    // menu itens
    static constexpr int NumberMenuItems = 4;

    ScreenManager* screenManager_ = nullptr; // screen manager
    GameManager* gameManager_ = nullptr;     // game manager

    int menuSelection_ = 0; // current menu selection
    float menuTime_ = 0.0f; // menu time for animation

    std::optional<Texture2D> textureLogo_; // logo texture
    std::optional<Texture2D> textureLens_; // lens texture

    std::optional<Texture2D> textureCursorAnim_; // cursor textures
    std::optional<Texture2D> textureCursorBullet_;
    std::optional<Texture2D> textureCursorArrow_;

    std::array<std::string, NumberMenuItems> menuNames_{"menu_sp", "menu_mp", "menu_hp",
                                                        "menu_qg"};

    // menu textures without hover
    std::array<std::optional<Texture2D>, NumberMenuItems> textureMenu_;
    // menu textures with hover
    std::array<std::optional<Texture2D>, NumberMenuItems> textureMenuHover_;
};

} // namespace ShipGame
