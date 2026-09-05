// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "ScreenManager/InputState.hpp"

namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace Microsoft::Xna::Framework::GamerServices {
class SignedInGamer;
}

namespace NetRumble {
/** @brief Owns NetRumble's screen stack and shared menu drawing resources. */
class ScreenManager final
    : public Microsoft::Xna::Framework::DrawableGameComponent {
public:
  /** @brief Creates a manager for the game. @param game Owning game. */
  explicit ScreenManager(Microsoft::Xna::Framework::Game &game);
  /** @brief Gets the owning game. @return Owning game. */
  [[nodiscard]] Microsoft::Xna::Framework::Game &getGamePublicProperty();
  /** @brief Gets the graphics device. @return Graphics device. */
  [[nodiscard]] Microsoft::Xna::Framework::Graphics::GraphicsDevice &
  getGraphicsDevicePublicProperty();
  /** @brief Gets the manager-owned content manager. @return Content manager. */
  [[nodiscard]] Microsoft::Xna::Framework::Content::ContentManager &
  getContentProperty();
  /** @brief Gets the shared sprite batch. @return Sprite batch. */
  [[nodiscard]] Microsoft::Xna::Framework::Graphics::SpriteBatch &
  getSpriteBatchProperty();
  /** @brief Gets the menu font. @return Menu font. */
  [[nodiscard]] Microsoft::Xna::Framework::Graphics::SpriteFont &
  getFontProperty();
  /** @brief Gets tracing state. @return True when tracing. */
  [[nodiscard]] bool getTraceEnabledProperty() const;
  /** @brief Sets tracing state. @param value New tracing state. */
  void setTraceEnabledProperty(bool value);
  /** @brief Gets the calculated title-safe area. @return Safe rectangle. */
  [[nodiscard]] Microsoft::Xna::Framework::Rectangle
  getTitleSafeAreaProperty() const;
  /** @brief Adds a screen to the top of the stack. @param screen Screen to own.
   */
  void AddScreen(std::shared_ptr<GameScreen> screen);
  /** @brief Removes a screen immediately. @param screen Screen to remove. */
  void RemoveScreen(GameScreen *screen);
  /** @brief Returns a snapshot of the stack. @return Current screens. */
  [[nodiscard]] std::vector<std::shared_ptr<GameScreen>> GetScreens() const;
  /** @brief Draws a solid rectangle with the shared blank texture. @param
   * rectangle Destination. @param color Fill color. */
  void DrawRectangle(const Microsoft::Xna::Framework::Rectangle &rectangle,
                     const Microsoft::Xna::Framework::Color &color);
  /** @brief Draws a black fullscreen fade. @param alpha Alpha from zero through
   * 255. */
  void FadeBackBufferToBlack(int alpha);
  /** @brief Updates all screens and routes input. @param gameTime Timing
   * snapshot. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime) override;
  /** @brief Draws all visible screens. @param gameTime Timing snapshot. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;
  /** @brief Returns logical runtime type name. @return Fully qualified type
   * name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

  /** @brief Gamer associated with the most recently accepted invitation. */
  Microsoft::Xna::Framework::GamerServices::SignedInGamer *invited = nullptr;

protected:
  /** @brief Loads shared screen resources. */
  void LoadContent() override;
  /** @brief Unloads shared screen resources. */
  void UnloadContent() override;

private:
  static void EraseByPointer(std::vector<std::shared_ptr<GameScreen>> &screens,
                             GameScreen *screen);
  void TraceScreens() const;
  std::vector<std::shared_ptr<GameScreen>> screens_;
  std::vector<std::shared_ptr<GameScreen>> screensToUpdate_;
  InputState input_;
  std::unique_ptr<Microsoft::Xna::Framework::Content::ContentManager> content_;
  std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch>
      spriteBatch_;
  std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> blankTexture_;
  Microsoft::Xna::Framework::Rectangle titleSafeArea_;
  bool traceEnabled_ = false;
  bool contentLoaded_ = false;
};
} // namespace NetRumble
