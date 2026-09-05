// SPDX-License-Identifier: MS-PL

#include "LoadingAndInstructionScreen.hpp"

#include <memory>
#include <optional>

#include "GameplayScreen.hpp"

namespace MarbleMazeGame
{
    void LoadingAndInstructionScreen::LoadContent()
    {
        background_ = Load<Texture2D>("Textures/instructions");
        font_ = Load<SpriteFont>("Fonts/MenuFont");

        gameplayScreen_ = std::make_shared<GameplayScreen>();
        gameplayScreen_->setScreenManager(GetScreenManager());
    }

    void LoadingAndInstructionScreen::HandleInput(InputState& input)
    {
        if (!isLoading_ && !input.Gestures.empty() &&
            input.Gestures[0].getGestureTypeProperty() == GestureType::Tap)
        {
#if defined(__EMSCRIPTEN__)
            // WebGL resources must be created on the thread that owns the browser GL context.
            isLoading_ = true;
            gameplayScreen_->LoadAssets();
            assetsLoaded_ = true;
#else
            thread_ = std::make_unique<System::Threading::Thread>(
                [gameplay = gameplayScreen_] { gameplay->LoadAssets(); });
            isLoading_ = true;
            thread_->Start();
#endif
        }

        GameScreen::HandleInput(input);
    }

    void LoadingAndInstructionScreen::Update(
        GameTime& gameTime,
        bool otherScreenHasFocus,
        bool coveredByOtherScreen)
    {
#if defined(__EMSCRIPTEN__)
        const bool loadingFinished = assetsLoaded_;
#else
        const bool loadingFinished =
            thread_ != nullptr &&
            thread_->getThreadStateProperty() == System::Threading::ThreadState::Stopped;
#endif

        if (loadingFinished && !IsExiting())
        {
            for (auto& screen : GetScreenManager()->GetScreens())
                screen->ExitScreen();

            GetScreenManager()->AddScreen(gameplayScreen_, std::nullopt);
        }

        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }
}
