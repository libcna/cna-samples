// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// HeightmapCollision.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "HeightMapInfo.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace HeightmapCollision
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::Model;

    /**
     * @brief Shows how to keep an object on the surface of a heightmap the content pipeline built.
     */
    class HeightmapCollisionGame : public Microsoft::Xna::Framework::Game
    {
    private:
        static constexpr float SphereVelocity = 2;
        static constexpr float SphereTurnSpeed = 0.025f;
        static constexpr float SphereRadius = 12.0f;

        static const Vector3 CameraPositionOffset;
        static const Vector3 CameraTargetOffset;

        GraphicsDeviceManager graphics;

        // XNA's Model is a reference type, so the originals are null until LoadContent assigns
        // them; std::optional is this port's established stand-in.
        std::optional<Model> terrain;

        Matrix projectionMatrix;
        Matrix viewMatrix;

        Vector3 spherePosition;
        float sphereFacingDirection = 0.0f;
        Matrix sphereRollingMatrix = Matrix::getIdentityProperty();

        std::optional<Model> sphere;

        // Borrowed from the terrain model's Tag, which owns it.
        HeightMapInfo* heightMapInfo = nullptr;

    public:
        /** @brief Creates the graphics device manager and registers the sample's own reader. */
        HeightmapCollisionGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "HeightmapCollision.HeightmapCollisionGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Builds the projection matrix now the graphics device exists. */
        void Initialize() override;

        /** @brief Loads the terrain and the sphere, and takes the heightmap off the terrain's Tag. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

    private:
        /** @brief Places the camera behind the sphere, lifted clear of the terrain. */
        void UpdateCamera();

    protected:
        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /**
         * @brief Draws a model with the default lighting rig and the sample's fog settings.
         *
         * @param model The model to draw.
         * @param worldMatrix Where to draw it.
         */
        void DrawModel(Model& model, const Matrix& worldMatrix);

        /** @brief Reads the keyboard and gamepad, and rolls the sphere over the terrain. */
        void HandleInput();
    };
}
