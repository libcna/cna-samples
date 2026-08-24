// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>

#include "Camera.hpp"
#include "Enums.hpp"
#include "Player.hpp"
#include "Settings.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "System/PlatformID.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class RenderTarget2D;
    class SpriteBatch;
}

namespace Spacewar
{
    class Screen;

    class SpacewarGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        static constexpr int FixedDrawingWidth = 1280;
        static constexpr int FixedDrawingHeight = 720;

        SpacewarGame();
        ~SpacewarGame() override;
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        [[nodiscard]] static GameState getGameStateProperty();
        [[nodiscard]] static int getGameLevelProperty();
        static void setGameLevelProperty(int value);
        [[nodiscard]] static Camera& getCameraProperty();
        [[nodiscard]] static Settings& getSettingsProperty();
        [[nodiscard]] static std::array<Player, 2>& getPlayersProperty();
        [[nodiscard]] static Microsoft::Xna::Framework::Content::ContentManager& getContentManagerProperty();
        [[nodiscard]] static System::PlatformID getCurrentPlatformProperty();
        [[nodiscard]] static const Microsoft::Xna::Framework::Input::KeyboardState& getKeyStateProperty();

    protected:
        void Initialize() override;
        void BeginRun() override;
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        bool BeginDraw() override;
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        void EndDraw() override;
        void LoadContent() override;
        void UnloadContent() override;

    private:
        void ChangeState(GameState nextState);
        void ToggleFullScreen();
        void BeginDrawScaling();
        void EndDrawScaling();

        int preferredWindowWidth_ = 1280;
        int preferredWindowHeight_ = 720;
        bool paused_ = false;
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics_;
        bool enableDrawScaling_ = true;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::RenderTarget2D> drawBuffer_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        bool justWentFullScreen_ = false;

        static Microsoft::Xna::Framework::Content::ContentManager* contentManager_;
        static Settings settings_;
        static std::unique_ptr<Camera> camera_;
        static std::array<Player, 2> players_;
        static GameState gameState_;
        static int gameLevel_;
        static std::unique_ptr<Screen> currentScreen_;
        static System::PlatformID currentPlatform_;
        static Microsoft::Xna::Framework::Input::KeyboardState keyState_;
    };
}
