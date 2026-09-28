// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LoadingScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/Threading/EventWaitHandle.hpp"
#include "System/TimeSpan.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
}

namespace Microsoft::Xna::Framework::Net
{
    class NetworkSession;
}

namespace NetworkStateManagement
{
    class IMessageDisplay;

    /**
     * @brief The loading screen coordinates transitions between the menu system and the game
     * itself. While a slow load is taking place, a background thread updates the network session
     * and draws the loading animation.
     */
    class LoadingScreen final : public GameScreen
    {
    public:
        /**
         * @brief Activates the loading screen.
         *
         * @param screenManager The screen manager.
         * @param loadingIsSlow Whether to show the loading animation (and run the worker).
         * @param controllingPlayer The player that controls the new screens, if any.
         * @param screensToLoad The screens to add once the others are gone; null entries are skipped.
         */
        static void Load(ScreenManager& screenManager, bool loadingIsSlow,
                         std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer,
                         std::vector<std::shared_ptr<GameScreen>> screensToLoad);

        /** @brief Joins the worker if a load was interrupted. */
        ~LoadingScreen() override;

        /**
         * @brief Updates the loading screen.
         *
         * @param gameTime Timing information.
         * @param otherScreenHasFocus Whether another screen has focus.
         * @param coveredByOtherScreen Whether another screen covers this one.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime,
                    bool otherScreenHasFocus, bool coveredByOtherScreen) override;
        /** @brief Draws the loading screen. @param gameTime Timing information. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        LoadingScreen(ScreenManager& screenManager, bool loadingIsSlow,
                      std::vector<std::shared_ptr<GameScreen>> screensToLoad);

        void DrawLoadingMessage(const Microsoft::Xna::Framework::GameTime& gameTime);
        void BackgroundWorkerThread();
        Microsoft::Xna::Framework::GameTime GetGameTime(long long& lastTime);
        void DrawLoadAnimation(const Microsoft::Xna::Framework::GameTime& gameTime);
        void UpdateNetworkSession();

        bool loadingIsSlow_;
        bool otherScreensAreGone_ = false;
        std::vector<std::shared_ptr<GameScreen>> screensToLoad_;

        std::optional<std::thread> backgroundThread;
        std::unique_ptr<System::Threading::EventWaitHandle> backgroundThreadExit;

        Microsoft::Xna::Framework::Graphics::GraphicsDevice* graphicsDevice = nullptr;
        Microsoft::Xna::Framework::Net::NetworkSession* networkSession = nullptr;
        IMessageDisplay* messageDisplay = nullptr;

        Microsoft::Xna::Framework::GameTime loadStartTime;
        System::TimeSpan loadAnimationTimer = System::TimeSpan::Zero;
    };
}
