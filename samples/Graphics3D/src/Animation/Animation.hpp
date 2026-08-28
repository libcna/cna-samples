// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Animation.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/TimeSpan.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /**
     * @brief Plays a sprite-sheet animation.
     */
    class Animation
    {
    private:
        // The texture with animation frames
        Texture2D animationTexture;
        // The size and structure of whole frames sheet in animationTexture. The animationTexture
        // could hold animation sequence organized in multiple rows and multiple columns, that's
        // why animation engine should know how the frames are organized inside a frames sheet
        Point sheetSize;
        // Amount of time between frames
        System::TimeSpan frameInterval;
        // Time passed since last frame
        System::TimeSpan nextFrame;

    public:
        // Current frame in the animation sequence
        Point currentFrame;
        // The size of single frame inside the animationTexture
        Point frameSize;

        /**
         * @brief Constructor of an animation class.
         *
         * @param frameSheet Texture with animation frames sheet.
         * @param size Single frame size.
         * @param frameSheetSize The whole frame sheet size.
         * @param interval Interval between progressing to the next frame.
         */
        Animation(const Texture2D& frameSheet, const Point& size, const Point& frameSheetSize,
                  const System::TimeSpan& interval);

        /**
         * @brief Updates the animation progress.
         *
         * @param gameTime Provides a snapshot of timing values.
         * @return True if the animation progressed; in such case the caller could update the
         *         position of the animated character.
         */
        bool Update(const GameTime& gameTime);

        /**
         * @brief Rendering of the animation.
         *
         * @param spriteBatch SpriteBatch in which current frame will be rendered.
         * @param position The position of current frame.
         * @param spriteEffect SpriteEffect to apply on current frame.
         */
        void Draw(SpriteBatch& spriteBatch, const Vector2& position, SpriteEffects spriteEffect);

        /**
         * @brief Rendering of the animation.
         *
         * @param spriteBatch SpriteBatch in which current frame will be rendered.
         * @param position The position of the current frame.
         * @param scale Scale factor to apply on the current frame.
         * @param spriteEffect SpriteEffect to apply on the current frame.
         */
        void Draw(SpriteBatch& spriteBatch, const Vector2& position, float scale,
                  SpriteEffects spriteEffect);
    };
}
