// SPDX-License-Identifier: MS-PL
#pragma once

// PlayerMovement.hpp — C++ port of ShipGame/PlayerMovement.cs (XNA 4.0 Ship
// Game Starter Kit). The ship's flight model: local-space force, velocity and
// damping for both translation and rotation.

#include <algorithm>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "GameOptions.hpp"
#include "InputManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::Keys;

// Port of ShipGame/PlayerMovement.cs.
class PlayerMovement {
public:
    Vector3 position; // player position
    Vector3 velocity; // velocity in local player space
    Vector3 force;    // forces in local player space

    // player rotation
    Matrix rotation;
    // rotation velocities around each local player axis
    Vector3 rotationVelocityAxis;
    // rotation forces around each local player axis
    Vector3 rotationForce;

    float maxVelocity;         // maximum player velocity
    float maxRotationVelocity; // maximum player rotation velocity

    float dampingForce;         // damping force
    float dampingRotationForce; // damping rotation force

    // maximum force created by input stick
    float inputForce;
    // maximum rotation force created by input stick
    float inputRotationForce;

    // Create a new player movement object for handling player motion
    PlayerMovement() {
        position = Vector3::Zero;
        velocity = Vector3::Zero;
        force = Vector3::Zero;

        rotation = Matrix::getIdentityProperty();
        rotationVelocityAxis = Vector3::Zero;
        rotationForce = Vector3::Zero;

        maxVelocity = GameOptions::MovementVelocity;
        dampingForce = GameOptions::MovementForceDamping;
        inputForce = GameOptions::MovementForce;

        maxRotationVelocity = GameOptions::MovementRotationVelocity;
        dampingRotationForce = GameOptions::MovementRotationForceDamping;
        inputRotationForce = GameOptions::MovementRotationForce;
    }

    // Resets the position and rotation of the player and zero forces
    void Reset(Matrix transfrom) {
        rotation = transfrom;
        position = transfrom.getTranslationProperty();

        velocity = Vector3::Zero;
        force = Vector3::Zero;

        rotationVelocityAxis = Vector3::Zero;
        rotationForce = Vector3::Zero;
    }

    // Get the current postion and rotation as a matrix
    Matrix getTransform() const {
        Matrix transform;

        // set rotation
        transform = rotation;

        // set translation
        transform.setTranslationProperty(position);

        return transform;
    }

    // Get the normalized velocity
    float getVelocityFactor() const { return velocity.Length() / maxVelocity; }

    // Get/Set the velocity vector transformed to world space
    // transform local velocity to world space
    Vector3 getWorldVelocity() const {
        return velocity.X * rotation.getRightProperty() + velocity.Y * rotation.getUpProperty() +
               velocity.Z * rotation.getForwardProperty();
    }
    void setWorldVelocity(Vector3 value) {
        // transform world velocity into local space
        velocity.X = Vector3::Dot(rotation.getRightProperty(), value);
        velocity.Y = Vector3::Dot(rotation.getUpProperty(), value);
        velocity.Z = Vector3::Dot(rotation.getForwardProperty(), value);
    }

    // Process movement input
    void ProcessInput(float elapsedTime, const InputState& current, int player) {
        // camera rotation
        rotationForce.X =
            inputRotationForce *
            current.padState[player].getThumbSticksProperty().getRightProperty().Y;
        rotationForce.Y =
            -inputRotationForce *
            current.padState[player].getThumbSticksProperty().getRightProperty().X;
        rotationForce.Z = 0.0f;

        // camera bank
        if (current.padState[player].getButtonsProperty().getRightShoulderProperty() ==
            ButtonState::Pressed)
            rotationForce.Z += inputRotationForce;
        if (current.padState[player].getButtonsProperty().getLeftShoulderProperty() ==
            ButtonState::Pressed)
            rotationForce.Z -= inputRotationForce;

        // move forward/backward
        force.X = inputForce *
                  current.padState[player].getThumbSticksProperty().getLeftProperty().X;

        if (current.padState[player].getButtonsProperty().getRightStickProperty() ==
            ButtonState::Pressed) {
            // slide up/down
            force.Y = inputForce *
                      current.padState[player].getThumbSticksProperty().getLeftProperty().Y;
            force.Z = 0.0f;
        } else {
            // slide left/right
            force.Y = 0.0f;
            force.Z = inputForce *
                      current.padState[player].getThumbSticksProperty().getLeftProperty().Y;
        }

        // keyboard camera rotation
        if (current.keyState[player].IsKeyDown(Keys::Up))
            rotationForce.X = inputRotationForce;
        if (current.keyState[player].IsKeyDown(Keys::Down))
            rotationForce.X = -inputRotationForce;
        if (current.keyState[player].IsKeyDown(Keys::Left))
            rotationForce.Y = inputRotationForce;
        if (current.keyState[player].IsKeyDown(Keys::Right))
            rotationForce.Y = -inputRotationForce;
        // keyboard camera bank
        if (current.keyState[player].IsKeyDown(Keys::A))
            rotationForce.Z = -inputRotationForce;
        if (current.keyState[player].IsKeyDown(Keys::D))
            rotationForce.Z = inputRotationForce;
        // move forward/backward
        if (current.keyState[player].IsKeyDown(Keys::W))
            force.Z = inputForce;
        if (current.keyState[player].IsKeyDown(Keys::S))
            force.Z = -inputForce;
        // slide left/right
        if (current.keyState[player].IsKeyDown(Keys::Q))
            force.X = -inputForce;
        if (current.keyState[player].IsKeyDown(Keys::E))
            force.X = inputForce;
    }

    void Update(float elapsedTime) {
        // apply force
        velocity += force * elapsedTime;

        // apply damping
        if (force.X > -0.001f && force.X < 0.001f) {
            if (velocity.X > 0)
                velocity.X = std::max(0.0f, velocity.X - dampingForce * elapsedTime);
            else
                velocity.X = std::min(0.0f, velocity.X + dampingForce * elapsedTime);
        }
        if (force.Y > -0.001f && force.Y < 0.001f) {
            if (velocity.Y > 0)
                velocity.Y = std::max(0.0f, velocity.Y - dampingForce * elapsedTime);
            else
                velocity.Y = std::min(0.0f, velocity.Y + dampingForce * elapsedTime);
        }
        if (force.Z > -0.001f && force.Z < 0.001f) {
            if (velocity.Z > 0)
                velocity.Z = std::max(0.0f, velocity.Z - dampingForce * elapsedTime);
            else
                velocity.Z = std::min(0.0f, velocity.Z + dampingForce * elapsedTime);
        }

        // crop with maximum velocity
        float velocityLength = velocity.Length();
        if (velocityLength > maxVelocity)
            velocity = Vector3::Normalize(velocity) * maxVelocity;

        // apply velocity
        position += rotation.getRightProperty() * velocity.X * elapsedTime;
        position += rotation.getUpProperty() * velocity.Y * elapsedTime;
        position += rotation.getForwardProperty() * velocity.Z * elapsedTime;

        // apply rot force
        rotationVelocityAxis += rotationForce * elapsedTime;

        // apply rot damping
        if (rotationForce.X > -0.001f && rotationForce.X < 0.001f) {
            if (rotationVelocityAxis.X > 0)
                rotationVelocityAxis.X =
                    std::max(0.0f, rotationVelocityAxis.X - dampingRotationForce * elapsedTime);
            else
                rotationVelocityAxis.X =
                    std::min(0.0f, rotationVelocityAxis.X + dampingRotationForce * elapsedTime);
        }

        if (rotationForce.Y > -0.001f && rotationForce.Y < 0.001f) {
            if (rotationVelocityAxis.Y > 0)
                rotationVelocityAxis.Y =
                    std::max(0.0f, rotationVelocityAxis.Y - dampingRotationForce * elapsedTime);
            else
                rotationVelocityAxis.Y =
                    std::min(0.0f, rotationVelocityAxis.Y + dampingRotationForce * elapsedTime);
        }

        if (rotationForce.Z > -0.001f && rotationForce.Z < 0.001f) {
            if (rotationVelocityAxis.Z > 0)
                rotationVelocityAxis.Z =
                    std::max(0.0f, rotationVelocityAxis.Z - dampingRotationForce * elapsedTime);
            else
                rotationVelocityAxis.Z =
                    std::min(0.0f, rotationVelocityAxis.Z + dampingRotationForce * elapsedTime);
        }

        // crop with maximum rot velocity
        float rotationVelocityLength = rotationVelocityAxis.Length();
        if (rotationVelocityLength > maxRotationVelocity)
            rotationVelocityAxis = Vector3::Normalize(rotationVelocityAxis) * maxRotationVelocity;

        // apply rot vel
        Matrix rotationVelocity = Matrix::getIdentityProperty();

        if (rotationVelocityAxis.X < -0.001f || rotationVelocityAxis.X > 0.001f)
            rotationVelocity =
                rotationVelocity * Matrix::CreateFromAxisAngle(rotation.getRightProperty(),
                                                               rotationVelocityAxis.X * elapsedTime);

        if (rotationVelocityAxis.Y < -0.001f || rotationVelocityAxis.Y > 0.001f)
            rotationVelocity =
                rotationVelocity * Matrix::CreateFromAxisAngle(rotation.getUpProperty(),
                                                               rotationVelocityAxis.Y * elapsedTime);

        if (rotationVelocityAxis.Z < -0.001f || rotationVelocityAxis.Z > 0.001f)
            rotationVelocity =
                rotationVelocity * Matrix::CreateFromAxisAngle(rotation.getForwardProperty(),
                                                               rotationVelocityAxis.Z * elapsedTime);

        rotation = rotation * rotationVelocity;
    }
};

} // namespace ShipGame
