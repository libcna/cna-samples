// SPDX-License-Identifier: MS-PL
#include "ScreenManager/LoadingScreen.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

LoadingScreen::LoadingScreen() {
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.5));
}

void LoadingScreen::Load(ScreenManager &manager, LoadHandler handler,
                         bool slow) {
  for (const auto &screen : manager.GetScreens())
    screen->ExitScreen();
  auto loading = std::shared_ptr<LoadingScreen>(new LoadingScreen());
  loading->loadingIsSlow_ = slow;
  loading->loadNextScreen_ = std::move(handler);
  manager.AddScreen(loading);
}

void LoadingScreen::Update(GameTime &gameTime, bool focus, bool covered) {
  GameScreen::Update(gameTime, focus, covered);
  if (otherScreensAreGone_) {
    getScreenManagerProperty().RemoveScreen(this);
    loadNextScreen_(this, System::EventArgs::Empty);
  }
}

void LoadingScreen::Draw(const GameTime &) {
  auto &manager = getScreenManagerProperty();
  if (getScreenStateProperty() == ScreenState::Active &&
      manager.GetScreens().size() == 1)
    otherScreensAreGone_ = true;
  if (!loadingIsSlow_)
    return;
  const std::string message = "Loading...";
  const Viewport viewport =
      manager.getGraphicsDevicePublicProperty().getViewportProperty();
  const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()),
                             static_cast<float>(viewport.getHeightProperty()));
  const Vector2 position =
      (viewportSize - manager.getFontProperty().MeasureString(message)) / 2.0f;
  manager.getSpriteBatchProperty().Begin();
  manager.getSpriteBatchProperty().DrawString(
      manager.getFontProperty(), message, position,
      Color(255, 255, 255,
            static_cast<SharpRuntime::intcs>(getTransitionAlphaProperty())));
  manager.getSpriteBatchProperty().End();
}

const std::string &LoadingScreen::GetTypeName() const {
  static const std::string name = "NetRumble.LoadingScreen";
  return name;
}
} // namespace NetRumble
