// SPDX-License-Identifier: MS-PL

#pragma once

#include <cmath>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace SpriteEffectsSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    class SpriteEffectsGame : public Game
    {
        GraphicsDeviceManager graphics_;

        KeyboardState lastKeyboardState_;
        GamePadState lastGamePadState_;
        KeyboardState currentKeyboardState_;
        GamePadState currentGamePadState_;

        enum class DemoEffect
        {
            Desaturate,
            Disappear,
            RefractCat,
            RefractGlacier,
            Normalmap,
        };

        DemoEffect currentEffect_ = DemoEffect::Desaturate;

        std::shared_ptr<Effect> desaturateEffect_;
        std::shared_ptr<Effect> disappearEffect_;
        std::shared_ptr<Effect> normalmapEffect_;
        std::shared_ptr<Effect> refractionEffect_;

        Texture2D catTexture_;
        Texture2D catNormalmapTexture_;
        Texture2D glacierTexture_;
        Texture2D waterfallTexture_;

        std::unique_ptr<SpriteBatch> spriteBatch_;

    public:
        SpriteEffectsGame()
            : graphics_(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
        }

        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "SpriteEffects.SpriteEffectsGame";
            return name;
        }

    protected:
        void LoadContent() override
        {
            desaturateEffect_ = getContentProperty().Load<std::shared_ptr<Effect>>("desaturate");
            disappearEffect_ = getContentProperty().Load<std::shared_ptr<Effect>>("disappear");
            normalmapEffect_ = getContentProperty().Load<std::shared_ptr<Effect>>("normalmap");
            refractionEffect_ = getContentProperty().Load<std::shared_ptr<Effect>>("refraction");

            catTexture_ = getContentProperty().Load<Texture2D>("cat");
            catNormalmapTexture_ = getContentProperty().Load<Texture2D>("cat_normalmap");
            glacierTexture_ = getContentProperty().Load<Texture2D>("glacier");
            waterfallTexture_ = getContentProperty().Load<Texture2D>("waterfall");

            spriteBatch_ = std::make_unique<SpriteBatch>(*graphics_.getGraphicsDeviceProperty());
        }

        void Update(GameTime& gameTime) override
        {
            HandleInput();

            if (NextButtonPressed())
            {
                currentEffect_ = static_cast<DemoEffect>(static_cast<int>(currentEffect_) + 1);

                if (currentEffect_ > DemoEffect::Normalmap)
                    currentEffect_ = DemoEffect::Desaturate;
            }

            Game::Update(gameTime);
        }

        void Draw(const GameTime& gameTime) override
        {
            switch (currentEffect_)
            {
                case DemoEffect::Desaturate:
                    DrawDesaturate(gameTime);
                    break;

                case DemoEffect::Disappear:
                    DrawDisappear(gameTime);
                    break;

                case DemoEffect::Normalmap:
                    DrawNormalmap(gameTime);
                    break;

                case DemoEffect::RefractCat:
                    DrawRefractCat(gameTime);
                    break;

                case DemoEffect::RefractGlacier:
                    DrawRefractGlacier(gameTime);
                    break;
            }

            Game::Draw(gameTime);
        }

    private:
        void DrawDesaturate(const GameTime& gameTime)
        {
            spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend,
                                nullptr, nullptr, nullptr, desaturateEffect_.get());

            const bytecs pulsate = static_cast<bytecs>(Pulsate(gameTime, 4.0f, 0.0f, 255.0f));

            spriteBatch_->Draw(glacierTexture_, QuarterOfScreen(0, 0), Color(255, 255, 255, 0));
            spriteBatch_->Draw(glacierTexture_, QuarterOfScreen(1, 0), Color(255, 255, 255, 64));
            spriteBatch_->Draw(glacierTexture_, QuarterOfScreen(0, 1), Color(255, 255, 255, 255));
            spriteBatch_->Draw(glacierTexture_, QuarterOfScreen(1, 1),
                               Color(255, 255, 255, static_cast<intcs>(pulsate)));

            spriteBatch_->End();
        }

        void DrawDisappear(const GameTime& gameTime)
        {
            spriteBatch_->Begin();
            spriteBatch_->Draw(glacierTexture_,
                               graphics_.getGraphicsDeviceProperty()->getViewportProperty().getBoundsProperty(),
                               Color::White);
            spriteBatch_->End();

            disappearEffect_->getParametersProperty()["OverlayScroll"]->SetValue(
                MoveInCircle(gameTime, 0.8f) * 0.25f);

            graphics_.getGraphicsDeviceProperty()->getTexturesProperty()(1, &waterfallTexture_);

            spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend,
                                nullptr, nullptr, nullptr, disappearEffect_.get());

            const bytecs fade = static_cast<bytecs>(Pulsate(gameTime, 2.0f, 0.0f, 255.0f));

            spriteBatch_->Draw(catTexture_, MoveInCircle(gameTime, catTexture_, 1.0f),
                               Color(255, 255, 255, static_cast<intcs>(fade)));

            spriteBatch_->End();
        }

        void DrawRefractCat(const GameTime& gameTime)
        {
            spriteBatch_->Begin();
            spriteBatch_->Draw(glacierTexture_,
                               graphics_.getGraphicsDeviceProperty()->getViewportProperty().getBoundsProperty(),
                               Color::White);
            spriteBatch_->End();

            refractionEffect_->getParametersProperty()["DisplacementScroll"]->SetValue(
                MoveInCircle(gameTime, 0.1f));

            graphics_.getGraphicsDeviceProperty()->getTexturesProperty()(1, &waterfallTexture_);

            spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend,
                                nullptr, nullptr, nullptr, refractionEffect_.get());

            spriteBatch_->Draw(catTexture_, MoveInCircle(gameTime, catTexture_, 1.0f), Color::White);

            spriteBatch_->End();
        }

        void DrawRefractGlacier(const GameTime& gameTime)
        {
            refractionEffect_->getParametersProperty()["DisplacementScroll"]->SetValue(
                MoveInCircle(gameTime, 0.2f));

            graphics_.getGraphicsDeviceProperty()->getTexturesProperty()(1, &waterfallTexture_);

            spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend,
                                nullptr, nullptr, nullptr, refractionEffect_.get());

            const Rectangle croppedGlacier(32, 32,
                                           glacierTexture_.getWidthProperty() - 64,
                                           glacierTexture_.getHeightProperty() - 64);

            spriteBatch_->Draw(glacierTexture_,
                               graphics_.getGraphicsDeviceProperty()->getViewportProperty().getBoundsProperty(),
                               croppedGlacier, Color::White);

            spriteBatch_->End();
        }

        void DrawNormalmap(const GameTime& gameTime)
        {
            spriteBatch_->Begin();
            spriteBatch_->Draw(glacierTexture_,
                               graphics_.getGraphicsDeviceProperty()->getViewportProperty().getBoundsProperty(),
                               Color::White);
            spriteBatch_->End();

            const Vector2 spinningLight = MoveInCircle(gameTime, 1.5f);

            const double time = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();

            const float tiltUpAndDown = 0.5f + static_cast<float>(std::cos(time * 0.75)) * 0.1f;

            Vector3 lightDirection(spinningLight * tiltUpAndDown, 1.0f - tiltUpAndDown);
            lightDirection.Normalize();

            normalmapEffect_->getParametersProperty()["LightDirection"]->SetValue(lightDirection);

            graphics_.getGraphicsDeviceProperty()->getTexturesProperty()(1, &catNormalmapTexture_);

            spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend,
                                nullptr, nullptr, nullptr, normalmapEffect_.get());

            spriteBatch_->Draw(catTexture_, CenterOnScreen(catTexture_), Color::White);

            spriteBatch_->End();
        }

        Rectangle QuarterOfScreen(int x, int y)
        {
            const auto viewport = graphics_.getGraphicsDeviceProperty()->getViewportProperty();

            const int w = viewport.getWidthProperty() / 2;
            const int h = viewport.getHeightProperty() / 2;

            return Rectangle(w * x, h * y, w, h);
        }

        Vector2 CenterOnScreen(const Texture2D& texture)
        {
            const auto viewport = graphics_.getGraphicsDeviceProperty()->getViewportProperty();

            const int x = (viewport.getWidthProperty() - texture.getWidthProperty()) / 2;
            const int y = (viewport.getHeightProperty() - texture.getHeightProperty()) / 2;

            return Vector2(static_cast<float>(x), static_cast<float>(y));
        }

        static float Pulsate(const GameTime& gameTime, float speed, float min, float max)
        {
            const double time = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() * speed;

            return min + (static_cast<float>(std::sin(time)) + 1.0f) / 2.0f * (max - min);
        }

        static Vector2 MoveInCircle(const GameTime& gameTime, float speed)
        {
            const double time = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() * speed;

            const float x = static_cast<float>(std::cos(time));
            const float y = static_cast<float>(std::sin(time));

            return Vector2(x, y);
        }

        Vector2 MoveInCircle(const GameTime& gameTime, const Texture2D& texture, float speed)
        {
            const auto viewport = graphics_.getGraphicsDeviceProperty()->getViewportProperty();

            const float x = static_cast<float>(viewport.getWidthProperty() - texture.getWidthProperty()) / 2.0f;
            const float y = static_cast<float>(viewport.getHeightProperty() - texture.getHeightProperty()) / 2.0f;

            return MoveInCircle(gameTime, speed) * 128.0f + Vector2(x, y);
        }

        void HandleInput()
        {
            lastKeyboardState_ = currentKeyboardState_;
            lastGamePadState_ = currentGamePadState_;

            currentKeyboardState_ = Keyboard::GetState();
            currentGamePadState_ = GamePad::GetState(PlayerIndex::One);

            if (currentKeyboardState_.IsKeyDown(Keys::Escape) ||
                currentGamePadState_.IsButtonDown(Buttons::Back))
            {
                Exit();
            }
        }

        bool NextButtonPressed()
        {
            if (currentKeyboardState_.IsKeyDown(Keys::Space) &&
                !lastKeyboardState_.IsKeyDown(Keys::Space))
                return true;

            if (currentGamePadState_.IsButtonDown(Buttons::A) &&
                lastGamePadState_.IsButtonUp(Buttons::A))
                return true;

            return false;
        }
    };
}
