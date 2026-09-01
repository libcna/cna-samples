// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game1.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Sprite.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"

namespace TouchGestureSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;
    using Microsoft::Xna::Framework::Input::Touch::GestureType;
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    /** @brief Demonstrates XNA touch selection and gesture handling. */
    class Game1 : public Game
    {
    private:
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> font;
        Texture2D cat;

        static constexpr const char* helpText =
            "Hold (in empty space) - Create sprite\n"
            "Hold (on sprite) - Remove sprite\n"
            "Tap - Change sprite color\n"
            "Drag - Move sprite\n"
            "Flick - Throws sprite\n"
            "Pinch - Scale sprite";

        std::vector<std::unique_ptr<Sprite>> sprites;
        Sprite* selectedSprite = nullptr;

        void BringToFront(Sprite* sprite)
        {
            const auto found = std::find_if(
                sprites.begin(), sprites.end(),
                [sprite](const std::unique_ptr<Sprite>& candidate)
                {
                    return candidate.get() == sprite;
                });
            if (found == sprites.end())
                return;

            std::unique_ptr<Sprite> owned = std::move(*found);
            sprites.erase(found);
            sprites.push_back(std::move(owned));
        }

        void Remove(Sprite* sprite)
        {
            const auto found = std::find_if(
                sprites.begin(), sprites.end(),
                [sprite](const std::unique_ptr<Sprite>& candidate)
                {
                    return candidate.get() == sprite;
                });
            if (found != sprites.end())
                sprites.erase(found);
        }

        void HandleTouchInput()
        {
            TouchCollection touches = TouchPanel::GetState();

            if (touches.getCountProperty() > 0 &&
                touches[0].getStateProperty() == TouchLocationState::Pressed)
            {
                const Vector2& position = touches[0].getPositionProperty();
                const Point touchPoint(
                    static_cast<int>(position.X), static_cast<int>(position.Y));

                selectedSprite = nullptr;
                for (auto it = sprites.rbegin(); it != sprites.rend(); ++it)
                {
                    Sprite* sprite = it->get();
                    if (sprite->getHitBoundsProperty().Contains(touchPoint))
                    {
                        selectedSprite = sprite;
                        break;
                    }
                }

                if (selectedSprite != nullptr)
                {
                    selectedSprite->Velocity = Vector2::Zero;
                    BringToFront(selectedSprite);
                }
            }

            while (TouchPanel::getIsGestureAvailableProperty())
            {
                const GestureSample gesture = TouchPanel::ReadGesture();
                switch (gesture.getGestureTypeProperty())
                {
                    case GestureType::Tap:
                    case GestureType::DoubleTap:
                        if (selectedSprite != nullptr)
                            selectedSprite->ChangeColor();
                        break;

                    case GestureType::Hold:
                        if (selectedSprite == nullptr)
                        {
                            auto sprite = std::make_unique<Sprite>(cat);
                            sprite->Center = gesture.getPositionProperty();
                            selectedSprite = sprite.get();
                            sprites.push_back(std::move(sprite));
                        }
                        else
                        {
                            Remove(selectedSprite);
                            selectedSprite = nullptr;
                        }
                        break;

                    case GestureType::FreeDrag:
                        if (selectedSprite != nullptr)
                            selectedSprite->Center += gesture.getDeltaProperty();
                        break;

                    case GestureType::Flick:
                        if (selectedSprite != nullptr)
                            selectedSprite->Velocity = gesture.getDeltaProperty();
                        break;

                    case GestureType::Pinch:
                        if (selectedSprite != nullptr)
                        {
                            const Vector2 a = gesture.getPositionProperty();
                            const Vector2 aOld = a - gesture.getDeltaProperty();
                            const Vector2 b = gesture.getPosition2Property();
                            const Vector2 bOld = b - gesture.getDelta2Property();

                            const float d = Vector2::Distance(a, b);
                            const float dOld = Vector2::Distance(aOld, bOld);
                            const float scaleChange = (d - dOld) * 0.01f;
                            selectedSprite->setScaleProperty(
                                selectedSprite->getScaleProperty() + scaleChange);
                        }
                        break;

                    default:
                        break;
                }
            }

            if (touches.getCountProperty() == 0)
                selectedSprite = nullptr;
        }

    public:
        /** @brief Constructs the touch-gesture sample. */
        Game1()
            : graphics(this)
        {
            graphics.setIsFullScreenProperty(true);
            getContentProperty().setRootDirectoryProperty("Content");

            // Frame rate is 30 fps by default for Windows Phone.
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        }

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `TouchGestureSample.Game1`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "TouchGestureSample.Game1";
            return name;
        }

    protected:
        /** @brief Enables the gestures used by the sample. */
        void Initialize() override
        {
            TouchPanel::setEnabledGesturesProperty(
                GestureType::Hold |
                GestureType::Tap |
                GestureType::DoubleTap |
                GestureType::FreeDrag |
                GestureType::Flick |
                GestureType::Pinch);

            Game::Initialize();
        }

        /** @brief Loads the original cat texture and instruction font. */
        void LoadContent() override
        {
            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
            cat = getContentProperty().Load<Texture2D>("cat");
            font = getContentProperty().Load<SpriteFont>("Font");
        }

        /**
         * @brief Handles input and advances every sprite.
         * @param gameTime Current game timing snapshot.
         */
        void Update(GameTime& gameTime) override
        {
            if (GamePad::GetState(PlayerIndex::One)
                    .getButtonsProperty().getBackProperty() == ButtonState::Pressed)
            {
                Exit();
            }

            HandleTouchInput();

            const auto bounds =
                getGraphicsDeviceProperty().getViewportProperty().getBoundsProperty();
            for (const auto& sprite : sprites)
                sprite->Update(gameTime, bounds);

            Game::Update(gameTime);
        }

        /**
         * @brief Draws all sprites and the original help text.
         * @param gameTime Current game timing snapshot.
         */
        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

            spriteBatch->Begin();
            for (const auto& sprite : sprites)
                sprite->Draw(*spriteBatch);
            spriteBatch->DrawString(*font, helpText, Vector2(10.0f, 32.0f), Color::White);
            spriteBatch->End();

            Game::Draw(gameTime);
        }
    };
}
