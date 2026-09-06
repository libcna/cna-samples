// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenPlayer.hpp — C++ port of ShipGame/Screens/ScreenPlayer.cs (XNA 4.0 Ship
// Game Starter Kit). Ship selection, one podium per player.

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"

#include "Screen.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::TextureCube;

class GameManager;
class LightList;
class ScreenManager;

// Port of ShipGame/Screens/ScreenPlayer.cs.
class ScreenPlayer : public Screen {
public:
    // constructor
    ScreenPlayer(ScreenManager* manager, GameManager* game)
        : screenManager_(manager), gameManager_(game) {}

    // called before screen shows
    void SetFocus(ContentManager* content, bool focus) override;

    void ProcessInput(float elapsedTime, InputManager* input) override;

    void Update(float elapsedTime) override;

    void Draw3D(GraphicsDevice* gd) override;

    void Draw2D(GraphicsDevice* gd, FontManager* font) override;

private:
    // Performs effect initialization, which is required in XNA 4.0
    void FixupShip(Model& model, const std::string& path);

    // Creates a reflection textureCube
    static TextureCube* GetReflectCube();

    static constexpr int NumberShips = 2; // number of available ships to choose from

    ScreenManager* screenManager_ = nullptr; // screen manager
    GameManager* gameManager_ = nullptr;     // game manager

    // name for each ship
    std::array<std::string, NumberShips> ships_{"ship2", "ship1"};

    // model for each ship
    std::array<std::optional<Model>, NumberShips> shipModels_;

    std::optional<Model> padModel_;       // ship pad model
    std::optional<Model> padHaloModel_;   // ship pad halo model
    std::optional<Model> padSelectModel_; // ship pad select model

    std::optional<Texture2D> textureChangeShip_;     // change ship texture
    std::optional<Texture2D> textureRotateShip_;     // rotate ship texture
    std::optional<Texture2D> textureSelectBack_;     // select and back texture
    std::optional<Texture2D> textureSelectCancel_;   // select and cancel texture
    std::optional<Texture2D> textureInvertYCheck_;   // checked invert y texture
    std::optional<Texture2D> textureInvertYUncheck_; // unchecked invert y texture

    std::shared_ptr<LightList> lights_; // lights for scene

    static std::optional<TextureCube> reflectCube_;

    // ship selection for each player
    std::array<int, 2> selection_{0, 1};

    // confirmed status for each player
    std::array<bool, 2> confirmed_{false, false};

    // invert Y flags (bit flag for each player)
    std::uint32_t invertY_ = 0;

    // rotation matrix for each player ship model
    std::array<Matrix, 2> rotation_{Matrix::getIdentityProperty(),
                                    Matrix::getIdentityProperty()};

    // total elapsed time for ship model rotation
    float elapsedTime_ = 0.0f;
};

} // namespace ShipGame
