// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Sprite.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace TouchGestureSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief A textured sprite manipulated by touch gestures. */
    class Sprite
    {
    public:
        // Use the exact XNA named-color values directly: this inline table can initialize before
        // CNA's out-of-line Color::White/Red/Blue/Green objects in another translation unit.
        /** @brief The possible sprite colors. */
        inline static const std::array<Microsoft::Xna::Framework::Color, 4> Colors{
            Microsoft::Xna::Framework::Color(255, 255, 255, 255),
            Microsoft::Xna::Framework::Color(255, 0, 0, 255),
            Microsoft::Xna::Framework::Color(0, 0, 255, 255),
            Microsoft::Xna::Framework::Color(0, 128, 0, 255)};

        /** @brief Fraction of velocity retained after a wall bounce. */
        static constexpr float BounceMagnitude = 0.5f;

        /** @brief Fraction of velocity lost per second. */
        static constexpr float Friction = 0.9f;

        /** @brief Minimum sprite scale. */
        static constexpr float MinScale = 0.5f;

        /** @brief Maximum sprite scale. */
        static constexpr float MaxScale = 2.0f;

    private:
        Texture2D texture;
        int colorIndex = 0;
        float scale = 1.0f;

    public:
        /** @brief Center of the sprite in viewport coordinates. */
        Vector2 Center;

        /** @brief Current sprite tint. */
        Microsoft::Xna::Framework::Color Color = Colors[0];

        /** @brief Current sprite velocity in pixels per second. */
        Vector2 Velocity;

        /**
         * @brief Constructs a sprite using the supplied texture.
         * @param textureValue Texture drawn for the sprite.
         */
        explicit Sprite(const Texture2D& textureValue)
            : texture(textureValue)
        {
        }

        /**
         * @brief Gets the current sprite scale.
         * @return The scale in the inclusive range MinScale through MaxScale.
         */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /**
         * @brief Sets and clamps the sprite scale.
         * @param value Requested scale.
         */
        void setScaleProperty(float value)
        {
            scale = MathHelper::Clamp(value, MinScale, MaxScale);
        }

        /**
         * @brief Gets the padded hit-test bounds of the sprite.
         * @return Scaled bounds inflated by ten pixels in each direction.
         */
        [[nodiscard]] Rectangle getHitBoundsProperty() const
        {
            Rectangle result(
                static_cast<int>(Center.X - texture.getWidthProperty() / 2 * scale),
                static_cast<int>(Center.Y - texture.getHeightProperty() / 2 * scale),
                static_cast<int>(texture.getWidthProperty() * scale),
                static_cast<int>(texture.getHeightProperty() * scale));
            result.Inflate(10, 10);
            return result;
        }

        /** @brief Advances the sprite to the next palette color. */
        void ChangeColor()
        {
            colorIndex = (colorIndex + 1) % static_cast<int>(Colors.size());
            Color = Colors[static_cast<std::size_t>(colorIndex)];
        }

        /**
         * @brief Advances motion, friction and wall bounces.
         * @param gameTime Current game timing snapshot.
         * @param bounds Bounds within which the sprite bounces.
         */
        void Update(const GameTime& gameTime, const Rectangle& bounds)
        {
            const float elapsed = static_cast<float>(
                gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

            Center += Velocity * elapsed;
            Velocity *= 1.0f - (Friction * elapsed);

            const float halfWidth = (texture.getWidthProperty() * scale) / 2.0f;
            const float halfHeight = (texture.getHeightProperty() * scale) / 2.0f;

            if (Center.X < bounds.getLeftProperty() + halfWidth)
            {
                Center.X = bounds.getLeftProperty() + halfWidth;
                Velocity.X *= -BounceMagnitude;
            }
            if (Center.X > bounds.getRightProperty() - halfWidth)
            {
                Center.X = bounds.getRightProperty() - halfWidth;
                Velocity.X *= -BounceMagnitude;
            }
            if (Center.Y < bounds.getTopProperty() + halfHeight)
            {
                Center.Y = bounds.getTopProperty() + halfHeight;
                Velocity.Y *= -BounceMagnitude;
            }
            if (Center.Y > bounds.getBottomProperty() - halfHeight)
            {
                Center.Y = bounds.getBottomProperty() - halfHeight;
                Velocity.Y *= -BounceMagnitude;
            }
        }

        /**
         * @brief Draws the sprite centered at its current position.
         * @param spriteBatch Active sprite batch.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            spriteBatch.Draw(
                texture,
                Center,
                std::optional<Rectangle>(std::nullopt),
                Color,
                0.0f,
                Vector2(
                    static_cast<float>(texture.getWidthProperty()) / 2.0f,
                    static_cast<float>(texture.getHeightProperty()) / 2.0f),
                scale,
                SpriteEffects::None,
                0.0f);
        }
    };
}
