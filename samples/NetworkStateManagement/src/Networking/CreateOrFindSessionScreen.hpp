// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CreateOrFindSessionScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Networking/OperationCompletedEventArgs.hpp"
#include "Screens/MenuScreen.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief This menu screen lets the user choose whether to create a new network session, or
     * search for an existing session to join.
     */
    class CreateOrFindSessionScreen final : public MenuScreen
    {
    public:
        /**
         * @brief Constructor fills in the menu contents.
         *
         * @param sessionType System Link or PlayerMatch.
         */
        explicit CreateOrFindSessionScreen(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        static std::string GetMenuTitle(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);

        void CreateSessionMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void CreateSessionOperationCompleted(System::Object* sender, const OperationCompletedEventArgs& e);
        void FindSessionsMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void FindSessionsOperationCompleted(System::Object* sender, const OperationCompletedEventArgs& e);

        Microsoft::Xna::Framework::Net::NetworkSessionType sessionType;
    };
}
