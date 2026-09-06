// SPDX-License-Identifier: MS-PL
#pragma once

// PowerupManager.hpp — C++ port of ShipGame/Graphics/PowerupManager.cs (XNA 4.0
// Ship Game Starter Kit). Owns every powerup in the level.

#include <list>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

#include "../GameManager.hpp"
#include "Powerup.hpp"

namespace ShipGame {

class LightList;

// Port of ShipGame/Graphics/PowerupManager.cs.
class PowerupManager {
public:
    // Create a new powerup manager
    explicit PowerupManager(GameManager* game) : game_(game) {}

    // Add a new powerup
    void Add(std::shared_ptr<Powerup> p) { powerups_.push_back(std::move(p)); }

    // Empty powerups list
    void Clear() { powerups_.clear(); }

    // Update all powerups
    void Update(float elapsedTime);

    // Draw all powerups
    void Draw(GraphicsDevice* gd, RenderTechnique technique, Vector3 cameraPosition,
              Matrix viewProjection, LightList* lights);

private:
    GameManager* game_ = nullptr; // game manager

    // linked list of active powerups
    std::list<std::shared_ptr<Powerup>> powerups_;

    // linked list of nodes to delete from the powerups list
    std::vector<std::list<std::shared_ptr<Powerup>>::iterator> deletePowerups_;
};

} // namespace ShipGame
