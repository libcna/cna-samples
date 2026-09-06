// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PowerupManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "PowerupManager.hpp"

namespace ShipGame {

void PowerupManager::Update(float elapsedTime) {
    // empty deleted powerups list
    deletePowerups_.clear();

    // for each powerup
    for (auto node = powerups_.begin(); node != powerups_.end(); ++node) {
        // update powerup
        bool running = (*node)->Update(game_, elapsedTime);

        // if finished running add to delete list
        if (running == false)
            deletePowerups_.push_back(node);
    }

    // delete all nodes in delete list
    for (const auto& s : deletePowerups_)
        powerups_.erase(s);
}

void PowerupManager::Draw(GraphicsDevice* gd, RenderTechnique technique, Vector3 cameraPosition,
                          Matrix viewProjection, LightList* lights) {
    // draw all powerups
    for (const auto& p : powerups_)
        p->Draw(game_, gd, technique, cameraPosition, viewProjection, lights);
}

} // namespace ShipGame
