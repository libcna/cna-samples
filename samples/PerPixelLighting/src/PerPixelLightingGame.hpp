// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PerPixelLighting.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "SampleCamera.hpp"
#include "SampleGrid.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace PerPixelLightingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief The central class for the sample Game.
     */
    class PerPixelLighting : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        PerPixelLighting();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "PerPixelLightingSample.PerPixelLighting".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Draws a sample mesh using a single effect with a single technique.
         *
         * This pattern is very common in simple effect usage.
         *
         * @param sampleMesh The model to draw; the sample's meshes all have one part.
         */
        void DrawSampleMesh(Model* sampleMesh);

    protected:
        /** @brief Initialize the sample. */
        void Initialize() override;

        /** @brief Load the graphics content. */
        void LoadContent() override;

        /**
         * @brief Update the game world.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draw the current scene.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        void HandleInput(const GameTime& gameTime, const Input::GamePadState& gpState,
                         const Input::KeyboardState& kbState);

        /**
         * The effect parameters set in this function are shared between all of the rendered
         * elements in the scene.
         */
        void SetSharedEffectParameters();

        GraphicsDeviceManager graphics;
        std::optional<SampleArcBallCamera> camera;
        Vector2 safeBounds;
        Vector2 debugTextHeight;
        // XNA's Model is a reference type, so each slot is null until LoadContent runs.
        std::array<std::optional<Model>, 5> sampleMeshes;
        SampleGrid grid;
        int activeMesh = 0, activeEffect = 0, activeTechnique = 0, activeCombination = 0;
        static constexpr int effectTechniqueCombinations[5][2] =
            {
                {0, 0}, {1, 0}, {0, 1}, {1, 1}, {1, 2}
            };
        static constexpr int effectTechniqueCombinationCount = 5;
        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> debugTextFont;
        Input::GamePadState lastGpState;
        Input::KeyboardState lastKbState;

        static constexpr float specularPowerMinimum = 0.5f;
        static constexpr float specularPowerMaximum = 128.0f;
        static constexpr float specularIntensityMinimum = 0.01f;
        static constexpr float specularIntensityMaximum = 10.0f;

        // Example 1.1: Effect objects used for this example.
        //
        // Content.Load<Effect> hands back a shared_ptr in CNA, where XNA returns a reference the
        // ContentManager keeps alive; the array owns them for the game's lifetime either way.
        std::array<std::shared_ptr<Effect>, 2> effects;

        std::array<EffectParameter*, 2> worldParameter{};
        std::array<EffectParameter*, 2> viewParameter{};
        std::array<EffectParameter*, 2> projectionParameter{};

        std::array<EffectParameter*, 2> cameraPositionParameter{};
        std::array<EffectParameter*, 2> specularPowerParameter{};
        std::array<EffectParameter*, 2> specularIntensityParameter{};

        // Example 1.2: Data fields corresponding to the effect paramters.
        Matrix world;
        float specularPower = 0.0f, specularIntensity = 0.0f;
    };
}
