// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Implements the original three-car and hue selection screen. */
    class CarSelection final : public IGameScreen
    {
    public:
        /** @brief Creates a car selector bound to the running game. */
        explicit CarSelection(RacingGameManager& game);
        /** @brief Advances car-selection state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws and processes car and hue selection. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the CarSelection screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

        /** @brief Adjusts a target rotation to the nearest source revolution. */
        static void AdjustRotRange(float& desiredRotation, float sourceRotation);
        /** @brief Wraps a rotation once into the -Pi to Pi range. */
        static void AdjustRotToPIRange(float& rotation);
        /** @brief Moves a rotation toward its target by a bounded step. */
        [[nodiscard]] static float InterpolateRotation(
            float rotation, float targetRotation, float nearlyEqualRotation);

    private:
        RacingGameManager& game;
        float carSelectionRotationZ = 0.0f;
    };
}
