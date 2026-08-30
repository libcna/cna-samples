// SPDX-License-Identifier: MS-PL

#include "HighScoreScreen.hpp"

#include <memory>
#include <optional>

#include "BackgroundScreen.hpp"
#include "MainMenuScreen.hpp"

namespace MarbleMazeGame
{
    void HighScoreScreen::HandleInput(InputState& input)
    {
        if (input.IsPauseGame(std::nullopt))
            Exit();

        if (!input.Gestures.empty() &&
            input.Gestures[0].getGestureTypeProperty() == GestureType::Tap)
        {
            Exit();
            input.Gestures.clear();
        }
    }

    void HighScoreScreen::Exit()
    {
        ExitScreen();
        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }
}
