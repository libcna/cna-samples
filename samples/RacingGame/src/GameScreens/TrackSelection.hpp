// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Implements the original three-track selection screen. */
    class TrackSelection final : public IGameScreen
    {
    public:
        /** @brief Creates a track selector bound to the running game. */
        explicit TrackSelection(RacingGameManager& game);
        /** @brief Advances track-selection state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws and processes the original track selector. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the TrackSelection screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
        std::array<float, 3> currentButtonSizes{1, 0, 0};
        bool ignoreMouse = true;
    };
}
