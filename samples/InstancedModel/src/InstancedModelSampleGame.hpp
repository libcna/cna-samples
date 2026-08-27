// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// InstancedModelSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "System/TimeSpan.hpp"

#include "SpinningInstance.hpp"

namespace InstancedModelSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Enum describes the various possible techniques that can be chosen to implement
     *        instancing.
     */
    enum class InstancingTechnique
    {
        HardwareInstancing,
        NoInstancing,
        NoInstancingOrStateBatching
    };

    /**
     * @brief Sample showing how to efficiently render many copies of a model, using hardware
     *        instancing to draw more than one copy in a single GPU batch.
     */
    class InstancedModelSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        InstancedModelSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "InstancedModelSample.InstancedModelSampleGame".
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
        /** Efficiently draws several copies of a piece of geometry using hardware instancing. */
        void DrawModelHardwareInstancing(Model& model, const std::vector<Matrix>& modelBones,
                                         const std::vector<Matrix>& instances,
                                         const Matrix& view, const Matrix& projection);

        /** Draws several copies of a piece of geometry without any special GPU instancing. */
        void DrawModelNoInstancing(Model& model, const std::vector<Matrix>& modelBones,
                                   const std::vector<Matrix>& instances,
                                   const Matrix& view, const Matrix& projection);

        /** The deliberately inefficient variant, kept for performance comparison. */
        void DrawModelNoInstancingOrStateBatching(Model& model,
                                                  const std::vector<Matrix>& modelBones,
                                                  const std::vector<Matrix>& instances,
                                                  const Matrix& view, const Matrix& projection);

        /** Helper for drawing the help text overlay. */
        void DrawOverlayText();

        /** Handles input for quitting or changing settings. */
        void HandleInput();

        GraphicsDeviceManager graphics;

        std::optional<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> spriteFont;

        // Instanced model rendering.
        InstancingTechnique instancingTechnique = InstancingTechnique::HardwareInstancing;

        static constexpr int InitialInstanceCount = 1000;

        std::vector<SpinningInstance> instances;
        std::vector<Matrix> instanceTransforms;
        std::optional<Model> instancedModel;
        std::vector<Matrix> instancedModelBones;
        std::unique_ptr<DynamicVertexBuffer> instanceVertexBuffer;

        // To store instance transform matrices in a vertex buffer, we use this custom
        // vertex type which encodes 4x4 matrices as a set of four Vector4 values.
        static const VertexDeclaration instanceVertexDeclaration;

        // Measure the framerate.
        int frameRate = 0;
        int frameCounter = 0;
        System::TimeSpan elapsedTime;

        // Input handling.
        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamePadState;
        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;
    };
}
