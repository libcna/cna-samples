// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ProfileSignInScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief In order to play a networked game, you must have a player profile signed in. If you
     * want to play on Live, that has to be a Live profile. Rather than just failing with an error
     * message, it is nice if we can automatically bring up the Guide screen when we detect that no
     * suitable profiles are currently signed in, so the user can easily correct the problem. This
     * screen checks the sign in state, and brings up the Guide user interface if there is a problem
     * with it. It then raises an event as soon as a valid profile has been signed in.
     */
    class ProfileSignInScreen final : public GameScreen
    {
    public:
        /** @brief Raised once a suitable profile is signed in. */
        System::EventHandler<System::EventArgs> ProfileSignedIn;

        /**
         * @brief Constructs a new profile sign in screen.
         *
         * @param sessionType The kind of session the profile must be able to join.
         */
        explicit ProfileSignInScreen(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);

        /**
         * @brief Updates the profile sign in screen.
         *
         * @param gameTime Timing information.
         * @param otherScreenHasFocus Whether another screen has focus.
         * @param coveredByOtherScreen Whether another screen covers this one.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime, bool otherScreenHasFocus,
                    bool coveredByOtherScreen) override;

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        [[nodiscard]] bool ValidProfileSignedIn() const;
        void ShowMarketplace();

        Microsoft::Xna::Framework::Net::NetworkSessionType sessionType;
        bool haveShownGuide = false;
        bool haveShownMarketplace = false;
    };
}
