// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AvailableSessionMenuEntry.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/AvailableSessionMenuEntry.hpp"

#include <cmath>
#include <cstdio>

#include "Microsoft/Xna/Framework/Net/AvailableNetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/QualityOfService.hpp"
#include "Screens/MenuScreen.hpp"

namespace NetworkStateManagement
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Net::AvailableNetworkSession;

    AvailableSessionMenuEntry::AvailableSessionMenuEntry(const AvailableNetworkSession& availableSession)
        : MenuEntry(GetMenuItemText(availableSession))
        , availableSession(availableSession)
    {
    }

    // Formats session information to create the menu text string.
    std::string AvailableSessionMenuEntry::GetMenuItemText(const AvailableNetworkSession& session)
    {
        const int totalSlots = session.getCurrentGamerCountProperty() + session.getOpenPublicGamerSlotsProperty();

        return session.getHostGamertagProperty() + " (" + std::to_string(session.getCurrentGamerCountProperty()) + "/" +
               std::to_string(totalSlots) + ")";
    }

    const AvailableNetworkSession& AvailableSessionMenuEntry::getAvailableSessionProperty() const { return availableSession; }

    void AvailableSessionMenuEntry::Update(MenuScreen& screen, bool isSelected, GameTime& gameTime)
    {
        MenuEntry::Update(screen, isSelected, gameTime);

        // Quality of service data can take some time to query, so it will not
        // be filled in straight away when NetworkSession.Find returns. We want
        // to display the list of available sessions straight away, and then
        // fill in the quality of service data whenever that becomes available,
        // so we keep checking until this data shows up.
        if (screen.getIsActiveProperty() && !gotQualityOfService)
        {
            const auto& qualityOfService = availableSession.getQualityOfServiceProperty();

            if (qualityOfService.getIsAvailableProperty())
            {
                const auto pingTime = qualityOfService.getAverageRoundtripTimeProperty();

                char text[32];
                std::snprintf(text, sizeof(text), " - %.0f ms", pingTime.getTotalMillisecondsProperty());
                setTextProperty(getTextProperty() + text);

                gotQualityOfService = true;
            }
        }
    }

    const std::string& AvailableSessionMenuEntry::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.AvailableSessionMenuEntry";
        return name;
    }
}
