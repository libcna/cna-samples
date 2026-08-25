// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Block.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Math.hpp"
#include "System/Random.hpp"

namespace TransformedCollision
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using System::Collections::Generic::List;

    /**
     * @brief This is the main type for your game.
     */
    class TransformedCollisionGame : public Microsoft::Xna::Framework::Game
    {
        GraphicsDeviceManager graphics;

        // The images we will draw
        Texture2D personTexture;
        Texture2D blockTexture;

        // The color data for the images; used for per pixel collision
        std::vector<Color> personTextureData;
        std::vector<Color> blockTextureData;

        // The images will be drawn with this SpriteBatch
        std::unique_ptr<SpriteBatch> spriteBatch;

        // Person
        Vector2 personPosition;
        static constexpr int PersonMoveSpeed = 5;

        // Blocks
        List<Block> blocks;
        float BlockSpawnProbability = 0.01f;
        static constexpr int BlockFallSpeed = 1;
        static constexpr float BlockRotateSpeed = 0.005f;
        Vector2 blockOrigin;

        System::Random random;

        // For when a collision is detected
        bool personHit = false;

        // The sub-rectangle of the drawable area which should be visible on all TVs
        Rectangle safeBounds;
        // Percentage of the screen on every side is the safe area
        static constexpr float SafeAreaPortion = 0.05f;

    public:
        /**
         * @brief Constructs the game and selects its content root directory.
         */
        TransformedCollisionGame()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
        }

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "TransformedCollision.TransformedCollisionGame".
         */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "TransformedCollision.TransformedCollisionGame";
            return name;
        }

    protected:
        /**
         * @brief Allows the game to perform any initialization it needs to before starting to
         *        run. This is where it can query for any required services and load any
         *        non-graphic related content. Calling base.Initialize will enumerate through
         *        any components and initialize them as well.
         */
        void Initialize() override
        {
            Game::Initialize();

            // Calculate safe bounds based on current resolution
            Viewport viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
            // C# allows a floating-point expression to be evaluated with more precision than its
            // type, and the 32-bit reference build of this sample does exactly that: it keeps
            // `viewport.Width * (1 - 2 * SafeAreaPortion)` in an extended-precision register and
            // truncates that value, so the original's safe area is 719x431 and not the 720x432 a
            // float-rounded product yields. The widening casts below reproduce the reference
            // build's own arithmetic, and with it the original person rest position and the exact
            // horizontal clamp range the player can reach.
            safeBounds = Rectangle(
                (int)((double)viewport.getWidthProperty() * SafeAreaPortion),
                (int)((double)viewport.getHeightProperty() * SafeAreaPortion),
                (int)((double)viewport.getWidthProperty() * (1 - 2 * SafeAreaPortion)),
                (int)((double)viewport.getHeightProperty() * (1 - 2 * SafeAreaPortion)));
            // Start the player in the center along the bottom of the screen
            personPosition.X = (float)((safeBounds.Width - personTexture.getWidthProperty()) / 2);
            personPosition.Y = (float)(safeBounds.Height - personTexture.getHeightProperty());
        }

        /**
         * @brief Load your graphics content.
         */
        void LoadContent() override
        {
            // Load textures
            blockTexture = getContentProperty().Load<Texture2D>("SpinnerBlock");
            personTexture = getContentProperty().Load<Texture2D>("Person");

            // Extract collision data
            blockTextureData =
                std::vector<Color>(blockTexture.getWidthProperty() * blockTexture.getHeightProperty());
            blockTexture.GetData(blockTextureData.data(), (int)blockTextureData.size());
            personTextureData =
                std::vector<Color>(personTexture.getWidthProperty() * personTexture.getHeightProperty());
            personTexture.GetData(personTextureData.data(), (int)personTextureData.size());

            // Calculate the block origin
            blockOrigin =
                Vector2((float)(blockTexture.getWidthProperty() / 2),
                        (float)(blockTexture.getHeightProperty() / 2));

            // Create a sprite batch to draw those textures
            spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        }

        /**
         * @brief Allows the game to run logic such as updating the world,
         *        checking for collisions, gathering input and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override
        {
            // Get input
            KeyboardState keyboard = Keyboard::GetState();
            GamePadState gamePad = GamePad::GetState(PlayerIndex::One);

            // Allows the game to exit
            if (gamePad.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
                keyboard.IsKeyDown(Keys::Escape))
            {
                this->Exit();
            }

            // Move the player left and right with arrow keys or d-pad
            if (keyboard.IsKeyDown(Keys::Left) ||
                gamePad.getDPadProperty().getLeftProperty() == ButtonState::Pressed)
            {
                personPosition.X -= PersonMoveSpeed;
            }
            if (keyboard.IsKeyDown(Keys::Right) ||
                gamePad.getDPadProperty().getRightProperty() == ButtonState::Pressed)
            {
                personPosition.X += PersonMoveSpeed;
            }

            // Prevent the person from moving off of the screen
            personPosition.X = MathHelper::Clamp(personPosition.X,
                (float)safeBounds.getLeftProperty(),
                (float)(safeBounds.getRightProperty() - personTexture.getWidthProperty()));

            // Update the person's transform
            Matrix personTransform =
                Matrix::CreateTranslation(Vector3(personPosition, 0.0f));

            // Spawn new falling blocks
            if (random.NextDouble() < BlockSpawnProbability)
            {
                Block newBlock;

                // at a random position just above the screen
                float x = (float)random.NextDouble() *
                    (getWindowProperty().getClientBoundsProperty().Width - blockTexture.getWidthProperty());
                newBlock.Position = Vector2(x, (float)(-blockTexture.getHeightProperty()));

                // with a random rotation
                newBlock.Rotation = (float)random.NextDouble() * MathHelper::TwoPi;

                blocks.Add(newBlock);
            }

            // Get the bounding rectangle of the person
            Rectangle personRectangle =
                Rectangle((int)personPosition.X, (int)personPosition.Y,
                personTexture.getWidthProperty(), personTexture.getHeightProperty());

            // Update each block
            personHit = false;
            for (int i = 0; i < blocks.getCountProperty(); i++)
            {
                // Animate this block falling
                // Block is a C# reference type, so the original mutates the stored object
                // through `blocks[i].Position += ...`. List<T> here holds elements by value,
                // so the same step is a read, a mutation and a write-back. Nothing else in
                // this loop aliases the element, so the two are equivalent.
                Block block = blocks.getItem(i);
                block.Position += Vector2(0.0f, BlockFallSpeed);
                block.Rotation += BlockRotateSpeed;
                blocks.setItem(i, block);

                // Build the block's transform
                Matrix blockTransform =
                    Matrix::CreateTranslation(Vector3(-blockOrigin, 0.0f)) *
                    // Matrix::CreateScale(block.Scale) *  would go here
                    Matrix::CreateRotationZ(blocks.getItem(i).Rotation) *
                    Matrix::CreateTranslation(Vector3(blocks.getItem(i).Position, 0.0f));

                // Calculate the bounding rectangle of this block in world space
                Rectangle blockRectangle = CalculateBoundingRectangle(
                         Rectangle(0, 0, blockTexture.getWidthProperty(), blockTexture.getHeightProperty()),
                         blockTransform);

                // The per-pixel check is expensive, so check the bounding rectangles
                // first to prevent testing pixels when collisions are impossible.
                if (personRectangle.Intersects(blockRectangle))
                {
                    // Check collision with person
                    if (IntersectPixels(personTransform, personTexture.getWidthProperty(),
                                        personTexture.getHeightProperty(), personTextureData,
                                        blockTransform, blockTexture.getWidthProperty(),
                                        blockTexture.getHeightProperty(), blockTextureData))
                    {
                        personHit = true;
                    }
                }

                // Remove this block if it have fallen off the screen
                if (blocks.getItem(i).Position.Y >
                    getWindowProperty().getClientBoundsProperty().Height + blockOrigin.Length())
                {
                    blocks.RemoveAt(i);

                    // When removing a block, the next block will have the same index
                    // as the current block. Decrement i to prevent skipping a block.
                    i--;
                }
            }

            Game::Update(gameTime);
        }

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override
        {
            GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

            // Change the background to red when the person was hit by a block
            if (personHit)
            {
                device.Clear(Color::Red);
            }
            else
            {
                device.Clear(Color::CornflowerBlue);
            }


            spriteBatch->Begin();

            // Draw person
            spriteBatch->Draw(personTexture, personPosition, Color::White);

            // Draw blocks
            for (const Block& block : blocks)
            {
                spriteBatch->Draw(blockTexture, block.Position, std::nullopt, Color::White,
                    block.Rotation, blockOrigin, 1.0f, SpriteEffects::None, 0.0f);
            }

            spriteBatch->End();


            Game::Draw(gameTime);
        }

    public:
        /**
         * @brief Determines if there is overlap of the non-transparent pixels
         *        between two sprites.
         *
         * @param rectangleA Bounding rectangle of the first sprite.
         * @param dataA      Pixel data of the first sprite.
         * @param rectangleB Bouding rectangle of the second sprite.
         * @param dataB      Pixel data of the second sprite.
         * @return True if non-transparent pixels overlap; false otherwise.
         */
        static bool IntersectPixels(Rectangle rectangleA, const std::vector<Color>& dataA,
                                    Rectangle rectangleB, const std::vector<Color>& dataB)
        {
            // Find the bounds of the rectangle intersection
            int top = System::Math::Max(rectangleA.getTopProperty(), rectangleB.getTopProperty());
            int bottom = System::Math::Min(rectangleA.getBottomProperty(), rectangleB.getBottomProperty());
            int left = System::Math::Max(rectangleA.getLeftProperty(), rectangleB.getLeftProperty());
            int right = System::Math::Min(rectangleA.getRightProperty(), rectangleB.getRightProperty());

            // Check every point within the intersection bounds
            for (int y = top; y < bottom; y++)
            {
                for (int x = left; x < right; x++)
                {
                    // Get the color of both pixels at this point
                    Color colorA = dataA[(x - rectangleA.getLeftProperty()) +
                                         (y - rectangleA.getTopProperty()) * rectangleA.Width];
                    Color colorB = dataB[(x - rectangleB.getLeftProperty()) +
                                         (y - rectangleB.getTopProperty()) * rectangleB.Width];

                    // If both pixels are not completely transparent,
                    if (colorA.getAProperty() != 0 && colorB.getAProperty() != 0)
                    {
                        // then an intersection has been found
                        return true;
                    }
                }
            }

            // No intersection found
            return false;
        }

        /**
         * @brief Determines if there is overlap of the non-transparent pixels between two
         *        sprites.
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

        /**
         * @brief Calculates an axis aligned rectangle which fully contains an arbitrarily
         *        transformed axis aligned rectangle.
         *
         * @param rectangle Original bounding rectangle.
         * @param transform World transform of the rectangle.
         * @return A new rectangle which contains the trasnformed rectangle.
         */
        static Rectangle CalculateBoundingRectangle(Rectangle rectangle,
                                                    Matrix transform)
        {
            // Get all four corners in local space
            Vector2 leftTop((float)rectangle.getLeftProperty(), (float)rectangle.getTopProperty());
            Vector2 rightTop((float)rectangle.getRightProperty(), (float)rectangle.getTopProperty());
            Vector2 leftBottom((float)rectangle.getLeftProperty(), (float)rectangle.getBottomProperty());
            Vector2 rightBottom((float)rectangle.getRightProperty(), (float)rectangle.getBottomProperty());

            // Transform all four corners into work space
            Vector2::Transform(leftTop, transform, leftTop);
            Vector2::Transform(rightTop, transform, rightTop);
            Vector2::Transform(leftBottom, transform, leftBottom);
            Vector2::Transform(rightBottom, transform, rightBottom);

            // Find the minimum and maximum extents of the rectangle in world space
            Vector2 min = Vector2::Min(Vector2::Min(leftTop, rightTop),
                                       Vector2::Min(leftBottom, rightBottom));
            Vector2 max = Vector2::Max(Vector2::Max(leftTop, rightTop),
                                       Vector2::Max(leftBottom, rightBottom));

            // Return that as a rectangle
            return Rectangle((int)min.X, (int)min.Y,
                             (int)(max.X - min.X), (int)(max.Y - min.Y));
        }
    };
}
