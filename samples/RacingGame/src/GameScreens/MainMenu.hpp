// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>

#include "GameScreens/IGameScreen.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Implements the original five-button Racing main menu. */
    class MainMenu final : public IGameScreen
    {
    public:
        /** @brief Creates a main menu bound to the running game. */
        explicit MainMenu(RacingGameManager& game);
        /** @brief Starts menu-owned state such as music. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws and processes the original five-button menu. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the MainMenu screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;
        /** @brief Interpolates rectangles using the original C# rounding. */
        [[nodiscard]] static Microsoft::Xna::Framework::Rectangle
        InterpolateRect(
            Microsoft::Xna::Framework::Rectangle first,
            Microsoft::Xna::Framework::Rectangle second,
            float interpolation);

    private:
        RacingGameManager& game;
        int selectedButton = 0;
        std::array<float, 5> currentButtonSizes{1, 0, 0, 0, 0};
        bool ignoreMouse = true;
        float idleTime = 0.0f;
        float pressedLeftMilliseconds = 0.0f;
        float pressedRightMilliseconds = 0.0f;
        bool musicHasStarted = false;

        void SetSelectedButton(int value);
    };
}
