// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Displays the authentic desktop help panel. */
    class Help final : public IGameScreen
    {
    public:
        /** @brief Creates a help screen bound to the running game. */
        explicit Help(RacingGameManager& game);
        /** @brief Advances help-screen state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the help screen and processes Back. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the Help screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
    };
}
