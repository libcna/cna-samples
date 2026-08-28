// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Cursor.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Ray.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace TrianglePicking
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Ray;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::RasterizerState;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Graphics::VertexPositionColor;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::KeyboardState;

    /**
     * @brief Sample showing how to implement per-triangle picking.
     */
    class TrianglePickingGame : public Microsoft::Xna::Framework::Game
    {
    private:
        static const std::array<std::string, 4> ModelFilenames;

        static constexpr float CameraRotateSpeed = 0.1f;
        static constexpr float CameraZoomSpeed = 0.01f;
        static constexpr float CameraMaxDistance = 10.0f;
        static constexpr float CameraMinDistance = 1.2f;
        static constexpr float CameraDefaultArc = -30.0f;
        static constexpr float CameraDefaultRotation = 225.0f;
        static constexpr float CameraDefaultDistance = 3.5f;

        GraphicsDeviceManager graphics;

        KeyboardState currentKeyboardState;
        GamePadState currentGamePadState;

        // XNA's SpriteBatch, SpriteFont, Model and BasicEffect are reference types, so the
        // originals are null until LoadContent assigns them; std::optional is this port's
        // established stand-in.
        std::optional<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> spriteFont;

        // Game.Components holds a borrowed pointer, so this port owns the component here.
        std::unique_ptr<Cursor> cursor;

        std::optional<Model> table;
        std::vector<Matrix> tableAbsoluteBoneTransforms;

        std::array<std::optional<Model>, 4> models;
        std::array<std::vector<Matrix>, 4> modelAbsoluteBoneTransforms;
        std::array<Matrix, 4> modelWorldTransforms;

        float cameraArc = CameraDefaultArc;
        float cameraRotation = CameraDefaultRotation;
        float cameraDistance = CameraDefaultDistance;

        Matrix viewMatrix;
        Matrix projectionMatrix;

        std::vector<std::string> insideBoundingSpheres;

        // C#'s `string pickedModelName` is null when nothing is picked; std::optional is this
        // port's established stand-in for a nullable reference.
        std::optional<std::string> pickedModelName;

        std::array<VertexPositionColor, 3> pickedTriangle;

        std::optional<BasicEffect> lineEffect;

        static const RasterizerState WireFrame;

    public:
        /** @brief Positions the models along the x axis and creates the cursor component. */
        TrianglePickingGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "TrianglePicking.TrianglePickingGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Builds the projection matrix now the graphics device exists. */
        void Initialize() override;

        /** @brief Loads the models, the font and the line effect. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

    private:
        /** @brief Runs the cursor ray against every model and remembers the closest triangle. */
        void UpdatePicking();

        /**
         * @brief Checks whether a ray intersects a model, using the per-triangle data the
         *        sample's own ContentProcessor attached to `Model.Tag`.
         *
         * @param ray The ray, in world space.
         * @param model The model to test.
         * @param modelTransform The matrix that places the model in the world.
         * @param insideBoundingSphere Set to true when the ray hits the model's bounding sphere.
         * @param vertex1 First vertex of the intersected triangle, in world space.
         * @param vertex2 Second vertex of the intersected triangle.
         * @param vertex3 Third vertex of the intersected triangle.
         * @return The distance to the closest intersection, or no value when there is none.
         */
        [[nodiscard]] static std::optional<float> RayIntersectsModel(
            Ray ray, Model& model, const Matrix& modelTransform,
            bool& insideBoundingSphere, Vector3& vertex1, Vector3& vertex2, Vector3& vertex3);

        /**
         * @brief Checks whether a ray intersects a triangle, by the Möller-Trumbore method.
         *
         * @param ray The ray, in the triangle's own space.
         * @param vertex1 First vertex of the triangle.
         * @param vertex2 Second vertex of the triangle.
         * @param vertex3 Third vertex of the triangle.
         * @param result Receives the distance along the ray, or no value when there is no hit.
         */
        static void RayIntersectsTriangle(const Ray& ray, const Vector3& vertex1,
                                          const Vector3& vertex2, const Vector3& vertex3,
                                          std::optional<float>& result);

    protected:
        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /** @brief Draws the picked triangle as a magenta wireframe on top of everything. */
        void DrawPickedTriangle();

        /** @brief Draws the bounding-sphere list and the picked model's name. */
        void DrawText();

        /**
         * @brief Draws a model with its bone transforms and the default lighting rig.
         *
         * @param model The model to draw.
         * @param worldTransform Where to draw it.
         * @param absoluteBoneTransforms The model's absolute bone transforms.
         */
        void DrawModel(Model& model, const Matrix& worldTransform,
                       const std::vector<Matrix>& absoluteBoneTransforms);

        /** @brief Reads the keyboard and gamepad, and exits on Escape or Back. */
        void HandleInput();

        /**
         * @brief Moves the camera from the keyboard and gamepad.
         * @param gameTime Provides a snapshot of timing values.
         */
        void UpdateCamera(const GameTime& gameTime);
    };
}
