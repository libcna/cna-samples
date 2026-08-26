// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShadowMapping.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace ShadowMapping
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing how to implement a simple shadow mapping technique where the shadow
     *        map always contains the contents of the viewing frustum.
     */
    class ShadowMappingGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        ShadowMappingGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "ShadowMapping.ShadowMappingGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /**
         * @brief LoadContent will be called once per game and is the place to load all of your
         *        content.
         */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic such as updating the world, checking for
         *        collisions, gathering input, and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /**
         * Creates the WorldViewProjection matrix from the perspective of the light using the
         * cameras bounding frustum to determine what is visible in the scene.
         */
        [[nodiscard]] Matrix CreateLightViewProjectionMatrix();

        /**
         * Renders the scene to the floating point render target then sets the texture for use
         * when drawing the scene.
         */
        void CreateShadowMap();

        /** Renders the scene using the shadow map to darken the shadow areas. */
        void DrawWithShadowMap();

        /** Helper function to draw a model. */
        void DrawModel(Model& model, bool createShadowMap);

        /** Render the shadow map texture to the screen. */
        void DrawShadowMapToScreen();

        /** Handles input for quitting the game. */
        void HandleInput(const GameTime& gameTime);

        /** Handles input for moving the camera. */
        void UpdateCamera(const GameTime& gameTime);

        // The size of the shadow map
        // The larger the size the more detail we will have for our entire scene
        static constexpr int shadowMapWidthHeight = 2048;

        static constexpr int windowWidth = 800;
        static constexpr int windowHeight = 480;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;

        // Starting position and direction of our camera
        Vector3 cameraPosition{0, 70, 100};
        Vector3 cameraForward{0, -0.4472136f, -0.8944272f};
        BoundingFrustum cameraFrustum{Matrix::getIdentityProperty()};

        // Light direction
        Vector3 lightDir{-0.3333333f, 0.6666667f, 0.6666667f};

        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;

        // Our two models in the scene. XNA's Model is a reference type, so the original's
        // fields are null until LoadContent runs.
        std::optional<Model> gridModel;
        std::optional<Model> dudeModel;

        float rotateDude = 0.0f;

        // The shadow map render target
        std::unique_ptr<RenderTarget2D> shadowRenderTarget;

        // Transform matrices
        Matrix world;
        Matrix view;
        Matrix projection;

        // ViewProjection matrix from the lights perspective
        Matrix lightViewProjection;
    };
}
