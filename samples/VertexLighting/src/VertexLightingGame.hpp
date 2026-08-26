// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// VertexLighting.cs
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
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace VertexLightingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief The central class for the sample Game.
     */
    class VertexLighting : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        VertexLighting();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "VertexLightingSample.VertexLighting".
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
        /**
         * Obtains EffectParameter objects from the Effect objects. The EffectParameters are
         * handles to the values in the shaders and are effectively how your C# code and your
         * shader code communicate.
         */
        void GetEffectParameters();

        void HandleInput(const GameTime& gameTime, const Input::GamePadState& gpState,
                         const Input::KeyboardState& kbState);

        /**
         * The effect parameters set in this function are shared between all of the rendered
         * elements in the scene.
         */
        void SetSharedEffectParameters();

        GraphicsDeviceManager graphics;
        std::optional<SampleArcBallCamera> camera;
        // XNA's Model is a reference type, so each slot is null until LoadContent runs; the
        // grid and the camera are likewise constructed there, not in the constructor.
        std::array<std::optional<Model>, 5> sampleMeshes;
        std::optional<SampleGrid> grid;
        int activeMesh = 0;
        bool enableAdvancedEffect = true;
        Input::GamePadState lastGpState;
        Input::KeyboardState lastKbState;

        // Example 1.1: Effect objects used for this example.
        //
        // Content.Load<Effect> hands back a shared_ptr in CNA, where XNA returns a reference the
        // ContentManager keeps alive; the game owns them for its lifetime either way.
        std::shared_ptr<Effect> noLightingEffect;
        std::shared_ptr<Effect> vertexLightingEffect;
        EffectParameter* projectionParameter = nullptr;
        EffectParameter* viewParameter = nullptr;
        EffectParameter* worldParameter = nullptr;
        EffectParameter* lightColorParameter = nullptr;
        EffectParameter* lightDirectionParameter = nullptr;
        EffectParameter* ambientColorParameter = nullptr;

        // Example 1.2: Data fields corresponding to the effect paramters.
        Matrix world, view, projection;
        Vector3 diffuseLightDirection;
        Vector4 diffuseLightColor;
        Vector4 ambientLightColor;
    };
}
