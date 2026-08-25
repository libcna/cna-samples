// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// TransformedSprite.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Math.hpp"

namespace TransformedCollisionTest
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief A sprite with an origin, rotation and scale, able to test its own drawn pixels
     *        against another such sprite.
     */
    class TransformedSprite
    {
    private:
        Texture2D texture;
        std::vector<Color> colorData;
        Matrix transform;

    public:
        /** @brief World position of the sprite's origin. */
        Vector2 Position;
        /** @brief Pivot point, in the sprite's own texture space. */
        Vector2 Origin;
        /** @brief Rotation about the origin, in radians. */
        float Rotation = 0.0f;
        /** @brief Uniform scale about the origin. */
        float Scale = 1.0f;

        /**
         * @brief Gets the sprite's texture.
         * @return The texture this sprite draws and tests.
         */
        [[nodiscard]] const Texture2D& getTextureProperty() const { return texture; }

        /**
         * @brief Sets the sprite's texture and reads back its pixels for collision testing.
         * @param value The texture to draw and test.
         */
        void setTextureProperty(const Texture2D& value)
        {
            texture = value;
            colorData = std::vector<Color>(
                texture.getWidthProperty() * texture.getHeightProperty());
            texture.GetData(colorData.data(), (int)colorData.size());
        }

        /**
         * @brief Gets the sprite's world transform as of the last UpdateTransform().
         * @return The world transform.
         */
        [[nodiscard]] const Matrix& getTransformProperty() const { return transform; }

        /**
         * @brief Rebuilds the world transform from the current origin, scale, rotation
         *        and position.
         */
        void UpdateTransform()
        {
            transform = Matrix::CreateTranslation(Vector3(-Origin, 0.0f)) *
                        Matrix::CreateScale(Scale) *
                        Matrix::CreateRotationZ(Rotation) *
                        Matrix::CreateTranslation(Vector3(Position, 0.0f));
        }

        /**
         * @brief Draws the sprite in world space.
         * @param spriteBatch An already-begun sprite batch.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            spriteBatch.Draw(texture, Position, std::nullopt, Color::Black, Rotation, Origin,
                Scale, SpriteEffects::None, 0.0f);
        }

        /**
         * @brief Tests this sprite's drawn pixels against another sprite's.
         * @param b The other sprite.
         * @return True if non-transparent pixels overlap; false otherwise.
         */
        [[nodiscard]] bool IntersectPixels(const TransformedSprite& b) const
        {
            return IntersectPixels(transform, texture.getWidthProperty(),
                           texture.getHeightProperty(), colorData,
                           b.transform, b.texture.getWidthProperty(),
                           b.texture.getHeightProperty(), b.colorData);
        }

        /**
         * @brief Determines if there is overlap of the non-transparent pixels between two
         *        sprites, transforming every pixel of A individually.
         *
         * @param transformA World transform of the first sprite.
         * @param widthA     Width of the first sprite's texture.
         * @param heightA    Height of the first sprite's texture.
         * @param dataA      Pixel color data of the first sprite.
         * @param transformB World transform of the second sprite.
         * @param widthB     Width of the second sprite's texture.
         * @param heightB    Height of the second sprite's texture.
         * @param dataB      Pixel color data of the second sprite.
         * @return True if non-transparent pixels overlap; false otherwise.
         */
        static bool IntersectPixelsSlow(
            Matrix transformA, int widthA, int heightA, const std::vector<Color>& dataA,
            Matrix transformB, int widthB, int heightB, const std::vector<Color>& dataB)
        {
            // Calculate a matrix which transforms from A's local space into
            // world space and then into B's local space
            Matrix transformAToB = transformA * Matrix::Invert(transformB);

            // For each row of pixels in A
            for (int yA = 0; yA < heightA; yA++)
            {
                // For each pixel in this row
                for (int xA = 0; xA < widthA; xA++)
                {
                    // Calculate this pixel's location in B
                    Vector2 positionInB =
                        Vector2::Transform(Vector2((float)xA, (float)yA), transformAToB);

                    // Round to the nearest pixel
                    int xB = (int)System::Math::Round(positionInB.X);
                    int yB = (int)System::Math::Round(positionInB.Y);

                    // If the pixel lies within the bounds of B
                    if (0 <= xB && xB < widthB &&
                        0 <= yB && yB < heightB)
                    {
                        // Get the colors of the overlapping pixels
                        Color colorA = dataA[xA + yA * widthA];
                        Color colorB = dataB[xB + yB * widthB];

                        // If both pixels are not completely transparent,
                        if (colorA.getAProperty() != 0 && colorB.getAProperty() != 0)
                        {
                            // then an intersection has been found
                            return true;
                        }
                    }
                }
            }

            // No intersection found
            return false;
        }

        /**
         * @brief Determines if there is overlap of the non-transparent pixels between two
         *        sprites, stepping through A's pixels incrementally.
         *
         * @param transformA World transform of the first sprite.
         * @param widthA     Width of the first sprite's texture.
         * @param heightA    Height of the first sprite's texture.
         * @param dataA      Pixel color data of the first sprite.
         * @param transformB World transform of the second sprite.
         * @param widthB     Width of the second sprite's texture.
         * @param heightB    Height of the second sprite's texture.
         * @param dataB      Pixel color data of the second sprite.
         * @return True if non-transparent pixels overlap; false otherwise.
         */
        static bool IntersectPixels(
            Matrix transformA, int widthA, int heightA, const std::vector<Color>& dataA,
            Matrix transformB, int widthB, int heightB, const std::vector<Color>& dataB)
        {
            // Calculate a matrix which transforms from A's local space into
            // world space and then into B's local space
            Matrix transformAToB = transformA * Matrix::Invert(transformB);

            // When a point moves in A's local space, it moves in B's local space with a
            // fixed direction and distance proportional to the movement in A.
            // This algorithm steps through A one pixel at a time along A's X and Y axes
            // Calculate the analogous steps in B:
            Vector2 stepX = Vector2::TransformNormal(Vector2::UnitX, transformAToB);
            Vector2 stepY = Vector2::TransformNormal(Vector2::UnitY, transformAToB);

            // Calculate the top left corner of A in B's local space
            // This variable will be reused to keep track of the start of each row
            Vector2 yPosInB = Vector2::Transform(Vector2::Zero, transformAToB);

            // For each row of pixels in A
            for (int yA = 0; yA < heightA; yA++)
            {
                // Start at the beginning of the row
                Vector2 posInB = yPosInB;

                // For each pixel in this row
                for (int xA = 0; xA < widthA; xA++)
                {
                    // Round to the nearest pixel
                    int xB = (int)System::Math::Round(posInB.X);
                    int yB = (int)System::Math::Round(posInB.Y);

                    // If the pixel lies within the bounds of B
                    if (0 <= xB && xB < widthB &&
                        0 <= yB && yB < heightB)
                    {
                        // Get the colors of the overlapping pixels
                        Color colorA = dataA[xA + yA * widthA];
                        Color colorB = dataB[xB + yB * widthB];

                        // If both pixels are not completely transparent,
                        if (colorA.getAProperty() != 0 && colorB.getAProperty() != 0)
                        {
                            // then an intersection has been found
                            return true;
                        }
                    }

                    // Move to the next pixel in the row
                    posInB += stepX;
                }

                // Move to the next row
                yPosInB += stepY;
            }

            // No intersection found
            return false;
        }
    };
}
