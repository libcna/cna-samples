// SPDX-License-Identifier: MS-PL
#include "Screens/NetworkBusyScreen.hpp"

#include <algorithm>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "System/IAsyncResult.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

NetworkBusyScreen::NetworkBusyScreen(std::string message,
                                     System::IAsyncResult *asyncResult)
    : message_(std::move(message)), asyncResult_(asyncResult) {
  setIsPopupProperty(true);
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.1));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.2));
}

void NetworkBusyScreen::LoadContent() {
  busyTexture_.emplace(getScreenManagerProperty()
                           .getGamePublicProperty()
                           .getContentProperty()
                           .Load<Texture2D>("Textures/chatTalking"));
}

void NetworkBusyScreen::Update(GameTime &gameTime, bool otherScreenHasFocus,
                               bool coveredByOtherScreen) {
  GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
  if (asyncResult_ != nullptr && asyncResult_->getIsCompletedProperty()) {
    OperationCompletedEventArgs args(asyncResult_);
    OperationCompleted.Raise(this, args);
    ExitScreen();
    asyncResult_ = nullptr;
  }
}

void NetworkBusyScreen::Draw(const GameTime &gameTime) {
  ScreenManager &screenManager = getScreenManagerProperty();
  SpriteBatch &spriteBatch = screenManager.getSpriteBatchProperty();
  SpriteFont &font = screenManager.getFontProperty();

  constexpr int hPad = 32;
  constexpr int vPad = 16;
  const Viewport viewport =
      screenManager.getGraphicsDevicePublicProperty().getViewportProperty();
  const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()),
                             static_cast<float>(viewport.getHeightProperty()));
  Vector2 textSize = font.MeasureString(message_);
  const Vector2 busyTextureSize(busyTexture_->getWidthProperty() *
                                busyTextureScale_);
  const Vector2 busyTextureOrigin(busyTexture_->getWidthProperty() / 2.0f,
                                  busyTexture_->getHeightProperty() / 2.0f);

  textSize.X = std::max(textSize.X, busyTextureSize.X);
  textSize.Y += busyTextureSize.Y + vPad;
  const Vector2 textPosition = (viewportSize - textSize) / 2.0f;
  const Rectangle backgroundRectangle(
      static_cast<int>(textPosition.X) - hPad,
      static_cast<int>(textPosition.Y) - vPad,
      static_cast<int>(textSize.X) + hPad * 2,
      static_cast<int>(textSize.Y) + vPad * 2);
  const Color color(
      255, 255, 255,
      static_cast<SharpRuntime::intcs>(getTransitionAlphaProperty()));
  const Rectangle backgroundRectangle2(
      backgroundRectangle.X - 1, backgroundRectangle.Y - 1,
      backgroundRectangle.Width + 2, backgroundRectangle.Height + 2);

  screenManager.DrawRectangle(
      backgroundRectangle2,
      Color(128, 128, 128,
            static_cast<SharpRuntime::intcs>(
                192.0f * getTransitionAlphaProperty() / 255.0f)));
  screenManager.DrawRectangle(
      backgroundRectangle,
      Color(0, 0, 0,
            static_cast<SharpRuntime::intcs>(
                232.0f * getTransitionAlphaProperty() / 255.0f)));

  spriteBatch.Begin();
  spriteBatch.DrawString(font, message_, textPosition, color);
  const float busyTextureRotation =
      static_cast<float>(
          gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()) *
      3.0f;
  const Vector2 busyTexturePosition(textPosition.X + textSize.X / 2.0f,
                                    textPosition.Y + textSize.Y -
                                        busyTextureSize.Y / 2.0f);
  spriteBatch.Draw(*busyTexture_, busyTexturePosition, std::nullopt, color,
                   busyTextureRotation, busyTextureOrigin, busyTextureScale_,
                   SpriteEffects::None, 0.0f);
  spriteBatch.End();
}

const std::string &NetworkBusyScreen::GetTypeName() const {
  static const std::string name = "NetRumble.NetworkBusyScreen";
  return name;
}
} // namespace NetRumble
