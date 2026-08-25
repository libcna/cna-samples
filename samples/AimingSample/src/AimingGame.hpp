// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Aiming
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief This is the main type for your game.
     */
    class AimingGame : public Microsoft::Xna::Framework::Game
    {
        /** how fast can the cat move? this is in terms of pixels per frame. */
        static constexpr float CatSpeed = 10.0f;

        /** how fast can the spot light turn? this is in terms of radians per frame. */
        static constexpr float SpotlightTurnSpeed = 0.025f;

        GraphicsDeviceManager graphics;

        // we'll need a spriteBatch to draw the spotlight and cat.
        std::unique_ptr<SpriteBatch> spriteBatch;

        // these four values control the spotlight and how it draws.
        // first is the actual sprite that we'll draw to represent the spotlight.
        Texture2D spotlightTexture;
        // next is the position of the spotlight on the screen.
        Vector2 spotlightPosition = Vector2();
        // the origin of the spotlightTexture. The spotlight will rotate around this
        // point.
        Vector2 spotlightOrigin = Vector2();
        // the angle that the spotlight is currently facing. this is in radians. a value
        // of 0 points to the right.
        float spotlightAngle = 0.0f;

        // these next three variables control the cat. catTexture is the sprite that
        // represents the cat...
        Texture2D catTexture;
        // ...catPosition is the cat's position on the screen...
        Vector2 catPosition = Vector2();
        // ...and catOrigin is the origin of catTexture. the sprite will be drawn
        // centered around this value.
        Vector2 catOrigin = Vector2();

    public:
        /** @brief Constructs the game and selects the back buffer for this platform. */
        AimingGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Aiming.AimingGame".
         */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Centres the spotlight and places the cat a quarter of the way across. */
        void Initialize() override;

        /** @brief Loads the two textures and works out their origins. */
        void LoadContent() override;

        /**
         * @brief Reads input, clamps the cat and turns the spotlight towards it.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        [[nodiscard]] static float TurnToFace(Vector2 position, Vector2 faceThis,
                                              float currentAngle, float turnSpeed);
        [[nodiscard]] static float WrapAngle(float radians);
        void HandleInput();
    };
}
