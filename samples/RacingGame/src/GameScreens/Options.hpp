// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

#include "GameScreens/IGameScreen.hpp"

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameScreens
{
    /** @brief Edits the original Racing desktop settings. */
    class Options final : public IGameScreen
    {
    public:
        /** @brief Creates an options screen from current persistent settings. */
        explicit Options(RacingGameManager& game);
        /** @brief Advances options-screen state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws and processes the original desktop options. */
        [[nodiscard]] bool Render() override;
        /** @brief Gets the Options screen identity. */
        [[nodiscard]] ScreenKind getKindProperty() const override;

    private:
        RacingGameManager& game;
        int currentOption = 0;
        int currentResolution = 4;
        std::string currentPlayerName;
        bool fullscreen = true;
        bool usePostScreenShaders = true;
        bool useShadowMapping = true;
        bool useHighDetail = true;
        float currentMusicVolume = 0.6f;
        float currentSoundVolume = 0.8f;
        float currentSensitivity = 0.5f;
    };
}
