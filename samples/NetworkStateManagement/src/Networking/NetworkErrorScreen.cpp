// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkErrorScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/NetworkErrorScreen.hpp"

#include <cstdio>

#include "Microsoft/Xna/Framework/GamerServices/GamerPrivilegeException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/NetworkException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/NetworkNotAvailableException.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionJoinError.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionJoinException.hpp"
#include "Resources.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Net;

    NetworkErrorScreen::NetworkErrorScreen(const std::exception& exception)
        : MessageBoxScreen(GetErrorMessage(exception), false)
    {
    }

    std::string NetworkErrorScreen::GetErrorMessage(const std::exception& exception)
    {
        // Debug.WriteLine
        std::fprintf(stderr, "Network operation threw %s\n", exception.what());

        // Is this a GamerPrivilegeException?
        if (dynamic_cast<const GamerPrivilegeException*>(&exception) != nullptr)
        {
            if (Guide::getIsTrialModeProperty())
                return Resources::ErrorTrialMode;
            return Resources::ErrorGamerPrivilege;
        }

        // Is it a NetworkSessionJoinException?
        if (const auto* joinException = dynamic_cast<const NetworkSessionJoinException*>(&exception))
        {
            switch (joinException->getJoinErrorProperty())
            {
            case NetworkSessionJoinError::SessionFull:
                return Resources::ErrorSessionFull;
            case NetworkSessionJoinError::SessionNotFound:
                return Resources::ErrorSessionNotFound;
            case NetworkSessionJoinError::SessionNotJoinable:
                return Resources::ErrorSessionNotJoinable;
            default:
                break;
            }
        }

        // Is this a NetworkNotAvailableException?
        if (dynamic_cast<const NetworkNotAvailableException*>(&exception) != nullptr)
        {
            return Resources::ErrorNetworkNotAvailable;
        }

        // Is this a NetworkException?
        if (dynamic_cast<const NetworkException*>(&exception) != nullptr)
        {
            return Resources::ErrorNetwork;
        }

        // Otherwise just a generic error message.
        return Resources::ErrorUnknown;
    }

    const std::string& NetworkErrorScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.NetworkErrorScreen";
        return name;
    }
}
