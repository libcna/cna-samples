// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Displays the original Racing press-start splash screen. */
    class SplashScreen final : public IGameScreen
    {
    public:
        /** @brief Creates a splash screen bound to the running game. */
        explicit SplashScreen(RacingGameManager& game);
        /** @brief Advances splash-screen state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the splash and exits it when start input is received. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the Splash screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
    };
}
