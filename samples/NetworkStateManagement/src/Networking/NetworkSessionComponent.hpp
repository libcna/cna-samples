// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkSessionComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Networking/OperationCompletedEventArgs.hpp"

namespace Microsoft::Xna::Framework::GamerServices
{
    class InviteAcceptedEventArgs;
    class SignedInGamer;
}

namespace Microsoft::Xna::Framework::Net
{
    class GamerJoinedEventArgs;
    class GamerLeftEventArgs;
    class NetworkSession;
    class NetworkSessionEndedEventArgs;
}

namespace NetworkStateManagement
{
    class IMessageDisplay;
    class ScreenManager;

    /**
     * @brief Component in charge of owning and updating the current NetworkSession object. This is
     * responsible for calling NetworkSession.Update at regular intervals, and also exposes the
     * NetworkSession as a game service which can easily be looked up by any other code that needs
     * to access it.
     */
    class NetworkSessionComponent final : public Microsoft::Xna::Framework::GameComponent,
                                          public std::enable_shared_from_this<NetworkSessionComponent>
    {
    public:
        /** @brief Most gamers in a session. */
        static constexpr int MaxGamers = 16;
        /** @brief Most local gamers in a session. */
        static constexpr int MaxLocalGamers = 4;

        /**
         * @brief Creates a new NetworkSessionComponent.
         *
         * @param screenManager The screen manager.
         * @param networkSession The session the component owns and updates.
         */
        static void Create(ScreenManager& screenManager, std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession);

        using GameComponent::Dispose;

        /** @brief Initializes the component. */
        void Initialize() override;

        /** @brief Updates the network session. @param gameTime Timing information. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Event handler called when the system delivers an invite notification.
         *
         * @param screenManager The screen manager.
         * @param e The accepted invitation.
         */
        static void InviteAccepted(ScreenManager& screenManager,
                                   const Microsoft::Xna::Framework::GamerServices::InviteAcceptedEventArgs& e);

        /**
         * @brief Checks whether the specified session type is online.
         *
         * @param sessionType The session type.
         * @return Whether the type needs an online profile.
         */
        static bool IsOnlineSessionType(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);

        /**
         * @brief Decides which local player profiles should be included in a network session.
         *
         * @param sessionType The session type.
         * @param playerIndex The primary player.
         * @return The profiles, primary (or a non-guest) first.
         */
        static std::vector<Microsoft::Xna::Framework::GamerServices::SignedInGamer*> ChooseGamers(
            Microsoft::Xna::Framework::Net::NetworkSessionType sessionType, Microsoft::Xna::Framework::PlayerIndex playerIndex);

        /**
         * @brief Public method called when the user wants to leave the network session.
         *
         * @param screenManager The screen manager.
         * @param playerIndex The player asking.
         */
        static void LeaveSession(ScreenManager& screenManager, Microsoft::Xna::Framework::PlayerIndex playerIndex);

        /**
         * @brief Releases components disposed during earlier frames. The game's component loop keeps
         * raw pointers for the frame being updated, so a component that leaves its session is kept
         * alive until the game calls this before its next update.
         */
        static void ReleaseRetired();

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /**
         * @brief Shuts down the component.
         *
         * @param disposing Whether managed state is released.
         */
        void Dispose(bool disposing) override;

    private:
        NetworkSessionComponent(ScreenManager& screenManager,
                                std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession);

        void GamerJoined(System::Object* sender, const Microsoft::Xna::Framework::Net::GamerJoinedEventArgs& e);
        void GamerLeft(System::Object* sender, const Microsoft::Xna::Framework::Net::GamerLeftEventArgs& e);
        void NetworkSessionEnded(System::Object* sender, const Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs& e);
        static void JoinInvitedOperationCompleted(System::Object* sender, const OperationCompletedEventArgs& e);
        void LeaveSession();
        static NetworkSessionComponent* FindSessionComponent(Microsoft::Xna::Framework::Game& game);

        ScreenManager& screenManager;
        std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession;
        IMessageDisplay* messageDisplay = nullptr;
        bool notifyWhenPlayersJoinOrLeave = false;
        std::string sessionEndMessage;
    };
}
