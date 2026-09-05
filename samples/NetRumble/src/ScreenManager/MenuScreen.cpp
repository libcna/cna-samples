// SPDX-License-Identifier: MS-PL
#include "ScreenManager/MenuScreen.hpp"

#include <cmath>

#include "AudioManager.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

MenuScreen::MenuScreen() {
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.0));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(1.0));
}

std::vector<std::string> &MenuScreen::getMenuEntriesProperty() {
  return menuEntries_;
}

void MenuScreen::HandleInput(InputState &input) {
  if (input.getMenuUpProperty()) {
    if (--selectedEntry_ < 0)
      selectedEntry_ = static_cast<int>(menuEntries_.size()) - 1;
    AudioManager::PlaySoundEffect("menu_scroll");
  }
  if (input.getMenuDownProperty()) {
    if (++selectedEntry_ >= static_cast<int>(menuEntries_.size()))
      selectedEntry_ = 0;
    AudioManager::PlaySoundEffect("menu_scroll");
  }
  if (input.getMenuSelectProperty()) {
    AudioManager::PlaySoundEffect("menu_select");
    OnSelectEntry(selectedEntry_);
  } else if (input.getMenuCancelProperty())
    OnCancel();
}

void MenuScreen::Draw(const GameTime &gameTime) {
  auto &manager = getScreenManagerProperty();
  const Viewport viewport =
      manager.getGraphicsDevicePublicProperty().getViewportProperty();
  const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()),
                             static_cast<float>(viewport.getHeightProperty()));
  Vector2 position(0.0f, viewportSize.Y * 0.65f);
  const float transitionOffset =
      std::pow(getTransitionPositionProperty(), 2.0f);
  position.Y +=
      transitionOffset *
      (getScreenStateProperty() == ScreenState::TransitionOn ? 256.0f : 512.0f);
  auto &batch = manager.getSpriteBatchProperty();
  auto &font = manager.getFontProperty();
  batch.Begin();
  for (int i = 0; i < static_cast<int>(menuEntries_.size()); ++i) {
    Color color = Color::White;
    float scale = 1.0f;
    if (getIsActiveProperty() && i == selectedEntry_) {
      const float pulsate =
          static_cast<float>(std::sin(
              gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() *
              6.0)) +
          1.0f;
      color = Color::Orange;
      scale += pulsate * 0.05f;
    }
    color = Color(static_cast<SharpRuntime::intcs>(color.getRProperty()),
                  static_cast<SharpRuntime::intcs>(color.getGProperty()),
                  static_cast<SharpRuntime::intcs>(color.getBProperty()),
                  static_cast<SharpRuntime::intcs>(
                      getTransitionAlphaProperty()));
    const Vector2 origin(0.0f, font.getLineSpacingProperty() / 2.0f);
    const Vector2 size = font.MeasureString(menuEntries_[i]);
    position.X = viewportSize.X / 2.0f - size.X / 2.0f * scale;
    batch.DrawString(font, menuEntries_[i], position, color, 0.0f, origin,
                     scale, SpriteEffects::None, 0.0f);
    position.Y += static_cast<float>(font.getLineSpacingProperty());
  }
  batch.End();
}
} // namespace NetRumble
