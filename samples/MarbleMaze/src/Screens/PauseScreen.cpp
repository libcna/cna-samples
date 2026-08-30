// SPDX-License-Identifier: MS-PL

#include "PauseScreen.hpp"

#include <memory>
#include <optional>

#include "BackgroundScreen.hpp"
#include "GameplayScreen.hpp"
#include "MainMenuScreen.hpp"

namespace MarbleMazeGame
{
    void PauseScreen::ReturnGameMenuEntrySelected()
    {
        AudioManager::PauseResumeSounds(true);

        for (auto& screen : GetScreenManager()->GetScreens())
            if (dynamic_cast<GameplayScreen*>(screen.get()) == nullptr)
                screen->ExitScreen();

        auto gameplay = std::dynamic_pointer_cast<GameplayScreen>(GetScreenManager()->GetScreens()[0]);
        gameplay->IsActive = true;
    }

    void PauseScreen::RestartGameMenuEntrySelected()
    {
        AudioManager::PauseResumeSounds(true);

        for (auto& screen : GetScreenManager()->GetScreens())
            if (dynamic_cast<GameplayScreen*>(screen.get()) == nullptr)
                screen->ExitScreen();

        auto gameplay = std::dynamic_pointer_cast<GameplayScreen>(GetScreenManager()->GetScreens()[0]);
        gameplay->IsActive = true;
        gameplay->Restart();
    }

    void PauseScreen::OnCancel(PlayerIndex playerIndex)
    {
        (void)playerIndex;
        for (auto& screen : GetScreenManager()->GetScreens())
            screen->ExitScreen();

        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }
}
