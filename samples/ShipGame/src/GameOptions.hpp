// SPDX-License-Identifier: MS-PL
#pragma once

// GameOptions.hpp — C++ port of ShipGame/GameOptions.cs (XNA 4.0 Ship Game
// Starter Kit). Every tunable the game reads, in the original's own order.
//
// The C# fields are `public static` and mutable; the port keeps them mutable
// statics for the same reason -- nothing in the game writes them, but making
// them constants would be a change the original does not have.

#include <cstdint>

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;

// Port of ShipGame/GameOptions.cs.
struct GameOptions {
    // game screen horizontal resolution
    static inline int ScreenWidth = 1280;
    // game screen vertical resolution
    static inline int ScreenHeight = 720;

    // glow buffer resolution
    static inline int GlowResolution = 512;

    // maximum number of supported players
    static inline int MaxPlayers = 2;
    // max points (kills) to end game
    static inline int MaxPoints = 10;

    // how many octree subdivisions in collision mesh
    static inline std::uint32_t CollisionMeshSubdivisions = 4;

    // size of player collision box
    static inline int CollisionBoxRadius = 60;

    // maximum bones per model
    static inline int MaxBonesPerModel = 128;

    // inpulse force when two ships collide
    static inline float ShipCollidePush = 500;

    // use game pade vibrate?
    static inline bool UseGamepadVibrate = true;
    // gamepad vibration fadeout time
    static inline float VibrationFadeout = 0.1f;
    // gamepad vibration intensity
    static inline float VibrationIntensity = 0.5f;

    // max simultaneous particles per frame
    static inline int MaxParticles = 8192;
    // max simultaneous animated sprites per frame
    static inline int MaxSprites = 128;

    // color used for screen transitions
    static inline Vector4 FadeColor = Vector4::Zero;
    // time for screen transition in seconds
    static inline float FadeTime = 1.0f;

    // time shield is active
    static inline float ShieldUse = 2.0f;
    // time for shield recharge
    static inline float ShieldRecharge = 8.0f;

    // time boost is active
    static inline float BoostUse = 2.0f;
    // time for boost recgarge
    static inline float BoostRecharge = 8.0f;
    // how fast boost slows down after finished
    static inline float BoostSlowdown = 1000.0f;
    // force to apply forward when using boost
    static inline float BoostForce = 50.0f;

    // fadeout time for damage effect
    static inline float DamageFadeout = 0.5f;
    // timeout before you respawn after a kill
    static inline float DeathTimeout = 3.0f;

    // bobbing distance
    static inline float ShipBobbingRange = 4.0f;
    // bobbing speed
    static inline float ShipBobbingSpeed = 4.0f;

    // time between two blasters fire
    static inline float BlasterChargeTime = 0.2f;
    // time between two missiles fire
    static inline float MissileChargeTime = 0.5f;
    // blaster velocity
    static inline float BlasterVelocity = 6000;
    // missile velocity
    static inline float MissileVelocity = 4000;

    // offset for camera in 1st person mode
    static inline Vector3 CameraViewOffset = Vector3(0, -10, 0);
    // offset for camera in 3rd person mode
    static inline Vector3 CameraOffset = Vector3(0, 50, 125);
    // offset for camera target in 3rd person mode
    static inline Vector3 CameraTargetOffset = Vector3(0, 0, -50);
    // stiffness for camera in 3rd person mode
    static inline float CameraStiffness = 3000;
    // damping for camera in 3rd person mode
    static inline float CameraDamping = 600;
    // mass for camera in 3rd person mode
    static inline float CameraMass = 50;
    // offset for missile trail
    static inline Vector3 MissileTrailOffset = Vector3(0, 0, -10);

    // powerups rotation speed
    static inline float PowerupTurnSpeed = 2.0f;
    // up/down powerup movement speed
    static inline float PowerupMoveSpeed = 4.0f;
    // up/down powerup movement distance
    static inline float PowerupMoveDistance = 4.0f;
    // time for powerup respawn afetr picked up
    static inline float PowerupRespawnTime = 5.0f;

    // max ship velocity
    static inline float MovementVelocity = 700;
    // max ship velocity with boost activated
    static inline float MovementVelocityBoost = 1200;
    // force applied by controls to move ship
    static inline float MovementForce = 3000;
    // damping force used to stop movemnt
    static inline float MovementForceDamping = 750;
    // max rotation velocity
    static inline float MovementRotationVelocity = 1.1f;
    // rotation force applied by controls to rotate ship
    static inline float MovementRotationForce = 5.0f;
    // damping force used to stop rotation
    static inline float MovementRotationForceDamping = 3.0f;
};

} // namespace ShipGame
