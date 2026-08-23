#pragma once

#include <cmath>

#include <Microsoft/Xna/Framework/GameTime.hpp>
#include <Microsoft/Xna/Framework/Input/GamePadState.hpp>
#include <Microsoft/Xna/Framework/Input/KeyboardState.hpp>
#include <Microsoft/Xna/Framework/Input/Keys.hpp>
#include <Microsoft/Xna/Framework/MathHelper.hpp>
#include <Microsoft/Xna/Framework/Matrix.hpp>
#include <Microsoft/Xna/Framework/Quaternion.hpp>
#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace TexturesAndColorsSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Input;

enum class SampleArcBallCameraMode {
  Free = 0,
  RollConstrained = 1,
};

class SampleArcBallCamera {
public:
  static float ReadKeyboardAxis(const KeyboardState &keyState, Keys downKey,
                                Keys upKey) {
    float value = 0.0f;

    if (keyState.IsKeyDown(downKey)) {
      value -= 1.0f;
    }
    if (keyState.IsKeyDown(upKey)) {
      value += 1.0f;
    }

    return value;
  }

  explicit SampleArcBallCamera(SampleArcBallCameraMode controlMode)
      : orientation_(
            Quaternion::CreateFromAxisAngle(Vector3::Up, MathHelper::Pi)),
        inputDistanceRateValue_(4.0f), mode_(controlMode),
        yaw_(MathHelper::Pi) {}

  [[nodiscard]] Vector3 getDirectionProperty() const {
    Vector3 direction = Vector3::Zero;
    direction.X = -2.0f * ((orientation_.X * orientation_.Z) +
                           (orientation_.W * orientation_.Y));
    direction.Y = 2.0f * ((orientation_.W * orientation_.X) -
                          (orientation_.Y * orientation_.Z));
    direction.Z =
        ((orientation_.X * orientation_.X) +
         (orientation_.Y * orientation_.Y)) -
        ((orientation_.Z * orientation_.Z) + (orientation_.W * orientation_.W));
    return Vector3::Normalize(direction);
  }

  [[nodiscard]] Vector3 getRightProperty() const {
    Vector3 right = Vector3::Zero;
    right.X =
        ((orientation_.X * orientation_.X) +
         (orientation_.W * orientation_.W)) -
        ((orientation_.Z * orientation_.Z) + (orientation_.Y * orientation_.Y));
    right.Y = 2.0f * ((orientation_.X * orientation_.Y) +
                      (orientation_.Z * orientation_.W));
    right.Z = 2.0f * ((orientation_.X * orientation_.Z) -
                      (orientation_.Y * orientation_.W));
    return right;
  }

  [[nodiscard]] Vector3 getUpProperty() const {
    Vector3 up = Vector3::Zero;
    up.X = 2.0f * ((orientation_.X * orientation_.Y) -
                   (orientation_.Z * orientation_.W));
    up.Y =
        ((orientation_.Y * orientation_.Y) +
         (orientation_.W * orientation_.W)) -
        ((orientation_.Z * orientation_.Z) + (orientation_.X * orientation_.X));
    up.Z = 2.0f * ((orientation_.Y * orientation_.Z) +
                   (orientation_.X * orientation_.W));
    return up;
  }

  [[nodiscard]] Matrix getViewMatrixProperty() const {
    return Matrix::CreateLookAt(targetValue_ -
                                    (getDirectionProperty() * distanceValue_),
                                targetValue_, getUpProperty());
  }

  [[nodiscard]] SampleArcBallCameraMode getControlModeProperty() const {
    return mode_;
  }

  void setControlModeProperty(SampleArcBallCameraMode value) {
    if (value != mode_) {
      mode_ = value;
      SetCamera(targetValue_ - (getDirectionProperty() * distanceValue_),
                targetValue_, Vector3::Up);
    }
  }

  [[nodiscard]] Vector3 getTargetProperty() const { return targetValue_; }

  void setTargetProperty(const Vector3 &value) { targetValue_ = value; }

  [[nodiscard]] float getDistanceProperty() const { return distanceValue_; }

  void setDistanceProperty(float value) { distanceValue_ = value; }

  [[nodiscard]] float getInputDistanceRateProperty() const {
    return inputDistanceRateValue_;
  }

  void setInputDistanceRateProperty(float value) {
    inputDistanceRateValue_ = value;
  }

  [[nodiscard]] Vector3 getPositionProperty() const {
    return targetValue_ - (getDirectionProperty() * getDistanceProperty());
  }

  void setPositionProperty(const Vector3 &value) {
    SetCamera(value, targetValue_, Vector3::Up);
  }

  void OrbitUp(float angle) {
    switch (mode_) {
    case SampleArcBallCameraMode::Free:
      orientation_ = orientation_ *
                     Quaternion::CreateFromAxisAngle(Vector3::Right, -angle);
      orientation_ = Quaternion::Normalize(orientation_);
      break;

    case SampleArcBallCameraMode::RollConstrained:
      pitch_ -= angle;
      pitch_ = MathHelper::Clamp(pitch_, -MathHelper::PiOver2 + 0.0001f,
                                 MathHelper::PiOver2 - 0.0001f);
      orientation_ = Quaternion::CreateFromAxisAngle(Vector3::Up, -yaw_) *
                     Quaternion::CreateFromAxisAngle(Vector3::Right, pitch_);
      break;
    }
  }

  void OrbitRight(float angle) {
    switch (mode_) {
    case SampleArcBallCameraMode::Free:
      orientation_ =
          orientation_ * Quaternion::CreateFromAxisAngle(Vector3::Up, angle);
      orientation_ = Quaternion::Normalize(orientation_);
      break;

    case SampleArcBallCameraMode::RollConstrained:
      yaw_ -= angle;
      yaw_ = std::fmod(yaw_, MathHelper::TwoPi);
      orientation_ = Quaternion::CreateFromAxisAngle(Vector3::Up, -yaw_) *
                     Quaternion::CreateFromAxisAngle(Vector3::Right, pitch_);
      orientation_ = Quaternion::Normalize(orientation_);
      break;
    }
  }

  void RotateClockwise(float angle) {
    switch (mode_) {
    case SampleArcBallCameraMode::Free:
      orientation_ = orientation_ *
                     Quaternion::CreateFromAxisAngle(Vector3::Forward, angle);
      orientation_ = Quaternion::Normalize(orientation_);
      break;

    case SampleArcBallCameraMode::RollConstrained:
      break;
    }
  }

  void SetCamera(const Vector3 &position, const Vector3 &target,
                 const Vector3 &up) {
    Matrix temporary = Matrix::CreateLookAt(position, target, up);
    temporary = Matrix::Invert(temporary);

    targetValue_ = target;
    orientation_ = Quaternion::CreateFromRotationMatrix(temporary);

    if (mode_ != SampleArcBallCameraMode::Free) {
      Vector3 direction = getDirectionProperty();
      direction.Y = 0.0f;
      if (direction.Length() == 0.0f) {
        direction = Vector3::Forward;
      }
      direction.Normalize();

      const float signX = direction.X > 0.0f   ? 1.0f
                          : direction.X < 0.0f ? -1.0f
                                               : 0.0f;
      yaw_ = static_cast<float>(std::acos(-direction.Z)) * signX;
      pitch_ = -(static_cast<float>(std::acos(
                     Vector3::Dot(Vector3::Up, getDirectionProperty()))) -
                 MathHelper::PiOver2);
    }
  }

  void HandleDefaultKeyboardControls(const KeyboardState &keyboardState,
                                     const GameTime &gameTime) {
    const float elapsedTime = static_cast<float>(
        gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

    const float dx = elapsedTime *
                     ReadKeyboardAxis(keyboardState, Keys::A, Keys::D) *
                     InputTurnRate;
    const float dy = elapsedTime *
                     ReadKeyboardAxis(keyboardState, Keys::S, Keys::W) *
                     InputTurnRate;

    if (dy != 0.0f) {
      OrbitUp(dy);
    }
    if (dx != 0.0f) {
      OrbitRight(dx);
    }

    distanceValue_ += ReadKeyboardAxis(keyboardState, Keys::Z, Keys::X) *
                      inputDistanceRateValue_ * elapsedTime;
    if (distanceValue_ < 0.001f) {
      distanceValue_ = 0.001f;
    }

    if (mode_ != SampleArcBallCameraMode::Free) {
      const float dr = elapsedTime *
                       ReadKeyboardAxis(keyboardState, Keys::Q, Keys::E) *
                       InputTurnRate;
      if (dr != 0.0f) {
        RotateClockwise(dr);
      }
    }
  }

  void HandleDefaultGamepadControls(const GamePadState &gamePadState,
                                    const GameTime &gameTime) {
    if (gamePadState.getIsConnectedProperty()) {
      const float elapsedTime = static_cast<float>(
          gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

      const float dx =
          gamePadState.getThumbSticksProperty().getRightProperty().X *
          elapsedTime * InputTurnRate;
      const float dy =
          gamePadState.getThumbSticksProperty().getRightProperty().Y *
          elapsedTime * InputTurnRate;
      float dr = gamePadState.getTriggersProperty().getRightProperty() *
                 elapsedTime * InputTurnRate;
      dr -= gamePadState.getTriggersProperty().getLeftProperty() * elapsedTime *
            InputTurnRate;

      if (dy != 0.0f) {
        OrbitUp(dy);
      }
      if (dx != 0.0f) {
        OrbitRight(dx);
      }
      if (dr != 0.0f) {
        RotateClockwise(dr);
      }

      if (gamePadState.IsButtonDown(Buttons::A)) {
        distanceValue_ -= elapsedTime * inputDistanceRateValue_;
      }
      if (gamePadState.IsButtonDown(Buttons::B)) {
        distanceValue_ += elapsedTime * inputDistanceRateValue_;
      }
      if (distanceValue_ < 0.001f) {
        distanceValue_ = 0.001f;
      }
    }
  }

  void Reset() {
    orientation_ = Quaternion::CreateFromAxisAngle(Vector3::Up, MathHelper::Pi);
    distanceValue_ = 3.0f;
    targetValue_ = Vector3::Zero;
    yaw_ = MathHelper::Pi;
    pitch_ = 0.0f;
  }

private:
  Vector3 targetValue_ = Vector3::Zero;
  float distanceValue_ = 0.0f;
  Quaternion orientation_;
  float inputDistanceRateValue_;
  static constexpr float InputTurnRate = 3.0f;
  SampleArcBallCameraMode mode_;
  float yaw_;
  float pitch_ = 0.0f;
};

} // namespace TexturesAndColorsSample
