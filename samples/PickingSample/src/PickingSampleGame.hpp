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
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Ray.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

namespace PickingSample
{
    using Microsoft::Xna::Framework::BoundingSphere;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Ray;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;

    /**
     * @brief This sample shows how to see if a user's cursor is over an object, and how to find
     *        out where on the screen an object is.
     *
     * The sample puts several objects on a table. If the cursor is on the object, the object's
     * name is displayed. See the accompanying doc file for more information.
     */
    class PickingSampleGame : public Microsoft::Xna::Framework::Game
    {
    private:
        // ModelFilenames is the list of models that we will be putting on top of the
        // table. These strings will be used as arguments to content.Load<Model> and
        // will be drawn when the cursor is over an object.
        static const std::array<std::string, 4> ModelFilenames;

        // the following constants control the speed at which the camera moves
        // how fast does the camera move up, down, left, and right?
        static constexpr float CameraRotateSpeed = 0.01f;

        // the following constants control how the camera's default position
        static constexpr float CameraDefaultArc = -15.0f;
        static constexpr float CameraDefaultRotation = 185.0f;
        static constexpr float CameraDefaultDistance = 4.3f;

        GraphicsDeviceManager graphics;

        // a SpriteBatch and SpriteFont, which we will use to draw the objects' names
        // when they are selected.
        // XNA's SpriteBatch, SpriteFont and Model are reference types, so the originals are null
        // until LoadContent assigns them; std::optional is this port's established stand-in.
        std::optional<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> spriteFont;

        // The cursor is used to tell what the user's pointer/mouse is over. The cursor
        // is moved with the left thumbstick. On windows, the mouse can be used as well.
        // Game.Components holds borrowed pointers, so this port owns the component here.
        std::unique_ptr<Cursor> cursor;

        // the table that all of the objects are drawn on, and table model's
        // absoluteBoneTransforms. Since the table is not animated, these can be
        // calculated once and saved.
        std::optional<Model> table;
        std::vector<Matrix> tableAbsoluteBoneTransforms;

        // these are the models that we will draw on top of the table. we'll store them
        // and their bone transforms in arrays. Again, since these models aren't
        // animated, we can calculate their bone transforms once and save the result.
        std::array<std::optional<Model>, 4> models;
        std::array<std::vector<Matrix>, 4> modelAbsoluteBoneTransforms;
        // each model will need one more matrix: a world transform. This matrix will be
        // used to place each model at a different location in the world.
        std::array<Matrix, 4> modelWorldTransforms;

        Matrix viewMatrix;
        Matrix projectionMatrix;

        // this variable will store the current rotation value as the camera
        // rotates around the scene
        float cameraRotation = CameraDefaultRotation;

        // this variable will tell our game whether or not to draw a mesh's bounding sphere
        bool drawBoundingSphere = true;

    public:
        /** @brief Creates the graphics device manager and points the ContentManager at Content. */
        PickingSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "PickingSample.PickingSampleGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Places each model along the x axis and adds the cursor component. */
        void Initialize() override;

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
        /** @brief Draws the name of every model the cursor is currently over. */
        void DrawModelNames();

        /**
         * @brief DrawModel is a helper function that takes a model, world matrix, and bone
         *        transforms. It does just what its name implies, and draws the model.
         *
         * @param model The model to draw.
         * @param worldTransform Where to draw the model.
         * @param absoluteBoneTransforms The model's bone transforms; this can be calculated using
         *        the function Model.CopyAbsoluteBoneTransformsTo.
         * @param drawBoundingSphere Whether to draw each mesh's bounding sphere as well.
         */
        void DrawModel(Model& model, const Matrix& worldTransform,
                       const std::vector<Matrix>& absoluteBoneTransforms, bool drawBoundingSphere);

        /**
         * @brief This helper function checks to see if a ray will intersect with a model.
         *
         * The model's bounding spheres are used, and the model is transformed using the matrix
         * specified in the worldTransform argument.
         *
         * @param ray The ray to perform the intersection check with.
         * @param model The model to perform the intersection check with; the model's bounding
         *        spheres will be used.
         * @param worldTransform A matrix that positions the model in world space.
         * @param absoluteBoneTransforms This array of matrices contains the absolute bone
         *        transforms for the model.
         * @return True if the ray intersects the model.
         */
        [[nodiscard]] static bool RayIntersectsModel(const Ray& ray, Model& model,
                                                     const Matrix& worldTransform,
                                                     const std::vector<Matrix>& absoluteBoneTransforms);

        /**
         * @brief This helper function takes a BoundingSphere and a transform matrix, and returns a
         *        transformed version of that BoundingSphere.
         *
         * @param sphere The BoundingSphere to transform.
         * @param transform How to transform the BoundingSphere.
         * @return The transformed BoundingSphere.
         */
        [[nodiscard]] static BoundingSphere TransformBoundingSphere(const BoundingSphere& sphere,
                                                                    const Matrix& transform);
    };
}
