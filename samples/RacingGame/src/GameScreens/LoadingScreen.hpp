// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Displays the original loading status before the splash screen. */
    class LoadingScreen final : public IGameScreen
    {
    public:
        /** @brief Creates a loading screen bound to the running game. */
        explicit LoadingScreen(RacingGameManager& game);
        /** @brief Advances loading-screen state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the completed loading status and exits this screen. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the Loading screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
    };
}
