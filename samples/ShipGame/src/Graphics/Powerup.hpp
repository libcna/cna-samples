// SPDX-License-Identifier: MS-PL
#pragma once

// Powerup.hpp — C++ port of ShipGame/Graphics/Powerup.cs (XNA 4.0 Ship Game
// Starter Kit). One bobbing pickup: energy or missiles, respawning on a timer.

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

#include "../GameManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::Model;

class LightList;

// Port of ShipGame/Graphics/Powerup.cs.
class Powerup {
public:
    // Create a new powerup
    Powerup(PowerupType type, Matrix transform, Model* model)
        : powerupType_(type), transform_(transform), model_(model) {}

    // Update powerup for given elapsed time
    bool Update(GameManager* game, float elapsedTime);

    // Draw powerup
    void Draw(GameManager* game, GraphicsDevice* gd, RenderTechnique technique,
              Vector3 cameraPosition, Matrix viewProjection, LightList* lights);

private:
    PowerupType powerupType_; // powerup type

    Matrix transform_; // powerup position and orientation
    Matrix bobbing_;   // powerup bobbing transform

    Model* model_ = nullptr; // powerup model

    float elapsedTime_ = 0.0f; // elapsed time since spawned
    float waitTime_ = 0.0f;    // time to wait until respawn
                               // (zero when powerup is pickable)
};

} // namespace ShipGame
