// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"

#include "CubePrimitive.hpp"
#include "CylinderPrimitive.hpp"
#include "GeometricPrimitive.hpp"
#include "SpherePrimitive.hpp"
#include "TeapotPrimitive.hpp"
#include "TorusPrimitive.hpp"

namespace Primitives3D
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /** @brief Draws and interactively cycles XNA's sample geometric primitives. */
    class Primitives3DGame : public Microsoft::Xna::Framework::Game
    {
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> spriteFont;

        KeyboardState currentKeyboardState;
        KeyboardState lastKeyboardState;
        GamePadState currentGamePadState;
        GamePadState lastGamePadState;
        MouseState currentMouseState;
        MouseState lastMouseState;

        std::vector<std::unique_ptr<GeometricPrimitive>> primitives;
        int currentPrimitiveIndex = 0;
        RasterizerState wireFrameState;

        std::vector<Color> colors = {
            Color::Red, Color::Green, Color::Blue, Color::White, Color::Black,
        };
        int currentColorIndex = 0;
        bool isWireframe = false;

        [[nodiscard]] bool IsPressed(Keys key, Buttons button) const
        {
            return (currentKeyboardState.IsKeyDown(key) && lastKeyboardState.IsKeyUp(key)) ||
                   (currentGamePadState.IsButtonDown(button) &&
                    lastGamePadState.IsButtonUp(button));
        }

        [[nodiscard]] bool LeftMouseIsPressed(Rectangle rectangle) const
        {
            return currentMouseState.getLeftButtonProperty() == ButtonState::Pressed &&
                   lastMouseState.getLeftButtonProperty() != ButtonState::Pressed &&
                   rectangle.Contains(
                       currentMouseState.getXProperty(), currentMouseState.getYProperty());
        }

        void HandleInput()
        {
            lastKeyboardState = currentKeyboardState;
            lastGamePadState = currentGamePadState;
            lastMouseState = currentMouseState;

            currentKeyboardState = Keyboard::GetState();
            currentGamePadState = GamePad::GetState(PlayerIndex::One);
            currentMouseState = Mouse::GetState();

            if (IsPressed(Keys::Escape, Buttons::Back))
                Exit();

            const auto& viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
            const int halfWidth = viewport.getWidthProperty() / 2;
            const int halfHeight = viewport.getHeightProperty() / 2;

            const Rectangle topOfScreen(
                0, 0, viewport.getWidthProperty(), halfHeight);
            if (IsPressed(Keys::A, Buttons::A) || LeftMouseIsPressed(topOfScreen))
                currentPrimitiveIndex =
                    (currentPrimitiveIndex + 1) % static_cast<int>(primitives.size());

            const Rectangle botLeftOfScreen(0, halfHeight, halfWidth, halfHeight);
            if (IsPressed(Keys::B, Buttons::B) || LeftMouseIsPressed(botLeftOfScreen))
                currentColorIndex =
                    (currentColorIndex + 1) % static_cast<int>(colors.size());

            const Rectangle botRightOfScreen(
                halfWidth, halfHeight, halfWidth, halfHeight);
            if (IsPressed(Keys::Y, Buttons::Y) || LeftMouseIsPressed(botRightOfScreen))
                isWireframe = !isWireframe;
        }

    public:
        /** @brief Creates the sample game and configures its content root. */
        Primitives3DGame() : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
        }

        /** @brief Returns the fully-qualified sample type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "Primitives3D.Primitives3DGame";
            return name;
        }

        /** @brief Loads the font, procedural primitives, and wireframe state. */
        void LoadContent() override
        {
            auto& device = *graphics.getGraphicsDeviceProperty();
            spriteBatch = std::make_unique<SpriteBatch>(device);
            spriteFont.emplace(getContentProperty().Load<SpriteFont>("hudfont"));

            primitives.push_back(std::make_unique<CubePrimitive>(device));
            primitives.push_back(std::make_unique<SpherePrimitive>(device));
            primitives.push_back(std::make_unique<CylinderPrimitive>(device));
            primitives.push_back(std::make_unique<TorusPrimitive>(device));
            primitives.push_back(std::make_unique<TeapotPrimitive>(device));

            wireFrameState.setFillModeProperty(FillMode::WireFrame);
            wireFrameState.setCullModeProperty(CullMode::None);
        }

        /** @brief Handles one frame of sample input. */
        void Update(GameTime& gameTime) override
        {
            HandleInput();
            Game::Update(gameTime);
        }

        /** @brief Draws the rotating primitive and the original controls overlay. */
        void Draw(const GameTime& gameTime) override
        {
            auto& device = *graphics.getGraphicsDeviceProperty();
            device.Clear(Color::CornflowerBlue);

            if (isWireframe)
                device.setRasterizerStateProperty(wireFrameState);
            else
                device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

            const float time = static_cast<float>(
                gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());
            const float yaw = time * 0.4f;
            const float pitch = time * 0.7f;
            const float roll = time * 1.1f;
            const Vector3 cameraPosition(0.0f, 0.0f, 2.5f);
            const auto& viewport = device.getViewportProperty();

            const Matrix world = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
            const Matrix view = Matrix::CreateLookAt(
                cameraPosition, Vector3::Zero, Vector3::Up);
            const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
                1.0f, viewport.getAspectRatioProperty(), 1.0f, 10.0f);

            primitives[currentPrimitiveIndex]->Draw(
                world, view, projection, colors[currentColorIndex]);

            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

            const std::string text =
                "A or tap top of screen = Change primitive\n"
                "B or tap bottom left of screen = Change color\n"
                "Y or tap bottom right of screen = Toggle wireframe";
            spriteBatch->Begin();
            spriteBatch->DrawString(*spriteFont, text, Vector2(48.0f, 48.0f), Color::White);
            spriteBatch->End();

            Game::Draw(gameTime);
        }
    };
}
