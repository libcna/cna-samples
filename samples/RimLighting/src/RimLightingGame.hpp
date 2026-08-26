// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game1.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Button.hpp"
#include "ModelViewerCamera.hpp"
#include "Slidebar.hpp"
#include "UIElement.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"

namespace RimLighting
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief This is the main type for your game.
     */
    class SampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        SampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "RimLighting.SampleGame".
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
        void ButtonToggleWorldCameraOnClick();
        void SlideBarEnvironmentMapAmountOnValueChanged();
        void SlideBarFresnelFactorOnValueChanged();

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont, Model, TextureCube and Texture2D are reference types, so the
        // original's fields are null until LoadContent runs.
        std::optional<SpriteFont> spriteFont;

        // Whether we are rotating world or camera
        enum class RotatingMode
        {
            RotatingWorld,
            RotatingCamera
        };
        RotatingMode rotatingMode = RotatingMode::RotatingWorld;

        // Button to switch between the two rotating modes above
        std::unique_ptr<Button> buttonToggleWorldCamera;

        // Slidebars to tweak the effect
        std::unique_ptr<Slidebar> slideBarEnvironmentMapAmount;
        std::unique_ptr<Slidebar> slideBarFresnelFactor;

        // List of all UI Elements, which facilitates calling .Update() and .Draw() on the UI
        // elements. XNA's List<UIElement> holds references the controls' own fields also point
        // at; here the controls are owned by their unique_ptr fields and this list observes them.
        std::vector<UIElement*> uiElementList;

        static const Vector3 CameraInitPosition;
        ModelViewerCamera modelViewerCamera;

        // The mesh to be rendered
        std::optional<Model> model;

        // The cube texture for rimlighting effect
        std::optional<TextureCube> texureRimlightingCube;

        // Default texture for the mesh
        std::optional<Texture2D> texure2D;

        Matrix matrixWorld = Matrix::getIdentityProperty();
        Matrix matrixView;
        Vector2 vec2RotWorld = Vector2::Zero;
        Vector2 vec2RotCamera = Vector2::Zero;
    };
}
