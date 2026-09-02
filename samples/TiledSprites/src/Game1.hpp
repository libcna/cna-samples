// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsProfile.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"

namespace TiledSprites
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /** @brief Demonstrates tiling one texture in both axes with a wrapped SpriteBatch sampler. */
    class Game1 : public Game
    {
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;

    public:
        /** @brief Constructs the tiled-sprite sample and selects its Content root. */
        Game1()
            : graphics(this)
        {
            // XNA 4 Reach rejects wrapping this sample's 100x50 NPOT texture.
            graphics.setGraphicsProfileProperty(GraphicsProfile::HiDef);
            getContentProperty().setRootDirectoryProperty("Content");
        }

        /**
         * @brief Returns the sample game's runtime type name.
         *
         * @return The fully qualified original C# type name.
         */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "TiledSprites.Game1";
            return name;
        }

    protected:
        /** @brief Resets the sample's 30 Hz input gate, then initializes the base game. */
        void Initialize() override
        {
            lastInput = 0.0;
            Game::Initialize();
        }

    private:
        std::optional<Texture2D> xna;

    protected:
        /** @brief Creates SpriteBatch and loads the exact pipeline-built XNA logo texture. */
        void LoadContent() override
        {
            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
            xna.emplace(getContentProperty().Load<Texture2D>("XNA"));
        }

    private:
        double lastInput = 0.0;
        // C# performs integer division before assigning this constant to Double.
        double inputdelay = 1000 / 30;

    protected:
        /**
         * @brief Applies the original gamepad-only tile-count controls at a 30 Hz input cadence.
         *
         * @param gameTime Current frame timing.
         */
        void Update(GameTime& gameTime) override
        {
            if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
            {
                Exit();
            }

            if (lastInput + inputdelay
                <= gameTime.getTotalGameTimeProperty().getTotalMillisecondsProperty())
            {
                if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBProperty()
                    == ButtonState::Pressed)
                {
                    TilesX += 1;
                }
                if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getXProperty()
                    == ButtonState::Pressed)
                {
                    TilesX -= 1;
                }
                if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getYProperty()
                    == ButtonState::Pressed)
                {
                    TilesY -= 1;
                }
                if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getAProperty()
                    == ButtonState::Pressed)
                {
                    TilesY += 1;
                }
                lastInput = gameTime.getTotalGameTimeProperty().getTotalMillisecondsProperty();
            }

            Game::Update(gameTime);
        }

    private:
        int TilesX = 2;
        int TilesY = 2;

    protected:
        /**
         * @brief Draws the logo tiled TilesX by TilesY times at half scale.
         *
         * @param gameTime Current frame timing.
         */
        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

            const Rectangle source(
                0,
                0,
                xna->getWidthProperty() * TilesX,
                xna->getHeightProperty() * TilesY);

            spriteBatch->Begin(
                SpriteSortMode::Immediate,
                BlendState::AlphaBlend,
                &SamplerState::LinearWrap,
                &DepthStencilState::None,
                &RasterizerState::CullCounterClockwise);

            const Vector2 pos(50.0f);
            spriteBatch->Draw(
                *xna,
                pos,
                source,
                Color::White,
                0.0f,
                Vector2::Zero,
                0.5f,
                SpriteEffects::None,
                1.0f);
            spriteBatch->End();

            Game::Draw(gameTime);
        }
    };
}
