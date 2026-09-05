// SPDX-License-Identifier: MS-PL
#include "Screens/BackgroundScreen.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Rendering/Starfield.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

BackgroundScreen::BackgroundScreen() {
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.0));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(1.0));
}

BackgroundScreen::~BackgroundScreen() { Dispose(false); }

void BackgroundScreen::LoadContent() {
  ScreenManager &screenManager = getScreenManagerProperty();
  titleTexture_.emplace(
      screenManager.getContentProperty().Load<Texture2D>("Textures/title"));
  movement_ = 0.0;
  const Vector2 position =
      Vector2::Multiply(
          Vector2(static_cast<float>(
                      std::cos(movement_ / starsParallaxPeriod_)),
                  static_cast<float>(
                      std::sin(movement_ / starsParallaxPeriod_))),
          starsParallaxAmplitude_);
  starfield_ = std::make_unique<Starfield>(
      position, screenManager.getGraphicsDevicePublicProperty(),
      screenManager.getContentProperty());
  starfield_->LoadContent();
  GameScreen::LoadContent();
}

void BackgroundScreen::UnloadContent() {
  if (starfield_ != nullptr) {
    starfield_->UnloadContent();
    starfield_.reset();
  }
  GameScreen::UnloadContent();
}

void BackgroundScreen::Update(GameTime &gameTime, bool otherScreenHasFocus,
                              bool) {
  GameScreen::Update(gameTime, otherScreenHasFocus, false);
}

void BackgroundScreen::Draw(const GameTime &gameTime) {
  ScreenManager &screenManager = getScreenManagerProperty();
  if (starfield_ != nullptr) {
    movement_ +=
        gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    const Vector2 position = Vector2::Multiply(
        Vector2(static_cast<float>(
                    std::cos(movement_ / starsParallaxPeriod_)),
                static_cast<float>(
                    std::sin(movement_ / starsParallaxPeriod_))),
        starsParallaxAmplitude_);
    starfield_->Draw(position);
  }

  if (titleTexture_.has_value()) {
    const Rectangle titleSafeArea = screenManager.getTitleSafeAreaProperty();
    Vector2 titlePosition(
        titleSafeArea.X +
            (titleSafeArea.Width - titleTexture_->getWidthProperty()) / 2.0f,
        titleSafeArea.Y + titleSafeArea.Height * 0.05f);
    titlePosition.Y -=
        std::pow(getTransitionPositionProperty(), 2.0f) * titlePosition.Y;

    SpriteBatch &spriteBatch = screenManager.getSpriteBatchProperty();
    spriteBatch.Begin();
    spriteBatch.Draw(
        *titleTexture_, titlePosition,
        Color(255, 255, 255,
              static_cast<SharpRuntime::intcs>(getTransitionAlphaProperty())));
    spriteBatch.End();
  }
}

void BackgroundScreen::Dispose() { Dispose(true); }

void BackgroundScreen::Dispose(bool disposing) {
  if (!disposed_ && disposing && starfield_ != nullptr) {
    starfield_->Dispose();
    starfield_.reset();
  }
  disposed_ = true;
}

const std::string &BackgroundScreen::GetTypeName() const {
  static const std::string name = "NetRumble.BackgroundScreen";
  return name;
}
} // namespace NetRumble
