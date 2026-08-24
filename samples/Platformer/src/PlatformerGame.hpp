// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Accelerometer.hpp"
#include "Level.hpp"
#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "System/TimeSpan.hpp"

namespace Platformer
{
    class PlatformerGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        PlatformerGame();

        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        void LoadContent() override;
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        void HandleInput();
        void LoadNextLevel();
        void ReloadCurrentLevel();
        void DrawHud();
        void DrawShadowedString(const Microsoft::Xna::Framework::Graphics::SpriteFont& font,
                                const std::string& value,
                                Microsoft::Xna::Framework::Vector2 position,
                                Microsoft::Xna::Framework::Color color);

        Microsoft::Xna::Framework::GraphicsDeviceManager graphics_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> hudFont_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> winOverlay_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> loseOverlay_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> diedOverlay_;

        int levelIndex_ = -1;
        std::unique_ptr<Level> level_;
        bool wasContinuePressed_ = false;
        inline static const System::TimeSpan WarningTime = System::TimeSpan::FromSeconds(30.0);

        Microsoft::Xna::Framework::Input::GamePadState gamePadState_;
        Microsoft::Xna::Framework::Input::KeyboardState keyboardState_;
        Microsoft::Xna::Framework::Input::Touch::TouchCollection touchState_;
        AccelerometerState accelerometerState_;

        static constexpr int numberOfLevels = 3;
        std::optional<Microsoft::Xna::Framework::Media::Song> music_;
    };
}
