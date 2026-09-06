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
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "System/IDisposable.hpp"

namespace BoxCollider {
class CollisionMesh;
}

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;

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

// DrawMode belongs to AnimSpriteManager.cs, and that header needs the enumerations above, so the
// two cannot both be definitions in one order. A scoped enumeration has a fixed underlying type,
// which makes this opaque declaration legal and identical to the definition in
// Graphics/AnimSpriteManager.hpp.
enum class DrawMode : int;

class AnimSprite;
class AnimSpriteManager;
class EntityList;
class FontManager;
class InputManager;
class LightList;
class ParticleManager;
class ParticleSystem;
class PlayerShip;
class Powerup;
class PowerupManager;
class Projectile;
class ProjectileManager;

// Port of the GameManager class in ShipGame/GameManager.cs.
class GameManager : public System::IDisposable {
public:
    // Create a new game manager
    explicit GameManager(Microsoft::Xna::Framework::Audio::SoundBank* soundBank);

    ~GameManager() override;

    // Get the game mode
    GameMode getGameMode() const { return gameMode_; }
    void setGameMode(GameMode value) { gameMode_ = value; }

    // Set the game level
    void SetLevel(const std::string& levelFileName);

    // Set the player ships and invert Y otions
    void SetShips(const std::string& shipPlayer1, const std::string& shipPlayer2,
                  std::uint32_t invertYAxis);

    // Check if invert Y otions is enabled for a given player
    bool GetInvertY(int player) const;

    // Get file name for the ship selected by a given player
    std::string GetPlayerShip(int player) const;

    // Get the winner player on a multiplayer match
    int getPlayerWinner() const;

    // Load the game files (level, ships, wepons, etc...)
    void LoadFiles(Microsoft::Xna::Framework::Content::ContentManager& content);

    // Unload game files
    void UnloadFiles();

    // Play a sound in 2D
    void PlaySound(const std::string& soundName);

    // Play a sound in 3D at given position
    // (just fake 3D using distance attenuation but no stereo)
    void PlaySound3D(const std::string& soundName, Vector3 position);

    // Add vibration to the gamepad of the given player
    void SetVibration(int player, float duration);

    // Add damage to all players inside a splash sphere with distance attenuation
    void AddDamageSplash(int attacker, float damage, Vector3 position, float radius);

    // Add damage to a player and check for player kill
    void AddDamage(int attacker, int defender, float damage, Vector3 pushDirection);

    // Process game input
    void ProcessInput(float elapsedTime, InputManager* input);

    // Update game for given elapsed time
    void Update(float elapsedTime);

    // Draw the 3D game screen
    void Draw3D(Microsoft::Xna::Framework::Graphics::GraphicsDevice* gd);

    // Draw the 2D game screen
    void Draw2D(FontManager* font);

    // Load content
    void LoadContent(Microsoft::Xna::Framework::Graphics::GraphicsDevice* gd,
                     Microsoft::Xna::Framework::Content::ContentManager& content);

    // Unload content
    void UnloadContent();

    // Get index for player at given 3D position
    // (returns -1 if no player at that position)
    int GetPlayerAtPosition(Vector3 position);

    // Get the PlayerShip object for given player id)
    PlayerShip* GetPlayer(int playerId);

    // Create a new particle system and add it to the particle system manager
    std::shared_ptr<ParticleSystem> AddParticleSystem(ParticleSystemType type, Matrix transform);

    // Create a new animated sprite and add it to the animated sprite manager
    std::shared_ptr<AnimSprite> AddAnimSprite(AnimSpriteType type, Vector3 position, float radius,
                                              float viewOffset, float frameRate, DrawMode mode,
                                              int player);

    // Create a new powerup and add it to the powerup manager
    std::shared_ptr<Powerup> AddPowerup(PowerupType type, Matrix transform);

    // Create a new projectile and add it to the projectile manager
    std::shared_ptr<Projectile> AddProjectile(ProjectileType type, int player, Matrix transform,
                                              float velocity, float damage,
                                              RenderTechnique technique);

    // Draw a projectile
    void DrawProjectile(Microsoft::Xna::Framework::Graphics::GraphicsDevice* gd, ProjectileType p,
                        RenderTechnique technique, Vector3 cameraPosition, Matrix world,
                        Matrix viewProjection, LightList* lights);

    // Draw a model using given technique and camera settings
    void DrawModel(Microsoft::Xna::Framework::Graphics::GraphicsDevice* gd,
                   Microsoft::Xna::Framework::Graphics::Model* model, RenderTechnique technique,
                   Vector3 cameraPosition, Matrix world, Matrix viewProjection, LightList* lights);

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override;

private:
    void Dispose(bool disposing);

    // Draw the HUD interface
    void DrawHud(FontManager* font, Microsoft::Xna::Framework::Rectangle rect, Vector3 bars,
                 int barsLeft, int barsWidth, bool crosshair);

    // Draw the 3D game scene
    void DrawScene(Microsoft::Xna::Framework::Graphics::GraphicsDevice* gd,
                   RenderTechnique technique);

    bool isDisposed_ = false;

    std::unique_ptr<AnimSpriteManager> animatedSprite_;      // animated sprite manager
    std::unique_ptr<ProjectileManager> projectile_;          // projectile manager
    std::unique_ptr<ParticleManager> particle_;              // particle manager
    std::unique_ptr<PowerupManager> powerup_;                // powerup manager
    Microsoft::Xna::Framework::Audio::SoundBank* sound_ = nullptr; // sound manager

    GameMode gameMode_ = GameMode::SinglePlayer; // current game mode

    // ship file names for each player (selected and set from player screen)
    std::vector<std::string> shipFile_;

    std::uint32_t invertY_ = 0; // bit array for mouse invert Y
                                // (selected and set from player screen)

    std::string levelFile_; // current level file name
                            // (selected and set from level screen)

    // the player objects
    std::vector<std::shared_ptr<PlayerShip>> players_;

    // pad vibration times for each player (zero for no vibration)
    std::vector<float> vibrationTime_;

    Microsoft::Xna::Framework::Graphics::Viewport viewportLeft_;  // left split screen viewport
    Microsoft::Xna::Framework::Graphics::Viewport viewportRight_; // right split screen viewport

    Matrix projectionFull_;  // full screen projection matrix
    Matrix projectionSplit_; // split screen projection matrix

    std::optional<Microsoft::Xna::Framework::Graphics::Model> levelColor_; // level model
    std::shared_ptr<EntityList> levelSpawns_;                              // level spawn points
    std::shared_ptr<LightList> levelLights_;                               // level lights
    std::shared_ptr<BoxCollider::CollisionMesh> levelCollision_;           // level collision model

    // particle texture files (matches ParticleSystemType)
    std::vector<std::string> particleFiles_{"Spark1", "Point1", "Spark2", "Point1", "Point2"};
    std::vector<std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>> particleTextures_;

    // animated sprite texture files (matches AnimSpriteType)
    std::vector<std::string> animatedSpriteFiles_{"BlasterGrid_16", "MissileGrid_16",
                                                  "ShipGrid_32", "SpawnGrid_16", "ShieldGrid_32"};
    std::vector<std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>>
        animatedSpriteTextures_;

    // projectile modell files (matches ProjectileType)
    std::vector<std::string> projectileFiles_{"blaster", "missile"};
    std::vector<std::optional<Microsoft::Xna::Framework::Graphics::Model>> projectileModels_;

    // powerup model files (matches PowerupType)
    std::vector<std::string> powerupFiles_{"energy", "missile"};
    std::vector<std::optional<Microsoft::Xna::Framework::Graphics::Model>> powerupModels_;

    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> hudCrosshair_; // hud crosshair texture
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> hudEnergy_;    // hud energy/shield/boost texture
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> hudMissile_;   // hud missile texture
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> hudScore_;     // hud score texture
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> hudBars_;      // hud energy/shield/boost bars texture

    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> damageTexture_; // damage indication texture

    // list of currently playing 3D sounds
    std::vector<std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue>> cueSounds_;
    // 3D sounds finished and ready to delete
    std::vector<Microsoft::Xna::Framework::Audio::Cue*> cueSoundsDelete_;

    // global bone array used by DrawModel method
    std::vector<Matrix> bones_;
};

} // namespace ShipGame
