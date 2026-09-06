// SPDX-License-Identifier: MS-PL
#pragma once

// Projectile.hpp — C++ port of ShipGame/Graphics/Projectile.cs (XNA 4.0 Ship
// Game Starter Kit). One blaster bolt or missile in flight, with the explosion
// and trail it carries.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

#include "../GameManager.hpp"
#include "AnimSpriteManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::Model;

class LightList;
class ParticleSystem;

// Port of ShipGame/Graphics/Projectile.cs.
class Projectile {
public:
    // Create a new projectile
    Projectile(ProjectileType type, Model* model, int player, float velocity, float damage,
               Matrix source, Vector3 destination, RenderTechnique technique);

    // Set projectile explosion parameters
    void SetExplosion(AnimSpriteType sprite, float size, float frameRate, DrawMode mode,
                      float damage, float damageRadius, std::optional<std::string> sound);

    // Set projectile trail parameters
    void SetTrail(std::shared_ptr<ParticleSystem> trail, Matrix transform);

    // Update projectile
    bool Update(float elapsedTime, GameManager* game);

    // Draw projectile
    void Draw(GameManager* game, GraphicsDevice* gd, RenderTechnique defaultTechnique,
              Vector3 cameraPosition, Matrix viewProjection, LightList* lights);

private:
    ProjectileType projectileType_; // projectile type

    RenderTechnique technique_; // render technique

    Model* model_ = nullptr; // projectile model
    int player_;             // player owning the projectile

    Vector3 sourcePosition_;      // source position
    Vector3 destinationPosition_; // destination position

    float elapsedTime_; // elapsed time since created
    float totalTime_;   // total time to reach destination

    Matrix transform_; // current projectile transform matrix

    float contactDamage_; // contact damage if projectile hits a player

    AnimSpriteType animatedSprite_{};                   // animated sprite to play when projectile hits
    float animatedSpriteSize_ = 0.0f;                   // size of explosion animated sprite
    float animatedSpriteFrameRate_ = 0.0f;              // framerate for explosion animated sprite
    DrawMode animatedSpriteDrawMode_{};            // animated sprite drawing mode
    float explosionDamage_ = 0.0f;                      // splash damage for explosion
    float explosionDamageRadius_ = 0.0f;                // splash damage radius
    std::optional<std::string> explosionSound_;         // explosion sound

    // particle system used for projectile trail. The manager can drop the system from its own
    // list while this projectile is still steering it, so the reference is a shared owner --
    // which is what the C# field was, the garbage collector keeping the object alive.
    std::shared_ptr<ParticleSystem> system_;
    Matrix systemTransform_; // local transform to position particle system
};

} // namespace ShipGame
