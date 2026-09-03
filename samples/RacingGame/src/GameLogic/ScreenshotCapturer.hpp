// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

#include "Microsoft/Xna/Framework/GameComponent.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameLogic
{
    /** @brief Captures the current backbuffer when PrintScreen is pressed. */
    class ScreenshotCapturer final
        : public Microsoft::Xna::Framework::GameComponent
    {
    public:
        /**
         * @brief Creates the original desktop screenshot component.
         * @param game Racing game that owns the component and graphics device.
         */
        explicit ScreenshotCapturer(RacingGameManager& game);

        /**
         * @brief Captures a screenshot on a new PrintScreen press.
         * @param gameTime Snapshot of the current game timing state.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /** @brief Gets the most recently allocated screenshot number. */
        [[nodiscard]] int getScreenshotNumberProperty() const;

        /** @brief Returns the original fully qualified component type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        RacingGameManager* game;
        int screenshotNum = 0;

        [[nodiscard]] std::string ScreenshotNameBuilder(int number) const;
        [[nodiscard]] int GetCurrentScreenshotNum() const;
        void MakeScreenshot();
    };
}
