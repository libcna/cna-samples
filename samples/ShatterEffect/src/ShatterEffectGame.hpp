// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShatterEffectGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

namespace ShatterSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief This is the main type for your game.
     */
    class ShatterEffectGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        ShatterEffectGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "ShatterSample.ShatterEffectGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Load your graphics content. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic such as updating the world, checking for collisions,
         *        gathering input and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /** Set the required values in the shader. */
        void SetupEffect(const std::vector<Matrix>& transforms, ModelMesh& mesh,
                         ModelMeshPart& part);

        GraphicsDeviceManager graphics;

        Vector3 lightPosition = Vector3::UnitY;
        Vector4 ambientColor = Color::DarkGray.ToVector4();
        Vector4 diffuseColor = Color::White.ToVector4();
        Vector4 specularColor = Color::White.ToVector4();
        float specularPower = 50;

        float time = 0;

        static constexpr float translationRate = 50;
        static constexpr float rotationRate = MathHelper::Pi * 3;
        static constexpr float duration = 2.0f;

        // XNA's Model, SpriteFont and SpriteBatch are reference types, so the originals' fields
        // are null until LoadContent runs.
        std::optional<Model> model;
        std::optional<SpriteFont> font;
        std::optional<SpriteBatch> spriteBatch;

        Matrix view;
        Matrix projection;
        Vector3 cameraPosition{-696, 429, 835};
        Vector3 targetPosition{0, 60, 0};
    };
}
