// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "AlignedSpriteBatch.hpp"
#include "SafeAreaOverlay.hpp"

namespace SafeArea
{
    /** @brief Demonstrates title-safe placement and camera scrolling. */
    class SafeAreaGame final : public Microsoft::Xna::Framework::Game
    {
        static constexpr int ScreenWidth = 1280;
        static constexpr int ScreenHeight = 720;

        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;
        std::unique_ptr<SafeAreaOverlay> safeAreaOverlay;
        std::unique_ptr<AlignedSpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        Microsoft::Xna::Framework::Graphics::Texture2D catTexture;
        Microsoft::Xna::Framework::Graphics::Texture2D backgroundTexture;

        Microsoft::Xna::Framework::Vector2 catPosition;
        Microsoft::Xna::Framework::Vector2 catVelocity;
        Microsoft::Xna::Framework::Vector2 cameraPosition;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;
        Microsoft::Xna::Framework::Input::KeyboardState previousKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState previousGamePadState;

    public:
        /** @brief Creates the 1280x720 Safe Area sample. */
        SafeAreaGame()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
            graphics.setPreferredBackBufferWidthProperty(ScreenWidth);
            graphics.setPreferredBackBufferHeightProperty(ScreenHeight);

#if defined(XBOX) && !defined(NDEBUG)
            safeAreaOverlay = std::make_unique<SafeAreaOverlay>(*this);
            getComponentsProperty().Add(safeAreaOverlay.get());
#endif
        }

        /**
         * @brief Returns the fully qualified runtime type name.
         *
         * @return Fully qualified .NET-compatible type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "SafeArea.SafeAreaGame";
            return name;
        }

    protected:
        /** @brief Loads the original font, cat and background XNB assets. */
        void LoadContent() override
        {
            using namespace Microsoft::Xna::Framework::Graphics;

            spriteBatch = std::make_unique<AlignedSpriteBatch>(getGraphicsDeviceProperty());
            font.emplace(getContentProperty().Load<SpriteFont>("Font"));
            catTexture = getContentProperty().Load<Texture2D>("Cat");
            backgroundTexture = getContentProperty().Load<Texture2D>("Background");
        }

        /**
         * @brief Reads input and updates the cat and camera.
         *
         * @param gameTime Current timing snapshot.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override
        {
            HandleInput();
            UpdateCat();
            UpdateCamera();
            Game::Update(gameTime);
        }

        /**
         * @brief Draws the tiled background, cat and title-safe corner labels.
         *
         * @param gameTime Current timing snapshot.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override
        {
            using namespace Microsoft::Xna::Framework;

            getGraphicsDeviceProperty().Clear(Color::Black);

            const Vector2 screenCenter(
                static_cast<float>(ScreenWidth) / 2.0f,
                static_cast<float>(ScreenHeight) / 2.0f);
            const Vector2 scrollOffset = screenCenter - cameraPosition;

            spriteBatch->Begin();
            DrawBackground(scrollOffset);
            DrawCat(scrollOffset);
            DrawOverlays();
            spriteBatch->End();

            Game::Draw(gameTime);
        }

    private:
        void UpdateCat()
        {
            using namespace Microsoft::Xna::Framework;
            using namespace Microsoft::Xna::Framework::Input;

            constexpr float speedOfCat = 0.75f;
            constexpr float catFriction = 0.9f;

            const Vector2 flipY(1.0f, -1.0f);
            catVelocity = catVelocity +
                currentGamePadState.getThumbSticksProperty().getLeftProperty() *
                    flipY * speedOfCat;

            if (currentKeyboardState.IsKeyDown(Keys::Left))
            {
                catVelocity.X -= speedOfCat;
            }
            if (currentKeyboardState.IsKeyDown(Keys::Right))
            {
                catVelocity.X += speedOfCat;
            }
            if (currentKeyboardState.IsKeyDown(Keys::Up))
            {
                catVelocity.Y -= speedOfCat;
            }
            if (currentKeyboardState.IsKeyDown(Keys::Down))
            {
                catVelocity.Y += speedOfCat;
            }

            catPosition = catPosition + catVelocity;
            catVelocity = catVelocity * catFriction;
        }

        void UpdateCamera()
        {
            using namespace Microsoft::Xna::Framework;

            Vector2 maxScroll(
                static_cast<float>(ScreenWidth) / 2.0f,
                static_cast<float>(ScreenHeight) / 2.0f);
            constexpr float catSafeArea = 0.7f;
            maxScroll = maxScroll * catSafeArea;
            maxScroll = maxScroll - Vector2(
                static_cast<float>(catTexture.getWidthProperty()) / 2.0f,
                static_cast<float>(catTexture.getHeightProperty()) / 2.0f);

            const Vector2 min = catPosition - maxScroll;
            const Vector2 max = catPosition + maxScroll;

            cameraPosition.X = MathHelper::Clamp(cameraPosition.X, min.X, max.X);
            cameraPosition.Y = MathHelper::Clamp(cameraPosition.Y, min.Y, max.Y);
        }

        void DrawBackground(Microsoft::Xna::Framework::Vector2 scrollOffset)
        {
            using namespace Microsoft::Xna::Framework;

            int tileX = static_cast<int>(scrollOffset.X) % backgroundTexture.getWidthProperty();
            int tileY = static_cast<int>(scrollOffset.Y) % backgroundTexture.getHeightProperty();

            if (tileX > 0)
            {
                tileX -= backgroundTexture.getWidthProperty();
            }
            if (tileY > 0)
            {
                tileY -= backgroundTexture.getHeightProperty();
            }

            for (int x = tileX; x < ScreenWidth; x += backgroundTexture.getWidthProperty())
            {
                for (int y = tileY; y < ScreenHeight; y += backgroundTexture.getHeightProperty())
                {
                    spriteBatch->Draw(
                        backgroundTexture,
                        Vector2(static_cast<float>(x), static_cast<float>(y)),
                        Color::White);
                }
            }
        }

        void DrawCat(Microsoft::Xna::Framework::Vector2 scrollOffset)
        {
            using namespace Microsoft::Xna::Framework;

            const Vector2 catCenter(
                static_cast<float>(catTexture.getWidthProperty()) / 2.0f,
                static_cast<float>(catTexture.getHeightProperty()) / 2.0f);
            const Vector2 position = catPosition - catCenter + scrollOffset;

            spriteBatch->Draw(catTexture, position, Color::White);
        }

        void DrawOverlays()
        {
            using namespace Microsoft::Xna::Framework;
            using namespace Microsoft::Xna::Framework::Graphics;

            const Rectangle safeArea =
                getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();

            spriteBatch->DrawString(
                *font,
                "Top Left",
                Vector2(
                    static_cast<float>(safeArea.getLeftProperty()),
                    static_cast<float>(safeArea.getTopProperty())),
                Color::White,
                Alignment::TopLeft);
            spriteBatch->DrawString(
                *font,
                "Top Right",
                Vector2(
                    static_cast<float>(safeArea.getRightProperty()),
                    static_cast<float>(safeArea.getTopProperty())),
                Color::White,
                Alignment::TopRight);
            spriteBatch->DrawString(
                *font,
                "Bottom Left",
                Vector2(
                    static_cast<float>(safeArea.getLeftProperty()),
                    static_cast<float>(safeArea.getBottomProperty())),
                Color::White,
                Alignment::BottomLeft);
            spriteBatch->DrawString(
                *font,
                "Bottom Right",
                Vector2(
                    static_cast<float>(safeArea.getRightProperty()),
                    static_cast<float>(safeArea.getBottomProperty())),
                Color::White,
                Alignment::BottomRight);

            if (safeAreaOverlay != nullptr)
            {
                spriteBatch->DrawString(
                    *font,
                    "Press A to toggle the safe area overlay",
                    Vector2(
                        static_cast<float>(safeArea.getCenterProperty().X),
                        static_cast<float>(safeArea.getTopProperty())),
                    Color::White,
                    Alignment::TopCenter);
            }
        }

        void HandleInput()
        {
            using namespace Microsoft::Xna::Framework::Input;

            previousKeyboardState = currentKeyboardState;
            previousGamePadState = currentGamePadState;

            currentKeyboardState = Keyboard::GetState();
            currentGamePadState =
                GamePad::GetState(Microsoft::Xna::Framework::PlayerIndex::One);

            if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
                currentGamePadState.IsButtonDown(Buttons::Back))
            {
                Exit();
            }

            if (safeAreaOverlay != nullptr)
            {
                if ((currentKeyboardState.IsKeyDown(Keys::A) &&
                     previousKeyboardState.IsKeyUp(Keys::A)) ||
                    (currentGamePadState.IsButtonDown(Buttons::A) &&
                     previousGamePadState.IsButtonUp(Buttons::A)))
                {
                    safeAreaOverlay->setVisibleProperty(
                        !safeAreaOverlay->getVisibleProperty());
                }
            }
        }
    };
}
