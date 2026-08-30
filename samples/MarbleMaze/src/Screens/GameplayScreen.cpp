// SPDX-License-Identifier: MS-PL

#include "GameplayScreen.hpp"

#include <any>
#include <memory>
#include <optional>

#include "BackgroundScreen.hpp"
#include "CalibrationScreen.hpp"
#include "HighScoreScreen.hpp"
#include "PauseScreen.hpp"

namespace MarbleMazeGame
{
    using GameStateManagement::ScreenManager;

    void GameplayScreen::FinishCurrentGame()
    {
        IsActive = false;

        for (auto& screen : GetScreenManager()->GetScreens())
            screen->ExitScreen();

        if (HighScoreScreen::IsInHighscores(elapsedGameTime_))
        {
            ScreenManager* manager = GetScreenManager();
            const TimeSpan scoreTime = elapsedGameTime_;
            [[maybe_unused]] System::IAsyncResult* keyboardInput = Guide::BeginShowKeyboardInput(
                PlayerIndex::One,
                "Player Name",
                "Enter your name (max 15 characters)",
                "Player",
                [manager, scoreTime](System::IAsyncResult& result)
                {
                    std::string playerName = Guide::EndShowKeyboardInput(&result);
                    if (playerName.size() > 15)
                        playerName = playerName.substr(0, 15);

                    HighScoreScreen::PutHighScore(playerName, scoreTime);
                    manager->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
                    manager->AddScreen(std::make_shared<HighScoreScreen>(), std::nullopt);
                    delete &result;
                },
                std::any{});
            return;
        }

        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<HighScoreScreen>(), std::nullopt);
    }

    void GameplayScreen::PauseCurrentGame()
    {
        IsActive = false;
        AudioManager::PauseResumeSounds(false);

        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<PauseScreen>(), std::nullopt);
    }

    void GameplayScreen::CalibrateGame()
    {
        IsActive = false;
        AudioManager::PauseResumeSounds(false);

        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<CalibrationScreen>(*this), std::nullopt);
    }

    bool GameplayScreen::HighScoreIsInHighscores() const
    {
        return HighScoreScreen::IsInHighscores(elapsedGameTime_);
    }
}
