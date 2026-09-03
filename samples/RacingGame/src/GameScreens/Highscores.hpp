// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Displays the original per-track ten-entry highscore table. */
    class Highscores final : public IGameScreen
    {
    public:
        /** @brief Creates a highscore screen bound to the running game. */
        explicit Highscores(RacingGameManager& game);
        /** @brief Advances highscore-screen state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws and processes the highscore table. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the Highscores screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
        int selectedLevel = 1;
    };
}
