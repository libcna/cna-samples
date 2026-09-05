// SPDX-License-Identifier: MS-PL
#pragma once
#include "BatchRemovalCollection.hpp"
#include "Gameplay/GameplayObject.hpp"
#include "Gameplay/ShipInput.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include <array>
#include <memory>
#include <optional>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace Microsoft::Xna::Framework::Graphics {
class SpriteBatch;
}
namespace Microsoft::Xna::Framework::Net {
class NetworkGamer;
class NetworkSession;
} // namespace Microsoft::Xna::Framework::Net
namespace NetRumble {
class Weapon;
class MineWeapon;
class Projectile;
class ParticleEffectManager;
/** @brief Player-controlled combat ship. */ class Ship final
    : public GameplayObject {
public:
  /** @brief Creates a ship with original radius and mass. */ Ship();
  /** @brief Destroys owned weapons and projectiles. */ ~Ship() override;
  /** @brief Gets score. @return Score. */ [[nodiscard]] int
  getScoreProperty() const {
    return score_;
  }
  /** @brief Sets score. @param v Score. */ void setScoreProperty(int v) {
    score_ = v;
  }
  /** @brief Gets life. @return Life. */ [[nodiscard]] float
  getLifeProperty() const {
    return life_;
  }
  /** @brief Sets life. @param v Life. */ void setLifeProperty(float v) {
    life_ = v;
  }
  /** @brief Gets primary weapon. @return Weapon. */ [[nodiscard]] Weapon *
  getWeaponProperty() const {
    return weapon_.get();
  }
  /** @brief Replaces primary weapon. @param v Weapon. */ void
  setWeaponProperty(std::unique_ptr<Weapon> v);
  /** @brief Gets projectiles. @return Projectile collection. */
  [[nodiscard]] BatchRemovalCollection<std::shared_ptr<Projectile>> &
  getProjectilesProperty() {
    return projectiles_;
  }
  /** @brief Gets shield. @return Shield. */ [[nodiscard]] float
  getShieldProperty() const {
    return shield_;
  }
  /** @brief Sets shield. @param v Shield. */ void setShieldProperty(float v) {
    shield_ = v;
  }
  /** @brief Gets spawn-safety state. @return Safety state. */
  [[nodiscard]] bool getSafeProperty() const {
    return safeTimer_ > 0;
  }
  /** @brief Sets spawn-safety state. @param v Safety state. */ void
  setSafeProperty(bool v) {
    safeTimer_ = v ? safeTimerMaximum_ : 0.0f;
  }
  /** @brief Gets tint. @return Ship color. */ [[nodiscard]] Microsoft::Xna::
      Framework::Color
      getColorProperty() const {
    return color_;
  }
  /** @brief Sets tint. @param v Ship color. */ void
  setColorProperty(Microsoft::Xna::Framework::Color v) {
    color_ = v;
  }
  /** @brief Gets respawn delay. @return Seconds. */ [[nodiscard]] float
  getRespawnTimerProperty() const {
    return respawnTimer_;
  }
  /** @brief Sets respawn delay. @param v Seconds. */ void
  setRespawnTimerProperty(float v) {
    respawnTimer_ = v;
  }
  /** @brief Gets last damaging object. @return Source. */
  [[nodiscard]] GameplayObject *getLastDamagedByProperty() const {
    return lastDamagedBy_;
  }
  /** @brief Gets current input. @return Input. */ [[nodiscard]] ShipInput
  getShipInputProperty() const {
    return shipInput_;
  }
  /** @brief Sets current input. @param v Input. */ void
  setShipInputProperty(ShipInput v) {
    shipInput_ = v;
  }
  /** @brief Gets visual variation. @return Variation. */ [[nodiscard]] int
  getVariationProperty() const {
    return variation_;
  }
  /** @brief Sets visual variation. @param v Variation. */ void
  setVariationProperty(int v);
  /** @brief Spawns or respawns the ship. */ void Initialize() override;
  /** @brief Updates movement, weapons, shields and projectiles. @param
   * elapsedTime Elapsed seconds. */
  void
  Update(float elapsedTime) override;
  /** @brief Draws ship, shield and projectiles. @param elapsedTime Elapsed
   * seconds. @param spriteBatch Batch. */
  void
  Draw(float elapsedTime,
       Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch);
  /** @brief Applies damage. @param source Damage source. @param damageAmount
   * Damage. @return True if applied. */
  bool
  Damage(GameplayObject *source, float damageAmount) override;
  /** @brief Kills and scores the ship. @param source Cause. @param cleanupOnly
   * Suppress effects. */
  void
  Die(GameplayObject *source, bool cleanupOnly) override;
  /** @brief Loads static ship textures. @param contentManager Content manager.
   */
  static void
  LoadContent(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);
  /** @brief Releases static textures. */ static void UnloadContent();
  /** @brief Tests color uniqueness in a session. @param networkGamer Gamer.
   * @param networkSession Session. @return True when unique. */
  static bool
  HasUniqueColorIndex(
      Microsoft::Xna::Framework::Net::NetworkGamer *networkGamer,
      Microsoft::Xna::Framework::Net::NetworkSession *networkSession);
  /** @brief Finds next unused color. @param currentColorIndex Current index.
   * @param networkSession Session. @return New index. */
  static SharpRuntime::bytecs
  GetNextUniqueColorIndex(
      SharpRuntime::bytecs currentColorIndex,
      Microsoft::Xna::Framework::Net::NetworkSession *networkSession);
  /** @brief Finds previous unused color. @param currentColorIndex Current
   * index. @param networkSession Session. @return New index. */
  static SharpRuntime::bytecs
  GetPreviousUniqueColorIndex(
      SharpRuntime::bytecs currentColorIndex,
      Microsoft::Xna::Framework::Net::NetworkSession *networkSession);
  /** @brief Gets variation count. @return Four. */ static int
  getVariationsProperty() {
    return variations_;
  }
  /** @brief Colors available to network players. */
  static std::array<Microsoft::Xna::Framework::Color, 18> ShipColors;
  /** @brief Particle manager that receives ship effects. */
  static ParticleEffectManager *ParticleEffectManagerInstance;

private:
  static constexpr float fullSpeed_ = 320.0f;
  static constexpr float dragPerSecond_ = 0.9f;
  static constexpr float fireThresholdSquared_ = 0.25f;
  static constexpr float rotationRadiansPerSecond_ = 6.0f;
  static constexpr float velocityMaximum_ = 320.0f;
  static constexpr float shieldMaximum_ = 100.0f;
  static constexpr float shieldAlphaMaximum_ = 150.0f;
  static constexpr float shieldRechargePerSecond_ = 50.0f;
  static constexpr float shieldRechargeTimerMaximum_ = 2.5f;
  static constexpr float shieldScaleBase_ = 1.2f;
  static constexpr float shieldPulseAmplitude_ = 0.15f;
  static constexpr float shieldPulseRate_ = 0.2f;
  static constexpr float safeTimerMaximum_ = 4.0f;
  static constexpr float lifeMaximum_ = 25.0f;
  static constexpr float respawnTimerOnDeath_ = 5.0f;
  static constexpr int variations_ = 4;
  static std::array<
      std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>,
      variations_>
      primaryTextures_, overlayTextures_;
  static std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      shieldTexture_;
  int score_ = 0;
  float life_ = 0;
  std::unique_ptr<Weapon> weapon_;
  std::unique_ptr<MineWeapon> mineWeapon_;
  BatchRemovalCollection<std::shared_ptr<Projectile>> projectiles_;
  float shield_ = 0, shieldRechargeTimer_ = 0, safeTimer_ = 0;
  Microsoft::Xna::Framework::Color color_ =
      Microsoft::Xna::Framework::Color::White;
  float respawnTimer_ = 0;
  GameplayObject *lastDamagedBy_ = nullptr;
  ShipInput shipInput_;
  int variation_ = 0;
  float shieldPulseTime_ = 0;
};
} // namespace NetRumble
