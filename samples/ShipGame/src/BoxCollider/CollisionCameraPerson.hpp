// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionCameraPerson.hpp — C++ port of BoxCollider/CollisionCameraPerson.cs
// (XNA 4.0 Ship Game Starter Kit). The walking "quake like" camera: gravity,
// jumping, step climbing and a separate up/down look rotation.

#include <cmath>

#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/IDisposable.hpp"
#include "System/TimeSpan.hpp"

#include "CollisionCamera.hpp"

namespace BoxCollider {

// person camera (quake like camera). Port of BoxCollider/CollisionCameraPerson.cs.
class CollisionCameraPerson : public CollisionCamera, public System::IDisposable {
public:
    CollisionCameraPerson(Vector3 position, Vector3 lookPosition, float angle, float aspect,
                          float width, float height, float stepHeight, float headHeight,
                          float upDownRot, float gravity, float jumpHeight)
        : CollisionCamera(position, lookPosition, angle, aspect) {
        width *= 0.5f;
        height *= 0.5f;

        stepHeight_ = stepHeight;
        headHeight_ = headHeight - height;
        upDownRot_ = upDownRot;
        gravity_ = gravity;
        jumpHeight_ = jumpHeight;

        transform_ = world;

        onGround_ = false;
        velocity_ = Vector3::Zero;

        box.emplace(Vector3(-width, -height + stepHeight, -width), Vector3(width, height, width));
    }

    ~CollisionCameraPerson() override { Dispose(true); }

    void Draw(GraphicsDevice* gd) override {
        box->min += world.getTranslationProperty();
        box->max += world.getTranslationProperty();
        box->min.Y -= headHeight_;
        box->max.Y -= headHeight_;

        box->Draw(gd);

        box->min -= world.getTranslationProperty();
        box->max -= world.getTranslationProperty();
        box->min.Y += headHeight_;
        box->max.Y += headHeight_;
    }

    void Reset(Matrix m) override {
        // make sure matrix Y axis is (0,1,0)
        transform_ = m;
        if (transform_.M22 < 0.9999f) {
            // rotate Y to (0,1,0)
            Vector3 axisY(transform_.M21, transform_.M22, transform_.M23);
            float ang = (float)std::acos(axisY.Y);
            Vector3 axis = Vector3::Normalize(Vector3::Cross(axisY, Vector3::UnitY));
            Vector3 pos = transform_.getTranslationProperty();
            transform_.setTranslationProperty(Vector3::Zero);
            transform_ = transform_ * Matrix::CreateFromAxisAngle(axis, ang);
            transform_.setTranslationProperty(pos);
        }
        upDownRot_ = 0.0f;
        world = transform_;
        view = Matrix::Invert(world);
        frustum = BoundingFrustum(view * projection);
    }

    void Update(System::TimeSpan elapsedTime, CollisionMesh& collisionMesh,
                const GamePadState& gamepadState, const KeyboardState& keyboardState) override {
        float timeSeconds = (float)elapsedTime.getTotalSecondsProperty();

        float speedBoost = gamepadState.getTriggersProperty().getLeftProperty();
        if (keyboardState.IsKeyDown(Keys::LeftShift))
            speedBoost = 1.0f;

        float rotSpeed = 2.0f * timeSeconds;
        float moveSpeed = (300.0f + 400.0f * speedBoost) * timeSeconds;

        if (onGround_ == false)
            velocity_.Y -= gravity_ * timeSeconds;
        else {
            if (gamepadState.getButtonsProperty().getAProperty() == ButtonState::Pressed ||
                keyboardState.IsKeyDown(Keys::Space)) {
                velocity_.Y = (float)std::sqrt(gravity_ * 2.0f * jumpHeight_);
                onGround_ = false;
            } else
                velocity_.Y = 0.0f;
        }

        Vector3 position = transform_.getTranslationProperty();

        Vector3 axisX(transform_.M11, transform_.M12, transform_.M13);
        Vector3 axisY(0, 1, 0);
        Vector3 axisZ(transform_.M31, transform_.M32, transform_.M33);

        Vector3 translate, rotate;
        GetInputVectors(gamepadState, keyboardState, translate, rotate);

        Vector3 newPosition = position;
        newPosition += axisX * (moveSpeed * translate.X);
        newPosition -= axisZ * (moveSpeed * translate.Z);
        newPosition += velocity_ * timeSeconds;

        float moveY = 12.5f * stepHeight_ * timeSeconds;
        if (autoMoveY_ >= 0) {
            if (moveY > autoMoveY_)
                moveY = autoMoveY_;
        } else {
            moveY = -moveY;
            if (moveY < autoMoveY_)
                moveY = autoMoveY_;
        }
        newPosition.Y += moveY;
        autoMoveY_ = 0;

        collisionMesh.BoxMove(*box, position, newPosition, 1, 0, 3, newPosition);

        if (std::abs(newPosition.Y - position.Y) < 0.0001f && velocity_.Y > 0.0f)
            velocity_.Y = 0.0f;

        float dist;
        Vector3 pos, norm;
        if (velocity_.Y <= 0)
            if (true == collisionMesh.BoxIntersect(*box, newPosition,
                                                   newPosition + Vector3(0, -2 * stepHeight_, 0),
                                                   dist, pos, norm)) {
                if (norm.Y > 0.70710678f) {
                    onGround_ = true;
                    autoMoveY_ = stepHeight_ - dist;
                } else
                    onGround_ = false;
            } else
                onGround_ = false;

        upDownRot_ -= rotSpeed * rotate.X;
        if (upDownRot_ > 1)
            upDownRot_ = 1;
        else if (upDownRot_ < -1)
            upDownRot_ = -1;

        Matrix rotX = Matrix::CreateFromAxisAngle(axisX, upDownRot_);
        Matrix rotY = Matrix::CreateFromAxisAngle(axisY, -rotSpeed * rotate.Y);

        transform_.setTranslationProperty(Vector3::Zero);
        transform_ = transform_ * rotY;
        Orthonormalize(transform_);

        world.setTranslationProperty(Vector3::Zero);
        world = transform_ * rotX;

        transform_.setTranslationProperty(newPosition);
        newPosition.Y += headHeight_;
        world.setTranslationProperty(newPosition);

        view = Matrix::Invert(world);

        frustum = BoundingFrustum(view * projection);
    }

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override { Dispose(true); }

private:
    void Dispose(bool disposing) {
        if (disposing && !isDisposed_) {
            box.reset();
        }
    }

    bool isDisposed_ = false;

    Matrix transform_;   // the person transform matrix (without up/down rot)
    Vector3 velocity_;   // current velocity vector used only by gravity

    float headHeight_ = 0.0f; // height from center of box to eye position
    float stepHeight_ = 0.0f; // max height for step player can climb without jumping

    float gravity_ = 0.0f;    // gravity intensity
    bool onGround_ = false;   // is player on ground (false if in air)
    float jumpHeight_ = 0.0f; // height player will reach when jumping

    float upDownRot_ = 0.0f;  // up/down view rotation
    float autoMoveY_ = 0.0f;  // distance to move in Y axis on next update
                              // in order to climb up/down a step
};

} // namespace BoxCollider
