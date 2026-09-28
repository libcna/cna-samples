// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// JoinSessionScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Networking/OperationCompletedEventArgs.hpp"
#include "Screens/MenuScreen.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief This menu screen displays a list of available network sessions, and lets the user
     * choose which one to join.
     */
    class JoinSessionScreen final : public MenuScreen
    {
    public:
        /**
         * @brief Constructs a menu screen listing the available network sessions.
         *
         * @param availableSessions The sessions a search found; the screen keeps them.
         */
        explicit JoinSessionScreen(Microsoft::Xna::Framework::Net::AvailableNetworkSessionCollection availableSessions);

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        static constexpr int MaxSearchResults = 8;

        void AvailableSessionMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void JoinSessionOperationCompleted(System::Object* sender, const OperationCompletedEventArgs& e);
        void BackMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);

        Microsoft::Xna::Framework::Net::AvailableNetworkSessionCollection availableSessions;
    };
}
