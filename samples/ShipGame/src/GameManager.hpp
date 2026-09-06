// SPDX-License-Identifier: MS-PL
#pragma once

// GameManager.hpp — C++ port of ShipGame/GameManager.cs (XNA 4.0 Ship Game
// Starter Kit). The game's own enumerations and the manager that owns the
// players, the level, the four sub-managers and every draw path.
//
// The C# file is one compilation unit and its types refer to each other freely.
// In C++ the enumerations and the class declaration live here and the bodies in
// GameManager.cpp, which is what lets the graphics and screen headers include
// this one for the enumerations without a cycle.

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace ShipGame {

// supported rendering techniques
enum class RenderTechnique {
    PlainMapping = 0, // plain texture mapping
    NormalMapping,    // normal mapping
    ViewMapping       // view aligned mapping (used for blaster)
};

// game modes
enum class GameMode {
    None = 0,
    SinglePlayer, // single player mode
    MultiPlayer   // multiplayer mode
};

// animated sprites
enum class AnimSpriteType {
    Blaster = 0, // blaster hit
    Missile,     // missile explode
    Ship,        // ship explode
    Spawn,       // ship/object spawn
    Shield       // ship shield
};

// projectiles
enum class ProjectileType {
    Blaster = 0, // blaster projectile
    Missile      // missile projectile
};

// particle systems
enum class ParticleSystemType {
    ShipExplode = 0, // ship explode
    ShipTrail,       // ship trail
    MissileExplode,  // missile explode
    MissileTrail,    // missile trail
    BlasterExplode   // blaster explode
};

// powerup types
enum class PowerupType {
    Energy = 0, // 50% energy
    Missile     // 3 missiles
};

class GameManager;

} // namespace ShipGame
