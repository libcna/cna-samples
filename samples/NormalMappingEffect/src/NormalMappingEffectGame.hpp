// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NormalMappingEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace NormalMappingEffect
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample shows how to render a model using a custom effect.
     */
    class NormalMappingEffectGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        NormalMappingEffectGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "NormalMappingEffect.NormalMappingEffectGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Load your graphics content. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /** Handles input for quitting the game. */
        void HandleInput();

        /** Handles camera input. */
        void UpdateCamera(const GameTime& gameTime);

        GraphicsDeviceManager graphics;

        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamePadState;
        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;

        // XNA's Model is a reference type, so the original's field is null until
        // LoadContent runs.
        std::optional<Model> model;

        // the next 4 fields are inputs to the normal mapping effect, and will be set
        // at load time.  change these to change the light properties to modify
        // the appearance of the model.
        Vector4 lightColor{1, 1, 1, 1};
        Vector4 ambientLightColor{.2f, .2f, .2f, 1};
        float shininess = .3f;
        float specularPower = 4.0f;

        // the sample arc ball camera values
        float cameraArc = 0;
        float cameraRotation = 45;
        float cameraDistance = 1500;

        // the light rotates around the origin using these 3 constants.  the light
        // position is set in the draw function.
        static constexpr float LightHeight = 600;
        static constexpr float LightRotationRadius = 800;
        static constexpr float LightRotationSpeed = .5f;
        bool rotateLight = true;
        float lightRotation = 0;
    };
}
