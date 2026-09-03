// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Owns one complete active race within the screen stack. */
    class GameScreen final : public IGameScreen
    {
    public:
        /** @brief Loads the selected level and resets the race. */
        explicit GameScreen(RacingGameManager& game);
        /** @brief Creates a race screen and optionally reuses an already loaded level. */
        GameScreen(RacingGameManager& game, bool loadSelectedLevel);
        /** @brief Updates the active race. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the race and processes return-to-menu input. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the Game screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
    };
}
