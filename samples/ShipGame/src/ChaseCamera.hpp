// SPDX-License-Identifier: MS-PL
#pragma once

// ChaseCamera.hpp — C++ port of ShipGame/ChaseCamera.cs (XNA 4.0 Ship Game
// Starter Kit). A spring-damper camera that trails the ship and pulls in when
// the level geometry gets between it and the look-at point.

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "BoxCollider/CollisionMesh.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using BoxCollider::CollisionMesh;

// This class was first seen in the Chase Camera sample.
class ChaseCamera {
public:
    // ---- Chased object properties (set externally each frame) ----

    // Position of object being chased.
    Vector3 getChasePosition() const { return chasePosition_; }
    void setChasePosition(Vector3 value) { chasePosition_ = value; }

    // Direction the chased object is facing.
    Vector3 getChaseDirection() const { return chaseDirection_; }
    void setChaseDirection(Vector3 value) { chaseDirection_ = value; }

    // Chased object's Up vector.
    Vector3 getUp() const { return up_; }
    void setUp(Vector3 value) { up_ = value; }

    // ---- Desired camera positioning (set when creating camera or changing view) ----

    // Desired camera position in the chased object's coordinate system.
    Vector3 getDesiredPositionOffset() const { return desiredPositionOffset_; }
    void setDesiredPositionOffset(Vector3 value) { desiredPositionOffset_ = value; }

    // Desired camera position in world space.
    Vector3 getDesiredPosition() {
        // Ensure correct value even if update has not been called this frame
        UpdateWorldPositions();

        return desiredPosition_;
    }

    // Look at point in the chased object's coordinate system.
    Vector3 getLookAtOffset() const { return lookAtOffset_; }
    void setLookAtOffset(Vector3 value) { lookAtOffset_ = value; }

    // Look at point in world space.
    Vector3 getLookAt() {
        // Ensure correct value even if update has not been called this frame
        UpdateWorldPositions();

        return lookAt_;
    }

    // ---- Camera physics (typically set when creating camera) ----

    // Physics coefficient which controls the influence of the camera's position
    // over the spring force. The stiffer the spring, the closer it will stay to
    // the chased object.
    float getStiffness() const { return stiffness_; }
    void setStiffness(float value) { stiffness_ = value; }

    // Physics coefficient which approximates internal friction of the spring.
    // Sufficient damping will prevent the spring from oscillating infinitely.
    float getDamping() const { return damping_; }
    void setDamping(float value) { damping_ = value; }

    // Mass of the camera body. Heaver objects require stiffer springs with less
    // damping to move at the same rate as lighter objects.
    float getMass() const { return mass_; }
    void setMass(float value) { mass_ = value; }

    // ---- Current camera properties (updated by camera physics) ----

    // Position of camera in world space.
    Vector3 getPosition() const { return collisionPosition_; }

    // Velocity of camera.
    Vector3 getVelocity() const { return velocity_; }

    // ---- Matrix properties ----

    // View transform matrix.
    Matrix getView() const { return view_; }

    // ---- Methods ----

    // Forces camera to be at desired position and to stop moving. The is useful
    // when the chased object is first created or after it has been teleported.
    // Failing to call this after a large change to the chased object's position
    // will result in the camera quickly flying across the world.
    void Reset() {
        UpdateWorldPositions();

        // Stop motion
        velocity_ = Vector3::Zero;

        // Force desired position
        position_ = desiredPosition_;

        UpdateMatrices();
    }

    // Animates the camera from its current position towards the desired offset
    // behind the chased object. The camera's animation is controlled by a simple
    // physical spring attached to the camera and anchored to the desired position.
    void Update(float elapsedTime, CollisionMesh* collision) {
        UpdateWorldPositions();

        // Calculate spring force
        Vector3 stretch = position_ - desiredPosition_;
        Vector3 force = -stiffness_ * stretch - damping_ * velocity_;

        // Apply acceleration
        Vector3 acceleration = force / mass_;
        velocity_ += acceleration * elapsedTime;

        // Apply velocity
        position_ += velocity_ * elapsedTime;
        collisionPosition_ = position_;

        // test camera for collision with world
        if (collision != nullptr) {
            float collisionDistance;
            Vector3 collisionPoint, collisionNormal;
            if (collision->PointIntersect(lookAt_, position_, collisionDistance, collisionPoint,
                                          collisionNormal)) {
                Vector3 dir = Vector3::Normalize(collisionPoint - lookAt_);
                collisionPosition_ = collisionPoint - 10 * dir;
            }
        }

        UpdateMatrices();
    }

private:
    // Rebuilds object space values in world space. Invoke before publicly
    // returning or privately accessing world space values.
    void UpdateWorldPositions() {
        // Construct a matrix to transform from object space to worldspace
        Matrix transform = Matrix::getIdentityProperty();
        transform.setForwardProperty(getChaseDirection());
        transform.setUpProperty(getUp());
        transform.setRightProperty(Vector3::Cross(getUp(), getChaseDirection()));

        // Calculate desired camera properties in world space
        desiredPosition_ =
            getChasePosition() + Vector3::TransformNormal(getDesiredPositionOffset(), transform);
        lookAt_ = getChasePosition() + Vector3::TransformNormal(getLookAtOffset(), transform);
    }

    // Rebuilds camera's view and projection matricies.
    void UpdateMatrices() { view_ = Matrix::CreateLookAt(getPosition(), getLookAt(), getUp()); }

    Vector3 chasePosition_;
    Vector3 chaseDirection_;
    Vector3 up_ = Vector3::Up;

    Vector3 desiredPositionOffset_{0, 2.0f, 2.0f};
    Vector3 desiredPosition_;

    Vector3 lookAtOffset_{0, 2.8f, 0};
    Vector3 lookAt_;

    float stiffness_ = 1800.0f;
    float damping_ = 600.0f;
    float mass_ = 50.0f;

    Vector3 position_;
    Vector3 collisionPosition_;

    Vector3 velocity_;

    Matrix view_;
};

} // namespace ShipGame
