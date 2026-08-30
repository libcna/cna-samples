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
            thread_ = std::make_unique<System::Threading::Thread>(
                [gameplay = gameplayScreen_] { gameplay->LoadAssets(); });
            isLoading_ = true;
            thread_->Start();
        }

        GameScreen::HandleInput(input);
    }

    void LoadingAndInstructionScreen::Update(
        GameTime& gameTime,
        bool otherScreenHasFocus,
        bool coveredByOtherScreen)
    {
        if (thread_ != nullptr &&
            thread_->getThreadStateProperty() == System::Threading::ThreadState::Stopped &&
            !IsExiting())
        {
            for (auto& screen : GetScreenManager()->GetScreens())
                screen->ExitScreen();

            GetScreenManager()->AddScreen(gameplayScreen_, std::nullopt);
        }

        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }
}
