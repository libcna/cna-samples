// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Projectile.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Projectile.hpp"

#include <algorithm>

#include "System/ArgumentNullException.hpp"

#include "../PlayerShip.hpp"
#include "LightList.hpp"
#include "ParticleSystem.hpp"

namespace ShipGame {

Projectile::Projectile(ProjectileType type, Model* model, int player, float velocity, float damage,
                       Matrix source, Vector3 destination, RenderTechnique technique) {
    projectileType_ = type;
    this->model_ = model;
    this->player_ = player;

    sourcePosition_ = source.getTranslationProperty();
    destinationPosition_ = destination;

    this->technique_ = technique;

    contactDamage_ = damage;

    elapsedTime_ = 0;
    totalTime_ = (source.getTranslationProperty() - destination).Length() / velocity;
    transform_ = source;
}

void Projectile::SetExplosion(AnimSpriteType sprite, float size, float frameRate, DrawMode mode,
                              float damage, float damageRadius, std::optional<std::string> sound) {
    animatedSprite_ = sprite;
    animatedSpriteSize_ = size;
    animatedSpriteFrameRate_ = frameRate;
    animatedSpriteDrawMode_ = mode;
    explosionDamage_ = damage;
    explosionDamageRadius_ = damageRadius;
    explosionSound_ = std::move(sound);
}

void Projectile::SetTrail(std::shared_ptr<ParticleSystem> trail, Matrix transform) {
    system_ = std::move(trail);
    systemTransform_ = transform;
}

bool Projectile::Update(float elapsedTime, GameManager* game) {
    if (game == nullptr) {
        throw System::ArgumentNullException("game");
    }

    // add elapsed time for this frame
    this->elapsedTime_ += elapsedTime;

    // normalize time
    float normalizedTime = std::min(1.0f, this->elapsedTime_ / totalTime_);

    // compute current projectile position
    Vector3 position = Vector3::Lerp(sourcePosition_, destinationPosition_, normalizedTime);

    // set postion into projectile transform matrix
    transform_.setTranslationProperty(position);

    // if projectile includes a particle system update its position
    if (system_ != nullptr)
        system_->SetTransform(systemTransform_ * transform_);

    // check if projectile hit any player
    int playerHit = game->GetPlayerAtPosition(position);

    // if a player is hit or reached destination explode projectile
    if ((playerHit != player_ && playerHit != -1) || normalizedTime == 1.0f) {
        // compute explosion position moving hit point into hit normal direction
        Vector3 explosionPosition =
            position + 0.5f * animatedSpriteSize_ * transform_.getBackwardProperty();

        // set transform to explosion position
        transform_.setTranslationProperty(explosionPosition);

        // if an animated sprite explosion is available, create it
        if (animatedSpriteSize_ > 0)
            game->AddAnimSprite(animatedSprite_, explosionPosition, animatedSpriteSize_, 10.0f,
                                animatedSpriteFrameRate_, animatedSpriteDrawMode_, -1);

        // if splash damage is available, apply splash damage to nearby players
        if (explosionDamageRadius_ > 0)
            game->AddDamageSplash(player_, explosionDamage_, explosionPosition,
                                  explosionDamageRadius_);

        // if exploded on a player add contact damage
        if (playerHit != -1 && game->GetPlayer(playerHit)->getIsAlive())
            game->AddDamage(player_, playerHit, contactDamage_,
                            Vector3::Normalize(destinationPosition_ - sourcePosition_));

        // if explosion sound is available, play it
        if (explosionSound_.has_value())
            game->PlaySound3D(*explosionSound_, explosionPosition);

        // add explosion particle system
        if (projectileType_ == ProjectileType::Missile)
            game->AddParticleSystem(ParticleSystemType::MissileExplode, transform_);
        else if (projectileType_ == ProjectileType::Blaster)
            game->AddParticleSystem(ParticleSystemType::BlasterExplode, transform_);

        // kill trail particle system
        if (system_ != nullptr)
            system_->SetTotalTime(-1e10f);

        // return false to kill the projectile
        return false;
    }

    // return true to keep projectile alive
    return true;
}

void Projectile::Draw(GameManager* game, GraphicsDevice* gd, RenderTechnique defaultTechnique,
                      Vector3 cameraPosition, Matrix viewProjection, LightList* lights) {
    if (game == nullptr) {
        throw System::ArgumentNullException("game");
    }

    if (technique_ == RenderTechnique::ViewMapping) {
        game->DrawModel(gd, model_, technique_, cameraPosition, transform_, viewProjection,
                        nullptr);
    } else {
        game->DrawModel(gd, model_, defaultTechnique, cameraPosition, transform_, viewProjection,
                        lights);
    }
}

} // namespace ShipGame
