// SPDX-License-Identifier: MS-PL
#include "ScreenManager/MessageBoxScreen.hpp"

#include <algorithm>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;
static const std::string usageText = "A button = Okay\nB button = Cancel";

MessageBoxScreen::MessageBoxScreen(std::string message)
    : message_(std::move(message)) {
  setIsPopupProperty(true);
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.25));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.25));
}
MessageBoxScreen::MessageBoxScreen(std::string message, bool pauseMenu)
    : MessageBoxScreen(std::move(message)) {
  pauseMenu_ = pauseMenu;
}

void MessageBoxScreen::LoadContent() {
  smallFont_.emplace(
      getScreenManagerProperty().getContentProperty().Load<SpriteFont>(
          "Fonts/MessageBox"));
}

void MessageBoxScreen::HandleInput(InputState &input) {
  const bool aDown = input.CurrentGamePadState.IsButtonDown(Buttons::A);
  if (input.getMenuSelectProperty() && (!pauseMenu_ || aDown)) {
    Accepted.Raise(this, System::EventArgs::Empty);
    ExitScreen();
  } else if (input.getMenuCancelProperty() ||
             (input.getMenuSelectProperty() && pauseMenu_ && !aDown)) {
    Cancelled.Raise(this, System::EventArgs::Empty);
    ExitScreen();
  }
}

void MessageBoxScreen::Draw(const GameTime &) {
  auto &manager = getScreenManagerProperty();
  manager.FadeBackBufferToBlack(getTransitionAlphaProperty() * 2 / 3);
  const Viewport viewport =
      manager.getGraphicsDevicePublicProperty().getViewportProperty();
  const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()),
                             static_cast<float>(viewport.getHeightProperty()));
  const Vector2 textSize = manager.getFontProperty().MeasureString(message_);
  const Vector2 textPosition = (viewportSize - textSize) / 2.0f;
  const Vector2 usageSize = smallFont_->MeasureString(usageText);
  Vector2 usagePosition = (viewportSize - usageSize) / 2.0f;
  usagePosition.Y = textPosition.Y +
                    manager.getFontProperty().getLineSpacingProperty() * 1.1f;
  Rectangle rect(static_cast<int>(std::min(usagePosition.X, textPosition.X)),
                 static_cast<int>(textPosition.Y),
                 static_cast<int>(std::max(usageSize.X, textSize.X)),
                 static_cast<int>(
                     manager.getFontProperty().getLineSpacingProperty() * 1.1f +
                     usageSize.Y));
  rect.X -= static_cast<int>(0.1f * rect.Width);
  rect.Y -= static_cast<int>(0.1f * rect.Height);
  rect.Width += static_cast<int>(0.2f * rect.Width);
  rect.Height += static_cast<int>(0.2f * rect.Height);
  manager.DrawRectangle(
      Rectangle(rect.X - 1, rect.Y - 1, rect.Width + 2, rect.Height + 2),
      Color(128, 128, 128,
            static_cast<SharpRuntime::intcs>(
                192.0f * getTransitionAlphaProperty() / 255.0f)));
  manager.DrawRectangle(
      rect, Color(0, 0, 0,
                  static_cast<SharpRuntime::intcs>(
                      232.0f * getTransitionAlphaProperty() / 255.0f)));
  auto &batch = manager.getSpriteBatchProperty();
  const Color color(
      255, 255, 255,
      static_cast<SharpRuntime::intcs>(getTransitionAlphaProperty()));
  batch.Begin();
  batch.DrawString(manager.getFontProperty(), message_, textPosition, color);
  batch.DrawString(*smallFont_, usageText, usagePosition, color);
  batch.End();
}

const std::string &MessageBoxScreen::GetTypeName() const {
  static const std::string name = "NetRumble.MessageBoxScreen";
  return name;
}
} // namespace NetRumble
