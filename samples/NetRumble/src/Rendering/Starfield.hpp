// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/IDisposable.hpp"

namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics {
class Effect;
class EffectParameter;
class GraphicsDevice;
class SpriteBatch;
} // namespace Microsoft::Xna::Framework::Graphics

namespace NetRumble {
/** @brief Renders the layered parallax starfield behind the game world. */
class Starfield final : public System::IDisposable {
public:
  /**
   * @brief Constructs a starfield.
   * @param position Initial world position.
   * @param graphicsDevice Rendering device.
   * @param contentManager Content manager.
   */
  Starfield(Microsoft::Xna::Framework::Vector2 position,
            Microsoft::Xna::Framework::Graphics::GraphicsDevice &graphicsDevice,
            Microsoft::Xna::Framework::Content::ContentManager &contentManager);

  /** @brief Releases owned graphics resources. */
  ~Starfield() override;

  /** @brief Loads graphics content. */
  void LoadContent();

  /** @brief Unloads graphics content. */
  void UnloadContent();

  /** @brief Regenerates stars around a new parallax origin. @param position New
   * origin. */
  void Reset(Microsoft::Xna::Framework::Vector2 position);

  /** @brief Updates and draws the starfield. @param position Current world
   * position. */
  void Draw(Microsoft::Xna::Framework::Vector2 position);

  /** @brief Disposes owned graphics resources. */
  void Dispose() override;

private:
  static constexpr int numberOfStars_ = 256;
  static constexpr int numberOfLayers_ = 8;
  static constexpr float maximumMovementPerUpdate_ = 128.0f;
  static constexpr int starSize_ = 2;
  static const std::array<Microsoft::Xna::Framework::Color, numberOfLayers_>
      layerColors_;
  static const std::array<float, numberOfLayers_> movementFactors_;
  static const Microsoft::Xna::Framework::Color backgroundColor_;

  void Dispose(bool disposing);

  Microsoft::Xna::Framework::Vector2 lastPosition_{};
  Microsoft::Xna::Framework::Vector2 position_{};
  std::array<Microsoft::Xna::Framework::Vector2, numberOfStars_> stars_{};
  Microsoft::Xna::Framework::Graphics::GraphicsDevice *graphicsDevice_;
  Microsoft::Xna::Framework::Content::ContentManager *contentManager_;
  std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch>
      spriteBatch_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> starTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> cloudTexture_;
  std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> cloudEffect_;
  Microsoft::Xna::Framework::Graphics::EffectParameter *cloudEffectPosition_{
      nullptr};
  bool disposed_{false};
};
} // namespace NetRumble
