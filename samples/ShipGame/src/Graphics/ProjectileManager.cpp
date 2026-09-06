// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ProjectileManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ProjectileManager.hpp"

namespace ShipGame {

void ProjectileManager::Update(float elapsedTime) {
    // empty deleted projectiles list
    deleteProjectiles_.clear();

    // for each projectile
    for (auto node = projectiles_.begin(); node != projectiles_.end(); ++node) {
        // update projectile
        bool running = (*node)->Update(elapsedTime, game_);

        // if finished running add to delete list
        if (running == false)
            deleteProjectiles_.push_back(node);
    }

    // delete all nodes in delete list
    for (const auto& s : deleteProjectiles_)
        projectiles_.erase(s);
}

void ProjectileManager::Draw(GraphicsDevice* gd, RenderTechnique technique, Vector3 cameraPosition,
                             Matrix viewProjection, LightList* lights) {
    // draw all projectiles
    for (const auto& p : projectiles_)
        p->Draw(game_, gd, technique, cameraPosition, viewProjection, lights);
}

} // namespace ShipGame
