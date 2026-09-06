// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Powerup.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Powerup.hpp"

#include <algorithm>
#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/ArgumentNullException.hpp"

#include "../GameOptions.hpp"
#include "../PlayerShip.hpp"
#include "AnimSpriteManager.hpp"
#include "LightList.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::MathHelper;

bool Powerup::Update(GameManager* game, float elapsedTime) {
    if (game == nullptr) {
        throw System::ArgumentNullException("game");
    }

    // add elapsed time for this frame
    this->elapsedTime_ += elapsedTime;

    // if waiting to respawn
    if (waitTime_ > 0) {
        // decrease wait time
        waitTime_ = std::max(0.0f, waitTime_ - elapsedTime);

        // if wait time is finished
        if (waitTime_ == 0) {
            // add powerup spawn animated sprite
            game->AddAnimSprite(AnimSpriteType::Spawn, transform_.getTranslationProperty(), 50, 40,
                                30, DrawMode::Additive, -1);

            // play powerup spawn sound
            game->PlaySound3D("powerup_spawn", transform_.getTranslationProperty());
        }

        // return true to keep powerup alive
        return true;
    }

    // calculate bobbing angles
    float turn_angle =
        std::fmod(this->elapsedTime_ * GameOptions::PowerupTurnSpeed, MathHelper::TwoPi);
    float move_angle =
        std::fmod(this->elapsedTime_ * GameOptions::PowerupMoveSpeed, MathHelper::TwoPi);

    // create bobbing matrix
    bobbing_ = Matrix::CreateRotationY(turn_angle) *
               Matrix::CreateTranslation((float)std::cos(move_angle) *
                                         GameOptions::PowerupMoveDistance * Vector3::Up);

    // check for any player at the powerup location
    int playerHit = game->GetPlayerAtPosition(transform_.getTranslationProperty());
    if (playerHit != -1) {
        // disable powerup until respawn time
        waitTime_ = GameOptions::PowerupRespawnTime;

        // get player at powerup location
        PlayerShip* p = game->GetPlayer(playerHit);
        switch (powerupType_) {
        case PowerupType::Energy:
            p->AddEnergy(0.5f); // add 50% energy
            break;
        case PowerupType::Missile:
            p->AddMissile(3); // add 3 missiles
            break;
        }

        // add powerup spawn animates sprite
        game->AddAnimSprite(AnimSpriteType::Spawn, transform_.getTranslationProperty(), 40, 40, 30,
                            DrawMode::Additive, -1);

        // play powerup get sound
        game->PlaySound("powerup_get");
    }

    // return true to keep powerup alive
    return true;
}

void Powerup::Draw(GameManager* game, GraphicsDevice* gd, RenderTechnique technique,
                   Vector3 cameraPosition, Matrix viewProjection, LightList* lights) {
    if (game == nullptr) {
        throw System::ArgumentNullException("game");
    }

    // if now waiting to respawn
    if (waitTime_ == 0) {
        // draw powerup model
        game->DrawModel(gd, model_, technique, cameraPosition, bobbing_ * transform_,
                        viewProjection, lights);
    }
}

} // namespace ShipGame
