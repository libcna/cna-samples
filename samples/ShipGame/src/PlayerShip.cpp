// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlayerShip.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "PlayerShip.hpp"

#include <algorithm>
#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/ArgumentNullException.hpp"

#include "Graphics/AnimSprite.hpp"
#include "Graphics/AnimSpriteManager.hpp"
#include "Graphics/LightList.hpp"
#include "Graphics/ParticleSystem.hpp"
#include "Graphics/Projectile.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Input::Keys;

PlayerShip::PlayerShip(GameManager* game, int player, Model model,
                       std::shared_ptr<EntityList> entities, float radius)
    : random_(player) {
    if (game == nullptr) {
        throw System::ArgumentNullException("game");
    }
    if (entities == nullptr) {
        throw System::ArgumentNullException("entities");
    }

    // save parameters
    gameManager_ = game;
    shipModel_ = std::move(model);
    shipEntities_ = std::move(entities);
    playerIndex_ = player;

    // create movement controller
    movement_.maxVelocity = GameOptions::MovementVelocity;

    // enable collision sound
    collisionSound_ = true;

    // create engine particle system with infinite life time
    particleBoost_ = gameManager_->AddParticleSystem(ParticleSystemType::ShipTrail, transform_);
    particleBoost_->SetTotalTime(1e10f);
    boostTransform_ = shipEntities_->GetTransform("engine");

    // create the ship collision box
    box.emplace(-radius, radius);

    // random bobbing offset
    bobbingTime_ = (float)random_.NextDouble();

    // setup 3rd person camera parameters
    camera3rdPerson_ = false;
    chaseCamera_.setDesiredPositionOffset(GameOptions::CameraOffset);
    chaseCamera_.setLookAtOffset(GameOptions::CameraTargetOffset);
    chaseCamera_.setStiffness(GameOptions::CameraStiffness);
    chaseCamera_.setDamping(GameOptions::CameraDamping);
    chaseCamera_.setMass(GameOptions::CameraMass);
}

PlayerShip::~PlayerShip() { Dispose(true); }

void PlayerShip::AddEnergy(float value) {
    // if shield active and damaging energy
    if (value < 0 && shieldUse_) {
        // play shield collide sound and return (no damage)
        gameManager_->PlaySound("shield_collide");
        return;
    }

    // apply value to energy
    energy_ = std::max(0.0f, std::min(1.0f, energy_ + value));

    // if reducing energy, add damage screen intensity and timeout
    if (value < 0) {
        float intensity = damageTime_ / GameOptions::DamageFadeout;
        damageColor_ = damageColor_ * intensity + Vector4::UnitX;
        damageTime_ = GameOptions::DamageFadeout;
    }

    // if no more energy, kill player
    if (energy_ == 0.0f)
        deadTime_ = GameOptions::DeathTimeout;
}

void PlayerShip::AddMissile(int value) {
    missileCount_ = std::max(0, std::min(9, missileCount_ + value));
}

Color PlayerShip::getDamageColor() {
    damageColor_.W = damageTime_ / GameOptions::DamageFadeout;
    return Color(damageColor_);
}

Vector3 PlayerShip::getCameraPosition() const {
    // if in 3rd person mode
    if (camera3rdPerson_) {
        // return chase camera position
        return chaseCamera_.getPosition();
    } else {
        // return player position
        return getPosition();
    }
}

Matrix PlayerShip::getViewMatrix() const {
    // if in 3rd person mode
    if (camera3rdPerson_) {
        // return chase camera view matrix
        return chaseCamera_.getView();
    } else {
        // return player view matrix including bobing and view offset
        return transformInverse_ * bobbingInverse_ * viewOffset_;
    }
}

Vector3 PlayerShip::getViewUp() const {
    // if 3rd person mode
    if (camera3rdPerson_) {
        // return chase camera up vector
        return chaseCamera_.getView().getUpProperty();
    } else {
        // return player up vector
        return transform_.getUpProperty();
    }
}

void PlayerShip::AddImpulseForce(Vector3 force) {
    movement_.velocity.X += Vector3::Dot(movement_.rotation.getRightProperty(), force);
    movement_.velocity.Y += Vector3::Dot(movement_.rotation.getUpProperty(), force);
    movement_.velocity.Z += Vector3::Dot(movement_.rotation.getForwardProperty(), force);
}

void PlayerShip::FireProjectile(ProjectileType projectile, float velocity) {
    switch (projectile) {
    case ProjectileType::Blaster: {
        // fire left blaster
        Matrix m = shipEntities_->GetTransform("blaster_left") * bobbing_ * transform_;

        std::shared_ptr<Projectile> p = gameManager_->AddProjectile(
            projectile, playerIndex_, m, velocity, 0.1f, RenderTechnique::ViewMapping);
        p->SetExplosion(AnimSpriteType::Blaster, 30, 30, DrawMode::AdditiveAndGlow, 0, 0,
                        std::nullopt);

        // fire right blaster
        m = shipEntities_->GetTransform("blaster_right") * bobbing_ * transform_;

        p = gameManager_->AddProjectile(projectile, playerIndex_, m, velocity, 0.1f,
                                        RenderTechnique::ViewMapping);
        p->SetExplosion(AnimSpriteType::Blaster, 30, 30, DrawMode::AdditiveAndGlow, 0, 0,
                        std::nullopt);

        // play blaster fire sound
        gameManager_->PlaySound("fire_primary");
    } break;
    case ProjectileType::Missile: {
        // fire missile
        Matrix m = shipEntities_->GetTransform("missile") * bobbing_ * transform_;

        std::shared_ptr<Projectile> p = gameManager_->AddProjectile(
            projectile, playerIndex_, m, velocity, 0.1f, RenderTechnique::NormalMapping);
        p->SetExplosion(AnimSpriteType::Missile, 90, 30, DrawMode::AdditiveAndGlow, 0.5f, 500,
                        "missile_explode");

        // set missile trail
        std::shared_ptr<ParticleSystem> Trail = gameManager_->AddParticleSystem(
            ParticleSystemType::MissileTrail, Matrix::getIdentityProperty());
        p->SetTrail(Trail, Matrix::CreateTranslation(GameOptions::MissileTrailOffset));

        // play missile fire sound
        gameManager_->PlaySound("fire_secondary");
    } break;
    }
}

void PlayerShip::ProcessInput(float elapsedTime, InputManager* input, int player) {
    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    // if dead, don't process input for player
    if (getIsAlive() == false) {
        return;
    }

    // process movement related inputs
    movement_.ProcessInput(elapsedTime, input->getCurrentState(), player);

    // if player invert Y is enabled, invert X rotation force
    if (gameManager_->GetInvertY(player))
        movement_.rotationForce.X = -movement_.rotationForce.X;

    // if blaster ready and input activated
    if (blaster_ == 1)
        if (input->IsKeyPressed(player, Keys::Space) ||
            input->getCurrentState().padState[player].getTriggersProperty().getRightProperty() >
                0) {
            // fire blaster
            FireProjectile(ProjectileType::Blaster, GameOptions::BlasterVelocity);
            // reset charge time
            blaster_ = 0;
        }

    // if missile is ready and input activated
    if (missile_ == 1 && missileCount_ > 0)
        if (input->IsKeyPressed(player, Keys::Enter) || input->IsTriggerPressedLeft(player)) {
            // fire missile
            FireProjectile(ProjectileType::Missile, GameOptions::MissileVelocity);
            // subtract missile count
            AddMissile(-1);
            // reset charge time
            missile_ = 0;
        }

    // if shield is ready and input
    if (shield_ == 1)
        if (input->IsKeyPressed(player, Keys::R) || input->IsButtonPressedA(player)) {
            // activate shield
            shieldUse_ = true;
            // create animated sprite
            animatedSpriteShield_ = gameManager_->AddAnimSprite(
                AnimSpriteType::Shield,
                transform_.getTranslationProperty() + transform_.getForwardProperty() * 10, 160, 80,
                15, DrawMode::Additive, player);
            // play shield sound
            gameManager_->PlaySound("shield_activate");
        }

    // if boost ready and input activated
    if (boost_ == 1)
        if (input->IsKeyPressed(player, Keys::LeftShift) ||
            input->IsKeyPressed(player, Keys::RightShift) || input->IsButtonPressedY(player) ||
            input->IsButtonPressedLeftStick(player)) {
            // activate boost
            boostUse_ = true;
            // play boost sound
            gameManager_->PlaySound("ship_boost");
        }

    // if camara switch input activated
    if (input->IsKeyPressed(player, Keys::Back) || input->IsButtonPressedB(player)) {
        // switch 3rd person mode
        camera3rdPerson_ = !camera3rdPerson_;
        // reset camera
        chaseCamera_.Reset();
    }
}

void PlayerShip::Reset(Matrix newTransform) {
    // reset movement to new transform
    movement_.Reset(newTransform);

    // store new transform and its inverse
    transform_ = newTransform;
    transformInverse_ = Matrix::Invert(newTransform);

    // reset chase camera
    chaseCamera_.setChasePosition(transform_.getTranslationProperty());
    chaseCamera_.setChaseDirection(transform_.getForwardProperty());
    chaseCamera_.setUp(transform_.getUpProperty());
    chaseCamera_.Reset();
}

void PlayerShip::Update(float elapsedTime, CollisionMesh* collision, EntityList* entities) {
    if (collision == nullptr) {
        throw System::ArgumentNullException("collision");
    }
    if (entities == nullptr) {
        throw System::ArgumentNullException("entities");
    }

    // updates damage screen time (zero for no damage indication)
    damageTime_ = std::max(0.0f, damageTime_ - elapsedTime);

    // if player dead
    if (getIsAlive() == false) {
        // disable engine particle system
        particleBoost_->setEnabled(false);

        // updates dead time (if zero, player is alive)
        deadTime_ = std::max(0.0f, deadTime_ - elapsedTime);

        // if player dead time expires, respawn
        if (getIsAlive() == true) {
            // reset player to a random spawn point
            Reset(entities->GetTransformRandom(random_));

            // add spawn animated sprite in front of player
            Vector3 Pos = movement_.position + 10 * movement_.rotation.getForwardProperty();
            gameManager_->AddAnimSprite(AnimSpriteType::Spawn, Pos, 140, 80, 30, DrawMode::Additive,
                                        playerIndex_);

            // play spawn sound
            gameManager_->PlaySound("ship_spawn");

            // reset energy, shield and boost
            energy_ = 1.0f;
            shield_ = 1.0f;
            boost_ = 1.0f;
            missileCount_ = 3;
        }

        return;
    }

    // hold position before movement
    Vector3 lastPostion = movement_.position;

    // update movement
    movement_.Update(elapsedTime);

    // test for collision with level
    Vector3 collisionPosition;
    if (collision->BoxMove(*box, lastPostion, movement_.position, 1.0f, 0.0f, 3,
                           collisionPosition)) {
        // update to valid position after collision
        movement_.position = collisionPosition;

        // compute new velocity after collision
        Vector3 newVelocity = (collisionPosition - lastPostion) * (1.0f / elapsedTime);

        // if collision sound enabled
        if (collisionSound_) {
            // test collision angle to play collision sound
            Vector3 WorldVel = movement_.getWorldVelocity();
            float dot = Vector3::Dot(Vector3::Normalize(WorldVel), Vector3::Normalize(newVelocity));
            if (dot < 0.7071f) {
                // play collision sound
                gameManager_->PlaySound("ship_collide");

                // set rumble intensity
                dot = 1 - 0.5f * (dot + 1);
                gameManager_->SetVibration(playerIndex_, dot * 0.5f);

                // disable collision sounds until ship stops colliding
                collisionSound_ = false;
            }
        }

        // set new velocity after collision
        movement_.setWorldVelocity(newVelocity);
    } else
        // clear of collisions, re-enable collision sounds
        collisionSound_ = true;

    // update player transform
    transform_ = movement_.rotation;
    transform_.setTranslationProperty(movement_.position);

    // compute inverse transform
    transformInverse_ = Matrix::Invert(transform_);

    // get normalized player velocity
    float velocityFactor = movement_.getVelocityFactor();

    // update bobbing
    bobbingTime_ += elapsedTime;
    float bobbingFactor = 1.0f - velocityFactor;
    float time = std::fmod(GameOptions::ShipBobbingSpeed * bobbingTime_, 2 * MathHelper::TwoPi);
    float distance = bobbingFactor * GameOptions::ShipBobbingRange;
    bobbing_.M41 = distance * (float)std::sin(time * 0.5f);
    bobbing_.M42 = distance * (float)std::sin(time);
    bobbingInverse_.M41 = -bobbing_.M41;
    bobbingInverse_.M42 = -bobbing_.M42;

    // compute transform with bobbing
    Matrix bobbingTransform = bobbing_ * transform_;

    // update particle system position
    particleBoost_->setEnabled(true);
    particleBoost_->SetTransform(boostTransform_ * bobbingTransform);

    // if shield active
    if (shieldUse_) {
        // update shield position
        animatedSpriteShield_->setPosition(bobbingTransform.getTranslationProperty() +
                                           10.0f * bobbingTransform.getForwardProperty());

        // update shiled charge
        shield_ -= elapsedTime / GameOptions::ShieldUse;

        // if shield charge depleted
        if (shield_ < 0) {
            // disable shield
            shieldUse_ = false;
            shield_ = 0;

            // kill shield animated sprite
            animatedSpriteShield_->SetTotalTime(0);
            animatedSpriteShield_ = nullptr;
        }
    } else
        // change shield
        shield_ = std::min(1.0f, shield_ + elapsedTime / GameOptions::ShieldRecharge);

    // if boost active
    if (boostUse_) {
        // increase ship maximum velocity
        movement_.maxVelocity = GameOptions::MovementVelocityBoost;
        // apply impulse force forward
        AddImpulseForce(transform_.getForwardProperty() * GameOptions::BoostForce);

        // set particle system velocity scale
        particleBoost_->setVelocityScale(
            std::min(1.0f, particleBoost_->getVelocityScale() + 4.0f * elapsedTime));

        // update shield charge
        boost_ -= elapsedTime / GameOptions::BoostUse;

        // if  boost depleated
        if (boost_ < 0) {
            // disable boost
            boostUse_ = false;
            boost_ = 0;
        }
    } else {
        // slowly returns ship maximum velocity to normal levels
        if (movement_.maxVelocity > GameOptions::MovementVelocity)
            movement_.maxVelocity -= GameOptions::BoostSlowdown * elapsedTime;

        // slowly returns particle system velocity scale to normal levels
        particleBoost_->setVelocityScale(
            std::max(0.1f, particleBoost_->getVelocityScale() - 2.0f * elapsedTime));

        // charge boost
        boost_ = std::min(1.0f, boost_ + elapsedTime / GameOptions::BoostRecharge);
    }

    // charge blaster
    blaster_ = std::min(1.0f, blaster_ + elapsedTime / GameOptions::BlasterChargeTime);

    // charge missile
    missile_ = std::min(1.0f, missile_ + elapsedTime / GameOptions::MissileChargeTime);

    // update chase camera
    chaseCamera_.setChasePosition(transform_.getTranslationProperty());
    chaseCamera_.setChaseDirection(transform_.getForwardProperty());
    chaseCamera_.setUp(transform_.getUpProperty());
    chaseCamera_.Update(elapsedTime, collision);
}

void PlayerShip::Draw(GraphicsDevice* gd, RenderTechnique technique, Vector3 cameraPosition,
                      Matrix viewProjection, LightList* lights) {
    // if not dead
    if (deadTime_ == 0.0f) {
        // render ship model
        gameManager_->DrawModel(gd, &shipModel_.value(), technique, cameraPosition, bobbing_ * transform_,
                                viewProjection, lights);
    }
}

void PlayerShip::Dispose() { Dispose(true); }

void PlayerShip::Dispose(bool disposing) {
    particleBoost_->SetTotalTime(0);
    if (disposing && !isDisposed_) {
        box.reset();
    }
}

} // namespace ShipGame
