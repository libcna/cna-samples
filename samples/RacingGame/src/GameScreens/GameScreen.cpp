// SPDX-License-Identifier: MS-PL

#include "GameScreens/GameScreen.hpp"

#include "GameLogic/Input.hpp"
#include "RacingGameManager.hpp"

namespace RacingGame::GameScreens
{
    GameScreen::GameScreen(RacingGameManager& setGame)
        : GameScreen(setGame, true)
    {
    }

    GameScreen::GameScreen(
        RacingGameManager& setGame, const bool loadSelectedLevel)
        : game(setGame)
    {
        if (loadSelectedLevel) game.LoadSelectedLevel();
        game.StartGearSound();
        game.PlayGameMusic();
    }

    void GameScreen::Update(Microsoft::Xna::Framework::GameTime&) {}

    bool GameScreen::Render()
    {
        game.DrawRace();
        const auto& input = game.getControlsProperty();
        const bool exit = input.backJustPressed ||
            (game.getRaceGameOverProperty() &&
             (input.mouseLeftJustPressed || input.acceptJustPressed ||
              input.cancelJustPressed || input.xJustPressed));
        if (exit)
        {
            game.StopGearSoundNow();
            game.PlayMenuMusic();
        }
        return exit;
    }

    ScreenKind GameScreen::getKindProperty() const
    {
        return ScreenKind::Game;
    }
}
