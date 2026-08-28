// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// DemoGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "Animation/Animation.hpp"
#include "Buttons/Checkbox.hpp"
#include "Models/Spaceship.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /**
     * @brief This is the main type for your game.
     */
    class Graphics3DSampleGame : public Microsoft::Xna::Framework::Game
    {
    private:
        static constexpr int buttonHeight = 70;
        static constexpr int buttonWidth = 70;
        static constexpr int buttonMargin = 15;

        GraphicsDeviceManager graphics;

        Spaceship spaceship;

        // XNA's components are reference types owned by Game.Components; the collection holds
        // borrowed pointers, so this port owns each one here and hands the raw pointer over.
        std::array<std::unique_ptr<Checkbox>, 3> lightEnablingButtons;
        std::unique_ptr<Checkbox> perpixelLightingButton;
        std::unique_ptr<Checkbox> animationButton;

        std::unique_ptr<Checkbox> backgroundTextureEnablingButton;

        float cameraFOV = 45; // Initial camera FOV (serves as a zoom level)
        float rotationXAmount = 0.0f;
        float rotationYAmount = 0.0f;
        // C#'s `float?`; std::optional is this port's established stand-in for a nullable.
        std::optional<float> prevLength;

        std::optional<Texture2D> background;

        std::optional<Animation> animation;
        Vector2 animationPosition;

        std::optional<SpriteBatch> spriteBatch;

    public:
        /**
         * @brief Provides SpriteBatch to components that draw sprites.
         * @return The game's SpriteBatch.
         */
        [[nodiscard]] SpriteBatch& getSpriteBatchProperty() { return *spriteBatch; }

        /** @brief Initialization that does not depend on GraphicsDevice. */
        Graphics3DSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Graphics3DSample.Graphics3DSampleGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Initialization that depends on GraphicsDevice but does not depend on Content. */
        void Initialize() override;

        /** @brief Loads content and creates graphics resources. */
        void LoadContent() override;

    private:
        /**
         * @brief Creates animation.
         * @return The animation described by Content/AnimationDef.xml.
         */
        Animation CreateAnimation();

        /** @brief Creates spaceship. */
        void CreateSpaceship();

        /** @brief Creates light enabling buttons. */
        void CreateLightEnablingButtons();

        /** @brief Creates per-pixel lighting button. */
        void CreatePerPixelLightingButton();

        /** @brief Creates animation button. */
        void CreateAnimationButton();

        /** @brief Create texture enabling button. */
        void CreateBackgroundTextureEnablingButton();

    protected:
        /**
         * @brief Updates spaceship rendering properties.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

    private:
        /** @brief Reads the gestures that rotate and zoom the camera. */
        void HandleInput();

        /**
         * @brief Gets spaceship rotation matrix.
         * @return The spaceship's rotation.
         */
        [[nodiscard]] Matrix GetRotationMatrix() const;

        /**
         * @brief Gets spaceship view matrix.
         * @return The camera's view matrix.
         */
        [[nodiscard]] Matrix GetViewMatrix() const;

    protected:
        /**
         * @brief Draws the game.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /** @brief Draws animation. */
        void DrawAnimation();
    };
}
