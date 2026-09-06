// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionCameraObserver.hpp — C++ port of BoxCollider/CollisionCameraObserver.cs
// (XNA 4.0 Ship Game Starter Kit). The free-flying "descent like" camera, sliding
// its bounding box along the collision mesh.

#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/IDisposable.hpp"
#include "System/TimeSpan.hpp"

#include "CollisionCamera.hpp"

namespace BoxCollider {

// observer camera (descent like camera). Port of BoxCollider/CollisionCameraObserver.cs.
class CollisionCameraObserver : public CollisionCamera, public System::IDisposable {
public:
    CollisionCameraObserver(Vector3 position, Vector3 lookPosition, float angle, float aspect,
                            float radius)
        : CollisionCamera(position, lookPosition, angle, aspect) {
        box.emplace(-radius, radius);
    }

    ~CollisionCameraObserver() override { Dispose(true); }

    void Draw(GraphicsDevice* gd) override {
        box->min += world.getTranslationProperty();
        box->max += world.getTranslationProperty();

        box->Draw(gd);

        box->min -= world.getTranslationProperty();
        box->max -= world.getTranslationProperty();
    }

    void Reset(Matrix m) override {
        world = m;
        view = Matrix::Invert(world);
        frustum = BoundingFrustum(view * projection);
    }

    void Update(System::TimeSpan elapsedTime, CollisionMesh& collisionMesh,
                const GamePadState& gamepadState, const KeyboardState& keyboardState) override {
        float timeSeconds = (float)elapsedTime.getTotalSecondsProperty();

        float speedBoost = 0.0f;
        if (gamepadState.getButtonsProperty().getLeftStickProperty() == ButtonState::Pressed)
            speedBoost = 1.0f;
        if (keyboardState.IsKeyDown(Keys::LeftShift))
            speedBoost = 1.0f;

        float rotSpeed = 2.0f * timeSeconds;
        float moveSpeed = (400.0f + 600.0f * speedBoost) * timeSeconds;

        Vector3 position = world.getTranslationProperty();

        Vector3 axisX(world.M11, world.M12, world.M13);
        Vector3 axisY(world.M21, world.M22, world.M23);
        Vector3 axisZ(world.M31, world.M32, world.M33);

        Vector3 translate, rotate;
        GetInputVectors(gamepadState, keyboardState, translate, rotate);
        if (gamepadState.getButtonsProperty().getRightStickProperty() == ButtonState::Pressed)
            rotate.X = rotate.Y = 0;

        Vector3 newPosition = position;
        newPosition += axisX * (moveSpeed * translate.X);
        newPosition += axisY * (moveSpeed * translate.Y);
        newPosition -= axisZ * (moveSpeed * translate.Z);

        collisionMesh.BoxMove(*box, position, newPosition, 1, 0, 3, newPosition);

        Matrix rotX = Matrix::CreateFromAxisAngle(axisX, -rotSpeed * rotate.X);
        Matrix rotY = Matrix::CreateFromAxisAngle(axisY, -rotSpeed * rotate.Y);
        Matrix rotZ = Matrix::CreateFromAxisAngle(axisZ, rotSpeed * rotate.Z);

        world.setTranslationProperty(Vector3(0, 0, 0));

        world = world * (rotX * rotY * rotZ);

        world.setTranslationProperty(newPosition);

        Orthonormalize(world);

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
};

} // namespace BoxCollider
