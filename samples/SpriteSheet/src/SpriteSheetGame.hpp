// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <string>

#include "ContentReaders.hpp"
#include "SpriteSheet.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace SpriteSheetSampleWindowsPhone
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using SpriteSheetRuntime::SpriteSheet;

    /** @brief Demonstrates build-time sprite-sheet packing and runtime lookup. */
    class SpriteSheetGame : public Game
    {
        GraphicsDeviceManager graphics_;
        std::unique_ptr<SpriteBatch> spriteBatch_;
        std::optional<SpriteSheet> spriteSheet_;
        std::optional<SpriteFont> spriteFont_;
        Texture2D checker_;

    public:
        /** @brief Constructs the Windows XNA 4.0 sample configuration. */
        SpriteSheetGame()
            : graphics_(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
            SpriteSheetRuntime::ContentReaders::Register();
            graphics_.setPreferredBackBufferWidthProperty(853);
            graphics_.setPreferredBackBufferHeightProperty(480);
        }

        /** @brief Returns the sample game's runtime type name. */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name =
                "SpriteSheetSampleWindowsPhone.SpriteSheetGame";
            return name;
        }

    protected:
        void LoadContent() override
        {
            spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
            spriteSheet_.emplace(getContentProperty().Load<SpriteSheet>("SpriteSheet"));
            spriteFont_.emplace(getContentProperty().Load<SpriteFont>("hudFont"));
            checker_ = getContentProperty().Load<Texture2D>("Checker");
        }

        void Update(GameTime& gameTime) override
        {
            HandleInput();
            Game::Update(gameTime);
        }

        void Draw(const GameTime& gameTime) override
        {
            const float time = static_cast<float>(
                gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

            getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

            spriteBatch_->Begin();

            spriteBatch_->DrawString(
                *spriteFont_,
                "Here are some individual sprites,\n"
                "all stored in a single sprite sheet:",
                Vector2(100.0f, 80.0f),
                Color::White);

            spriteBatch_->Draw(
                spriteSheet_->getTextureProperty(),
                Vector2(200.0f, 250.0f),
                std::optional<Rectangle>(spriteSheet_->SourceRectangle("cat")),
                Color::White,
                time,
                Vector2(50.0f, 50.0f),
                1.0f,
                SpriteEffects::None,
                0.0f);

            constexpr int animationFramesPerSecond = 20;
            constexpr int animationFrameCount = 7;

            int glowIndex = spriteSheet_->GetIndex("glow1");
            glowIndex += static_cast<int>(time * animationFramesPerSecond) %
                         animationFrameCount;

            spriteBatch_->Draw(
                spriteSheet_->getTextureProperty(),
                Rectangle(100, 150, 200, 200),
                spriteSheet_->SourceRectangle(glowIndex),
                Color::White);

            spriteBatch_->End();

            DrawEntireSpriteSheetTexture();

            Game::Draw(gameTime);
        }

    private:
        void DrawEntireSpriteSheetTexture()
        {
            const Vector2 location(500.0f, 80.0f);

            spriteBatch_->Begin(
                SpriteSortMode::Deferred,
                nullptr,
                const_cast<SamplerState*>(&SamplerState::LinearWrap),
                nullptr,
                nullptr);

            const int width = spriteSheet_->getTextureProperty().getWidthProperty();
            const int height = spriteSheet_->getTextureProperty().getHeightProperty();

            const Rectangle rectangle(
                static_cast<int>(location.X),
                static_cast<int>(location.Y + 70.0f),
                width,
                height);

            spriteBatch_->Draw(
                checker_, rectangle, Rectangle(0, 0, width, height), Color::White);

            spriteBatch_->End();

            spriteBatch_->Begin();

            spriteBatch_->DrawString(
                *spriteFont_,
                "And here is the combined\n"
                "sprite sheet texture:",
                location,
                Color::White);

            spriteBatch_->Draw(
                spriteSheet_->getTextureProperty(), rectangle, Color::White);

            spriteBatch_->End();
        }

        void HandleInput()
        {
            if (GamePad::GetState(PlayerIndex::One).IsButtonDown(Buttons::Back) ||
                Keyboard::GetState().IsKeyDown(Keys::Escape))
            {
                Exit();
            }
        }
    };
}
