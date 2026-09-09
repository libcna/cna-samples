// SPDX-License-Identifier: MS-PL
#pragma once

// Sphere.hpp — C++ port of Sphere.cs (XNA 4.0 PerformanceMeasuring sample).

#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

#include "Primitives/SpherePrimitive.hpp"

namespace PerformanceMeasuring {

using Microsoft::Xna::Framework::BoundingSphere;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;

/** @brief One simulated sphere: a coloured ball that moves and can be drawn. */
class Sphere {
public:
    /** @brief Centre of the sphere in world space. */
    Vector3 Position;
    /** @brief Movement per second, in world units. */
    Vector3 Velocity;
    /** @brief Colour the sphere is drawn in. */
    Microsoft::Xna::Framework::Color Color = Microsoft::Xna::Framework::Color::White;

    /**
     * @brief Creates a sphere and the primitive that draws it.
     *
     * @param graphics Device the primitive's buffers are created on.
     * @param radius   Radius in world units; the primitive is built at twice this diameter.
     */
    Sphere(GraphicsDevice& graphics, float radius) : primitive_(graphics, radius * 2.0f, 10), radius_(radius) {}

    /** @brief Gets the radius in world units. @return The radius. */
    [[nodiscard]] float getRadiusProperty() const { return radius_; }

    /** @brief Gets the bounding sphere at the current position. @return Bounds for collision tests. */
    [[nodiscard]] BoundingSphere getBoundsProperty() const { return BoundingSphere(Position, radius_); }

    /**
     * @brief Advances the position by the velocity over the elapsed time.
     *
     * @param gameTime Timing values for this frame.
     */
    void Update(const GameTime& gameTime) {
        Position += Velocity * (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    }

    /**
     * @brief Draws the sphere at its current position.
     *
     * @param view       View matrix.
     * @param projection Projection matrix.
     */
    void Draw(const Matrix& view, const Matrix& projection) {
        primitive_.Draw(Matrix::CreateTranslation(Position), view, projection, Color);
    }

private:
    SpherePrimitive primitive_;
    float radius_;
};

} // namespace PerformanceMeasuring
