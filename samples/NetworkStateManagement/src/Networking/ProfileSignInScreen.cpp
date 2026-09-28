// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ProfileSignInScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/ProfileSignInScreen.hpp"

#include <memory>

#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPrivileges.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Networking/NetworkSessionComponent.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/MessageBoxScreen.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework::GamerServices;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Net::NetworkSessionType;

    ProfileSignInScreen::ProfileSignInScreen(NetworkSessionType sessionType)
        : sessionType(sessionType)
    {
        setIsPopupProperty(true);
    }

    void ProfileSignInScreen::Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        if (ValidProfileSignedIn())
        {
            // As soon as we detect a suitable profile is signed in,
            // we raise the profile signed in event, then go away.
            ProfileSignedIn.Raise(this, System::EventArgs());

            ExitScreen();
        }
        else if (getIsActiveProperty() && !Guide::getIsVisibleProperty())
        {
            // If we are in trial mode, and they want to play online, and a profile
            // is signed in, take them to marketplace so they can purchase the game.
            if (Guide::getIsTrialModeProperty() && NetworkSessionComponent::IsOnlineSessionType(sessionType) &&
                ((*Gamer::getSignedInGamersProperty())[getControllingPlayerProperty().value()] != nullptr) &&
                !haveShownMarketplace)
            {
                ShowMarketplace();

                haveShownMarketplace = true;
            }
            else if (!haveShownGuide && !haveShownMarketplace)
            {
                // No suitable profile is signed in, and we haven't already shown
                // the Guide. Let's show it now, so they can sign in a profile.
                Guide::ShowSignIn(1, NetworkSessionComponent::IsOnlineSessionType(sessionType));

                haveShownGuide = true;
            }
            else
            {
                // Hmm. No suitable profile is signed in, but we already showed
                // the Guide, and the Guide isn't still visible. There is only
                // one thing that can explain this: they must have cancelled the
                // Guide without signing in a profile. We'd better just exit,
                // which will leave us on the same menu as before.
                ExitScreen();
            }
        }
    }

    // Helper checks whether a valid player profile is signed in.
    bool ProfileSignInScreen::ValidProfileSignedIn() const
    {
        // If there is no profile signed in, that is never good.
        SignedInGamer* gamer = (*Gamer::getSignedInGamersProperty())[getControllingPlayerProperty().value()];

        if (gamer == nullptr)
            return false;

        // If we want to play in a Live session, also make sure the profile is
        // signed in to Live, and that it has the privilege for online gameplay.
        if (NetworkSessionComponent::IsOnlineSessionType(sessionType))
        {
            if (!gamer->getIsSignedInToLiveProperty())
                return false;

            if (!gamer->getPrivilegesProperty().getAllowOnlineSessionsProperty())
                return false;
        }

        // Okeydokey, this looks good.
        return true;
    }

    // LIVE networking is not supported in trial mode. Rather than just giving the user an error,
    // this function asks if they want to purchase the full game, then takes them to Marketplace
    // where they can do that.
    void ProfileSignInScreen::ShowMarketplace()
    {
        auto confirmMarketplaceMessageBox = std::make_shared<MessageBoxScreen>(Resources::ConfirmMarketplace);

        confirmMarketplaceMessageBox->Accepted += [this](System::Object*, const PlayerIndexEventArgs&)
        {
            Guide::ShowMarketplace(getControllingPlayerProperty().value());
        };

        getScreenManagerProperty().AddScreen(std::move(confirmMarketplaceMessageBox), getControllingPlayerProperty());
    }

    const std::string& ProfileSignInScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.ProfileSignInScreen";
        return name;
    }
}
