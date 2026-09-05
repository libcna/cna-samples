// SPDX-License-Identifier: MS-PL
#include "ScreenManager/InputState.hpp"

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Input;

bool InputState::IsNewKeyPress(Keys key) const {
  return CurrentKeyboardState.IsKeyDown(key) && LastKeyboardState.IsKeyUp(key);
}

bool InputState::getMenuUpProperty() const {
  return IsNewKeyPress(Keys::Up) ||
         (CurrentGamePadState.IsButtonDown(Buttons::DPadUp) &&
          LastGamePadState.IsButtonUp(Buttons::DPadUp)) ||
         (CurrentGamePadState.IsButtonDown(Buttons::LeftThumbstickUp) &&
          LastGamePadState.IsButtonUp(Buttons::LeftThumbstickUp));
}
bool InputState::getMenuDownProperty() const {
  return IsNewKeyPress(Keys::Down) ||
         (CurrentGamePadState.IsButtonDown(Buttons::DPadDown) &&
          LastGamePadState.IsButtonUp(Buttons::DPadDown)) ||
         (CurrentGamePadState.IsButtonDown(Buttons::LeftThumbstickDown) &&
          LastGamePadState.IsButtonUp(Buttons::LeftThumbstickDown));
}
bool InputState::getMenuSelectProperty() const {
  return IsNewKeyPress(Keys::Space) || IsNewKeyPress(Keys::Enter) ||
         (CurrentGamePadState.IsButtonDown(Buttons::A) &&
          LastGamePadState.IsButtonUp(Buttons::A)) ||
         (CurrentGamePadState.IsButtonDown(Buttons::Start) &&
          LastGamePadState.IsButtonUp(Buttons::Start));
}
bool InputState::getMenuCancelProperty() const {
  return IsNewKeyPress(Keys::Escape) ||
         (CurrentGamePadState.IsButtonDown(Buttons::B) &&
          LastGamePadState.IsButtonUp(Buttons::B)) ||
         (CurrentGamePadState.IsButtonDown(Buttons::Back) &&
          LastGamePadState.IsButtonUp(Buttons::Back));
}
bool InputState::getPauseGameProperty() const {
  return IsNewKeyPress(Keys::Escape) ||
         (CurrentGamePadState.IsButtonDown(Buttons::Back) &&
          LastGamePadState.IsButtonUp(Buttons::Back)) ||
         (CurrentGamePadState.IsButtonDown(Buttons::Start) &&
          LastGamePadState.IsButtonUp(Buttons::Start));
}
bool InputState::getShipColorChangeUpProperty() const {
  return IsNewKeyPress(Keys::Up) ||
         (CurrentGamePadState.IsButtonDown(Buttons::RightShoulder) &&
          LastGamePadState.IsButtonUp(Buttons::RightShoulder));
}
bool InputState::getShipColorChangeDownProperty() const {
  return IsNewKeyPress(Keys::Down) ||
         (CurrentGamePadState.IsButtonDown(Buttons::LeftShoulder) &&
          LastGamePadState.IsButtonUp(Buttons::LeftShoulder));
}
bool InputState::getShipModelChangeUpProperty() const {
  return IsNewKeyPress(Keys::Right) ||
         (CurrentGamePadState.getTriggersProperty().getRightProperty() >=
              1.0f &&
          LastGamePadState.getTriggersProperty().getRightProperty() < 1.0f);
}
bool InputState::getShipModelChangeDownProperty() const {
  return IsNewKeyPress(Keys::Left) ||
         (CurrentGamePadState.getTriggersProperty().getLeftProperty() >= 1.0f &&
          LastGamePadState.getTriggersProperty().getLeftProperty() < 1.0f);
}
bool InputState::getMarkReadyProperty() const {
  return IsNewKeyPress(Keys::X) ||
         (CurrentGamePadState.IsButtonDown(Buttons::X) &&
          LastGamePadState.IsButtonUp(Buttons::X));
}

void InputState::Update() {
  LastKeyboardState = CurrentKeyboardState;
  LastGamePadState = CurrentGamePadState;
  CurrentKeyboardState = Keyboard::GetState();
  CurrentGamePadState = GamePad::GetState(PlayerIndex::One);
}
} // namespace NetRumble
