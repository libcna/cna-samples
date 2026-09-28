// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AvailableSessionMenuEntry.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Screens/MenuEntry.hpp"

namespace Microsoft::Xna::Framework::Net
{
    class AvailableNetworkSession;
}

namespace NetworkStateManagement
{
    /**
     * @brief Helper class customizes the standard MenuEntry class for displaying
     * AvailableNetworkSession objects.
     */
    class AvailableSessionMenuEntry final : public MenuEntry
    {
    public:
        /**
         * @brief Constructs a menu entry describing an available network session.
         *
         * @param availableSession The session; owned by the caller's collection.
         */
        explicit AvailableSessionMenuEntry(const Microsoft::Xna::Framework::Net::AvailableNetworkSession& availableSession);

        /** @brief Gets the available network session corresponding to this menu entry. @return The session. */
        [[nodiscard]] const Microsoft::Xna::Framework::Net::AvailableNetworkSession& getAvailableSessionProperty() const;

        /**
         * @brief Updates the menu item text, adding information about the network quality of service.
         *
         * @param screen The owning menu.
         * @param isSelected Whether the entry is selected.
         * @param gameTime Timing information.
         */
        void Update(MenuScreen& screen, bool isSelected, Microsoft::Xna::Framework::GameTime& gameTime) override;

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        static std::string GetMenuItemText(const Microsoft::Xna::Framework::Net::AvailableNetworkSession& session);

        const Microsoft::Xna::Framework::Net::AvailableNetworkSession& availableSession;
        bool gotQualityOfService = false;
    };
}
