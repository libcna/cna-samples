// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionCamera.hpp — C++ port of BoxCollider/CollisionCamera.cs (XNA 4.0 Ship
// Game Starter Kit). The base camera the collider ships: view/projection kept in
// step with the frustum, and the shared input and orthonormalization helpers.

#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/TimeSpan.hpp"

#include "CollisionMesh.hpp"
#include "CollisionTreeElem.hpp"

namespace BoxCollider {

using Microsoft::Xna::Framework::BoundingFrustum;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::GamePadState;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;

// base camera class. Port of BoxCollider/CollisionCamera.cs.
class CollisionCamera : public CollisionTreeElemDynamic {
public:
    float getNearPlane() const { return nearPlane_; }
    void setNearPlane(float value) {
        nearPlane_ = value;
        projection = Matrix::CreatePerspectiveFieldOfView(getAngle(), getAspect(), getNearPlane(),
                                                          getFarPlane());
        frustum = BoundingFrustum(view * projection);
    }

    float getFarPlane() const { return farPlane_; }
    void setFarPlane(float value) {
        farPlane_ = value;
        projection = Matrix::CreatePerspectiveFieldOfView(getAngle(), getAspect(), getNearPlane(),
                                                          getFarPlane());
        frustum = BoundingFrustum(view * projection);
    }

    float getAngle() const { return angle_; }
    void setAngle(float value) {
        angle_ = value;
        projection = Matrix::CreatePerspectiveFieldOfView(getAngle(), getAspect(), getNearPlane(),
                                                          getFarPlane());
        frustum = BoundingFrustum(view * projection);
    }

    float getAspect() const { return aspect_; }
    void setAspect(float value) {
        aspect_ = value;
        projection = Matrix::CreatePerspectiveFieldOfView(getAngle(), getAspect(), getNearPlane(),
                                                          getFarPlane());
        frustum = BoundingFrustum(view * projection);
    }

    Matrix world;      // camera position and rotation
    Matrix view;       // view = inverse( world )
    Matrix projection; // projection matrix

    BoundingFrustum frustum{Matrix::getIdentityProperty()}; // camera frustum

    ~CollisionCamera() override = default;

    // get world matrix axis or its tranlation component
    // (0 for X, 1 for Y, 2 for Z and 3 for translation)
    Vector3 GetWorldVector(int axis) const {
        switch (axis) {
            case 0: return Vector3(world.M11, world.M12, world.M13);
            case 1: return Vector3(world.M21, world.M22, world.M23);
            case 2: return Vector3(world.M31, world.M32, world.M33);
            case 3: return Vector3(world.M41, world.M42, world.M43);
        }

        return Vector3::Zero;
    }

    // get view matrix axis or its tranlation component
    // (0 for X, 1 for Y, 2 for Z and 3 for translation)
    Vector3 GetViewVector(int axis) const {
        switch (axis) {
            case 0: return Vector3(view.M11, view.M12, view.M13);
            case 1: return Vector3(view.M21, view.M22, view.M23);
            case 2: return Vector3(view.M31, view.M32, view.M33);
            case 3: return Vector3(view.M41, view.M42, view.M43);
        }

        return Vector3::Zero;
    }

    // get tranlation and rotation from input devices
    static void GetInputVectors(const GamePadState& gamepadState,
                                const KeyboardState& keyboardState, Vector3& translate,
                                Vector3& rotate) {
        translate = Vector3::Zero;
        rotate = Vector3::Zero;

        translate.X = gamepadState.getThumbSticksProperty().getLeftProperty().X;
        if (keyboardState.IsKeyDown(Keys::Q))
            translate.X -= 1.0f;
        if (keyboardState.IsKeyDown(Keys::E))
            translate.X += 1.0f;

        translate.Y = 0;

        translate.Z = gamepadState.getThumbSticksProperty().getLeftProperty().Y;
        if (keyboardState.IsKeyDown(Keys::W))
            translate.Z += 1.0f;
        if (keyboardState.IsKeyDown(Keys::S))
            translate.Z -= 1.0f;

        rotate.X = gamepadState.getThumbSticksProperty().getRightProperty().Y;
        if (keyboardState.IsKeyDown(Keys::Down))
            rotate.X -= 0.7f;
        if (keyboardState.IsKeyDown(Keys::Up))
            rotate.X += 0.7f;

        rotate.Y = gamepadState.getThumbSticksProperty().getRightProperty().X;
        if (keyboardState.IsKeyDown(Keys::Left))
            rotate.Y -= 0.7f;
        if (keyboardState.IsKeyDown(Keys::Right))
            rotate.Y += 0.7f;

        rotate.Z = 0;
        if (gamepadState.getButtonsProperty().getLeftShoulderProperty() == ButtonState::Pressed ||
            keyboardState.IsKeyDown(Keys::A))
            rotate.Z += 0.7f;
        if (gamepadState.getButtonsProperty().getRightShoulderProperty() == ButtonState::Pressed ||
            keyboardState.IsKeyDown(Keys::D))
            rotate.Z -= 0.7f;

        // The dead-zone clamps read `>= 0.00001f && < 0.00001f` in the original and can therefore
        // never be true. Reproduced, not repaired.
        if (rotate.X >= 0.00001f && rotate.X < 0.00001f)
            rotate.X = 0;
        if (rotate.Y >= 0.00001f && rotate.Y < 0.00001f)
            rotate.Y = 0;
        if (rotate.Z >= 0.00001f && rotate.Z < 0.00001f)
            rotate.Z = 0;
    }

    // make sure matrix axis are perpendicular and unit size
    static void Orthonormalize(Matrix& m) {
        Vector3 axisX(m.M11, m.M12, m.M13);
        Vector3 axisY(m.M21, m.M22, m.M23);
        Vector3 axisZ(m.M31, m.M32, m.M33);
        axisZ = Vector3::Normalize(Vector3::Cross(axisX, axisY));
        axisY = Vector3::Normalize(Vector3::Cross(axisZ, axisX));
        axisX = Vector3::Normalize(Vector3::Cross(axisY, axisZ));
        m.M11 = axisX.X; m.M12 = axisX.Y; m.M13 = axisX.Z;
        m.M21 = axisY.X; m.M22 = axisY.Y; m.M23 = axisY.Z;
        m.M31 = axisZ.X; m.M32 = axisZ.Y; m.M33 = axisZ.Z;
    }

    virtual void Draw(GraphicsDevice* gd) = 0;
    virtual void Reset(Matrix m) = 0;
    virtual void Update(System::TimeSpan elapsedTime, CollisionMesh& collisionMesh,
                        const GamePadState& gamepadState, const KeyboardState& keyboardState) = 0;

protected:
    CollisionCamera(Vector3 position, Vector3 lookPosition, float angle, float aspect) {
        setAngle(angle);
        setAspect(aspect);

        projection =
            Matrix::CreatePerspectiveFieldOfView(angle, aspect, getNearPlane(), getFarPlane());

        view = Matrix::CreateLookAt(position, lookPosition, Vector3::Up);

        world = Matrix::Invert(view);

        frustum = BoundingFrustum(view * projection);
    }

private:
    float nearPlane_ = 1.0f;
    float farPlane_ = 10000.0f;
    float angle_ = MathHelper::ToRadians(60);
    float aspect_ = 1.0f;
};

} // namespace BoxCollider
