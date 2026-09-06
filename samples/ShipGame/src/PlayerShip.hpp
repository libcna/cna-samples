// SPDX-License-Identifier: MS-PL
#pragma once

// PlayerShip.hpp — C++ port of ShipGame/PlayerShip.cs (XNA 4.0 Ship Game
// Starter Kit). One player's ship: charges, weapons, collision response,
// bobbing and the camera that follows it.
//
// The bodies live in PlayerShip.cpp because every one of them talks to
// GameManager, which in turn owns the players.

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "System/IDisposable.hpp"
#include "System/Random.hpp"

#include "BoxCollider/CollisionMesh.hpp"
#include "BoxCollider/CollisionTreeElem.hpp"
#include "ChaseCamera.hpp"
#include "EntityList.hpp"
#include "GameManager.hpp"
#include "GameOptions.hpp"
#include "InputManager.hpp"
#include "PlayerMovement.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::Model;
using BoxCollider::CollisionMesh;
using BoxCollider::CollisionTreeElemDynamic;

class AnimSprite;
class LightList;
class ParticleSystem;

// Port of ShipGame/PlayerShip.cs.
class PlayerShip : public CollisionTreeElemDynamic, public System::IDisposable {
public:
    // Create a new player ship
    PlayerShip(GameManager* game,                       // game manager
               int player,                              // player id
               Model model,                             // model for player ship
               std::shared_ptr<EntityList> entities,    // entity list for ship model
               float radius);                           // collision box radius

    ~PlayerShip() override;

    // Returns true if player is currently alive
    bool getIsAlive() const { return (deadTime_ == 0.0f); }

    // Returns player total points
    int getScore() const { return score_; }
    void setScore(int value) { score_ = value; }

    // Adds energy to ship (positive for adding and negative for subtracting)
    void AddEnergy(float value);

    // Adds missiles to ship (positive for adding and negative for subtracting)
    void AddMissile(int value);

    // Get the hud bars values (energy, shield and boost) as a Vector3
    Vector3 getBars() const { return Vector3(energy_, shield_, boost_); }

    // Renturns the number of missiles available
    int getMissileCount() const { return missileCount_; }

    // Return color to be used by damage screen
    Color getDamageColor();

    // Get current camera positon in world space
    Vector3 getCameraPosition() const;

    // Get current player position in world space
    // return player position including bobbing
    Vector3 getPosition() const { return (bobbing_ * transform_).getTranslationProperty(); }

    // Get the camera view matrix
    Matrix getViewMatrix() const;

    // Get current player transform matrix (world matrix)
    Matrix getTransform() const { return bobbing_ * transform_; }

    // Get camera up vector
    Vector3 getViewUp() const;

    // True if camera in 3rd person mode
    bool getCamera3rdPerson() const { return camera3rdPerson_; }

    // Adds a impulse force to player
    // (used to push player on explosion and player to player collision)
    void AddImpulseForce(Vector3 force);

    // Fire a projectile from ship with given type and velocity
    void FireProjectile(ProjectileType projectile, float velocity);

    // Process input for player ship (movement and weapons)
    void ProcessInput(float elapsedTime, InputManager* input, int player);

    // Reset player ship to given position and rotation and reset chase camera
    void Reset(Matrix newTransform);

    // Updates the player ship for given elapsed time
    void Update(float elapsedTime,        // elapsed time on this frame
                CollisionMesh* collision, // level collision mesh
                EntityList* entities);    // level spawn points

    // Renders the player ship model and
    // blaster and missile if available and charged
    void Draw(GraphicsDevice* gd, RenderTechnique technique, Vector3 cameraPosition,
              Matrix viewProjection, LightList* lights);

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override;

private:
    void Dispose(bool disposing);

    bool isDisposed_ = false;

    float shield_ = 1.0f; // current shield charge (1.0 when ready to use)
    float boost_ = 1.0f;  // curren boost charge (1.0 when ready to use)
    float energy_ = 1.0f; // energy charge (0.0 when ship is destroyed)

    bool shieldUse_ = false; // shield is active flag
    bool boostUse_ = false;  // boost is active flag

    float deadTime_ = 0.4f; // time left before ship respawn after death

    float blaster_ = 0.0f; // blaster charge (1.0 when ready to fire)
    float missile_ = 0.0f; // missile charge (1.0 when ready to fire)
    int missileCount_ = 0; // number of missiles available

    float damageTime_ = 0.0f; // time left showing damage screen
                              // (0.0 for no damage screen)
    Vector4 damageColor_;     // current damage screen color

    GameManager* gameManager_ = nullptr; // the game manager
    int playerIndex_ = 0;                // the player index for this ship
    int score_ = 0;                      // the player current score

    // player ship model. Held by value because Content.Load hands back a copy that owns its
    // resources, and nothing else in the game keeps the per-player ship model alive.
    std::optional<Model> shipModel_;
    std::shared_ptr<EntityList> shipEntities_; // player ship model entities
                                               // (blaster, missile, engine locations)

    PlayerMovement movement_; // player movement
                              // (handles control forces and collision response)
    bool collisionSound_ = false; // collision sound ready to play (use to disable
                                  // multiple collision sounds when sliding through walls)

    Matrix transform_;        // the player transform matrix (position/rotation)
    Matrix transformInverse_; // inverse of player transform matrix

    // view target offset for 1st person camera
    Matrix viewOffset_ = Matrix::CreateTranslation(GameOptions::CameraViewOffset);

    // shield animated sprite (not null when shild is active)
    std::shared_ptr<AnimSprite> animatedSpriteShield_;

    std::shared_ptr<ParticleSystem> particleBoost_; // engine particle system
    Matrix boostTransform_;                         // current transform for engine position

    float bobbingTime_ = 0.0f;                              // current time for ship bobbing
    Matrix bobbing_ = Matrix::getIdentityProperty();        // bobbing matrix
    Matrix bobbingInverse_ = Matrix::getIdentityProperty(); // inverse of bobbing matrix

    System::Random random_; // random generator

    bool camera3rdPerson_ = false; // is camera in 3rd person mode?
    ChaseCamera chaseCamera_;      // chase camera object
};

} // namespace ShipGame
