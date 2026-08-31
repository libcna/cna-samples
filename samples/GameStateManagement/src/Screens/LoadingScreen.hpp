// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "ScreenManager/GameScreen.hpp"

namespace GameStateManagement
{
    /** @brief Coordinates a fully transitioned loading boundary between screen groups. */
    class LoadingScreen final : public GameScreen
    {
    public:
        /** @brief Activates a loading screen. @param screenManager Manager. @param loadingIsSlow Whether to draw Loading. @param controllingPlayer Player. @param screensToLoad Screens to activate. */
        static void Load(ScreenManager& screenManager, bool loadingIsSlow,
                         std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer,
                         std::vector<std::shared_ptr<GameScreen>> screensToLoad);
        /** @brief Completes loading after previous screens disappear. @param gameTime Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen Cover state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime,
                    bool otherScreenHasFocus, bool coveredByOtherScreen) override;
        /** @brief Draws the loading message when required. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        LoadingScreen(ScreenManager& screenManager, bool loadingIsSlow,
                      std::vector<std::shared_ptr<GameScreen>> screensToLoad);

        bool loadingIsSlow_;
        bool otherScreensAreGone_ = false;
        std::vector<std::shared_ptr<GameScreen>> screensToLoad_;
    };
}
