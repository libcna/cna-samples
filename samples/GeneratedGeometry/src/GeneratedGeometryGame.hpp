// SPDX-License-Identifier: MS-PL

#pragma once

#include <cmath>
#include <optional>
#include <string>

#include "ContentReaders.hpp"
#include "Sky.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/TimeSpan.hpp"

namespace GeneratedGeometry
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /** @brief Demonstrates geometry generated during the XNA content-pipeline build. */
    class GeneratedGeometryGame : public Game
    {
        GraphicsDeviceManager graphics_;
        std::optional<Model> terrain_;
        std::optional<Sky> sky_;

    public:
        /** @brief Constructs the XNA 4.0 sample configuration. */
        GeneratedGeometryGame()
            : graphics_(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
            graphics_.setIsFullScreenProperty(true);
#endif

            ContentReaders::Register();
        }

        /** @brief Returns the sample game's runtime type name. */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name =
                "GeneratedGeometry.GeneratedGeometryGame";
            return name;
        }

    protected:
        void LoadContent() override
        {
            terrain_.emplace(getContentProperty().Load<Model>("terrain"));
            sky_.emplace(getContentProperty().Load<Sky>("sky"));
        }

        void Update(GameTime& gameTime) override
        {
            HandleInput();
            Game::Update(gameTime);
        }

        void Draw(const GameTime& gameTime) override
        {
            GraphicsDevice& device = *graphics_.getGraphicsDeviceProperty();
            device.Clear(Color::Black);

            Matrix projection = Matrix::CreatePerspectiveFieldOfView(
                MathHelper::PiOver4,
                device.getViewportProperty().getAspectRatioProperty(),
                1.0f,
                10000.0f);

            const float time = static_cast<float>(
                gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()) * 0.333f;

            const float cameraX = std::cos(time);
            const float cameraY = std::sin(time);

            const Vector3 cameraPosition = Vector3(cameraX, 0.0f, cameraY) * 64.0f;
            const Vector3 cameraFront(-cameraY, 0.0f, cameraX);

            const Matrix view = Matrix::CreateLookAt(
                cameraPosition,
                cameraPosition + cameraFront,
                Vector3::Up);

            DrawTerrain(view, projection);
            sky_->Draw(view, projection);

            Game::Draw(gameTime);
        }

    private:
        void DrawTerrain(const Matrix& view, const Matrix& projection)
        {
            for (ModelMesh* mesh : terrain_->getMeshesProperty())
            {
                for (Effect* effect : mesh->getEffectsProperty())
                {
                    auto* basicEffect = static_cast<BasicEffect*>(effect);
                    basicEffect->setViewProperty(view);
                    basicEffect->setProjectionProperty(projection);
                    basicEffect->EnableDefaultLighting();
                    basicEffect->setSpecularColorProperty(Vector3(0.6f, 0.4f, 0.2f));
                    basicEffect->setSpecularPowerProperty(8.0f);
                    basicEffect->setFogEnabledProperty(true);
                    basicEffect->setFogColorProperty(Vector3(0.15f));
                    basicEffect->setFogStartProperty(100.0f);
                    basicEffect->setFogEndProperty(320.0f);
                }

                mesh->Draw();
            }
        }

        void HandleInput()
        {
            const KeyboardState currentKeyboardState = Keyboard::GetState();
            const GamePadState currentGamePadState = GamePad::GetState(PlayerIndex::One);

            if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
                currentGamePadState.getButtonsProperty().getBackProperty() ==
                    ButtonState::Pressed)
            {
                Exit();
            }
        }
    };
}
