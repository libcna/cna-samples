// SPDX-License-Identifier: MS-PL

#include "MainMenuScreen.hpp"

#include <memory>
#include <optional>

#include "BackgroundScreen.hpp"
#include "HighScoreScreen.hpp"
#include "LoadingAndInstructionScreen.hpp"

namespace MarbleMazeGame
{
    void MainMenuScreen::HighScoreMenuEntrySelected()
    {
        for (auto& screen : GetScreenManager()->GetScreens())
            screen->ExitScreen();

        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<HighScoreScreen>(), std::nullopt);
    }

    void MainMenuScreen::StartGameMenuEntrySelected()
    {
        for (auto& screen : GetScreenManager()->GetScreens())
            screen->ExitScreen();

        GetScreenManager()->AddScreen(std::make_shared<LoadingAndInstructionScreen>(), std::nullopt);
    }

    void MainMenuScreen::OnCancel(PlayerIndex playerIndex)
    {
        (void)playerIndex;
        HighScoreScreen::SaveHighscore();
        GetScreenManager()->getGameProperty().Exit();
    }
}
