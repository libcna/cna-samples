// SPDX-License-Identifier: MS-PL

#pragma once

#include <cmath>
#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "DebugShapeRenderer.hpp"

namespace ShapeRenderingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /** @brief Main game type for the shape-rendering sample. */
    class ShapeRenderingSampleGame : public Game
    {
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;

        BoundingBox box;
        BoundingFrustum frustum;
        BoundingSphere sphere;

    public:
        /** @brief Creates the shape-rendering sample game. */
        ShapeRenderingSampleGame()
            : graphics(this)
            , frustum(Matrix::getIdentityProperty())
        {
            getContentProperty().setRootDirectoryProperty("Content");
        }

        /**
         * @brief Returns the fully qualified runtime type name.
         *
         * @return Fully qualified .NET-compatible type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name =
                "ShapeRenderingSample.ShapeRenderingSampleGame";
            return name;
        }

    protected:
        /** @brief Creates the rendering resources and debug shapes. */
        void LoadContent() override
        {
            auto& device = *graphics.getGraphicsDeviceProperty();

            spriteBatch = std::make_unique<SpriteBatch>(device);

            box = BoundingBox(Vector3(-3.0f), Vector3(3.0f));

            Matrix frustumView = Matrix::CreateLookAt(
                Vector3::Zero, Vector3::UnitX, Vector3::Up);
            Matrix frustumProjection = Matrix::CreatePerspectiveFieldOfView(
                MathHelper::PiOver4, 16.0f / 9.0f, 1.0f, 5.0f);
            frustum = BoundingFrustum(frustumView * frustumProjection);

            sphere = BoundingSphere(Vector3::Zero, 3.0f);

#if defined(SHAPE_RENDERING_SAMPLE_DEBUG)
            // C# Conditional("DEBUG") omits this call from non-debug callers.
            DebugShapeRenderer::Initialize(device);
#endif
        }

        /**
         * @brief Updates input and base game state.
         *
         * @param gameTime Current timing snapshot.
         */
        void Update(GameTime& gameTime) override
        {
            if (GamePad::GetState(PlayerIndex::One)
                    .getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
                Keyboard::GetState(PlayerIndex::One).IsKeyDown(Keys::Escape))
                Exit();

            Game::Update(gameTime);
        }

        /**
         * @brief Draws all sample shapes.
         *
         * @param gameTime Current timing snapshot.
         */
        void Draw(const GameTime& gameTime) override
        {
            auto& device = *graphics.getGraphicsDeviceProperty();
            device.Clear(Color::CornflowerBlue);

            float angle = static_cast<float>(
                gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());
            Vector3 eye(
                static_cast<float>(std::cos(static_cast<double>(angle * 0.5f))),
                0.0f,
                static_cast<float>(std::sin(static_cast<double>(angle * 0.5f))));
            eye = eye * 12.0f;
            eye.Y = 5.0f;

            Matrix view = Matrix::CreateLookAt(eye, Vector3::Zero, Vector3::Up);
            Matrix projection = Matrix::CreatePerspectiveFieldOfView(
                MathHelper::PiOver4,
                device.getViewportProperty().getAspectRatioProperty(),
                0.1f,
                1000.0f);

#if defined(SHAPE_RENDERING_SAMPLE_DEBUG)
            // These C# Conditional("DEBUG") calls are omitted in Release builds.
            DebugShapeRenderer::AddBoundingBox(box, Color::Yellow);
            DebugShapeRenderer::AddBoundingFrustum(frustum, Color::Green);
            DebugShapeRenderer::AddBoundingSphere(sphere, Color::Red);

            DebugShapeRenderer::AddTriangle(
                Vector3(-1.0f, 0.0f, 0.0f),
                Vector3(1.0f, 0.0f, 0.0f),
                Vector3(0.0f, 2.0f, 0.0f),
                Color::Purple);
            DebugShapeRenderer::AddLine(
                Vector3(0.0f, 0.0f, 0.0f),
                Vector3(3.0f, 3.0f, 3.0f),
                Color::Brown);

            DebugShapeRenderer::Draw(gameTime, view, projection);
#endif

            Game::Draw(gameTime);
        }
    };

} // namespace ShapeRenderingSample
