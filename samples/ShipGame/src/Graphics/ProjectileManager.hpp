// SPDX-License-Identifier: MS-PL
#pragma once

// ProjectileManager.hpp — C++ port of ShipGame/Graphics/ProjectileManager.cs
// (XNA 4.0 Ship Game Starter Kit). Owns every projectile in flight.

#include <list>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

#include "../GameManager.hpp"
#include "Projectile.hpp"

namespace ShipGame {

class LightList;

// Port of ShipGame/Graphics/ProjectileManager.cs.
class ProjectileManager {
public:
    // Create a new projetcile manager
    explicit ProjectileManager(GameManager* game) : game_(game) {}

    // Add a new projectile
    void Add(std::shared_ptr<Projectile> p) { projectiles_.push_back(std::move(p)); }

    // Update all projectiles
    void Update(float elapsedTime);

    // Draw all projectiles
    void Draw(GraphicsDevice* gd, RenderTechnique technique, Vector3 cameraPosition,
              Matrix viewProjection, LightList* lights);

private:
    GameManager* game_ = nullptr; // game manager

    // linked list of active projectiles
    std::list<std::shared_ptr<Projectile>> projectiles_;

    // linked list of nodes to delete from the projectiles list
    std::vector<std::list<std::shared_ptr<Projectile>>::iterator> deleteProjectiles_;
};

} // namespace ShipGame
