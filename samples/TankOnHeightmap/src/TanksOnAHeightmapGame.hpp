// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// TanksOnAHeightmap.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "HeightMapInfo.hpp"
#include "Tank.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace TanksOnAHeightmap
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::Model;

    /** @brief Demonstrates driving an animated tank over a generated heightmap. */
    class TanksOnAHeightmapGame : public Microsoft::Xna::Framework::Game
    {
    private:
        static const Vector3 CameraPositionOffset;
        static const Vector3 CameraTargetOffset;

        GraphicsDeviceManager graphics;
        std::optional<Model> terrain;
        Tank tank;
        Matrix projectionMatrix;
        Matrix viewMatrix;
        HeightMapInfo* heightMapInfo = nullptr;

    public:
        /** @brief Creates the graphics manager, content root, tank and custom reader registration. */
        TanksOnAHeightmapGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return `TanksOnAHeightmap.TanksOnAHeightmapGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Builds the projection matrix after the graphics device is created. */
        void Initialize() override;

        /** @brief Loads the generated terrain, its `HeightMapInfo` tag and the tank. */
        void LoadContent() override;

        /**
         * @brief Updates input and the chase camera.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

    private:
        /** @brief Places the camera behind the tank while keeping it above the terrain. */
        void UpdateCamera();

    protected:
        /**
         * @brief Draws the generated terrain followed by the animated tank.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /**
         * @brief Draws the terrain model with default lighting and black fog.
         * @param model The model to draw.
         */
        void DrawModel(Model& model);

        /** @brief Handles the original keyboard/gamepad exit and tank controls. */
        void HandleInput();
    };
}
