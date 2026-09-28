// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Screens/MenuScreen.hpp"

namespace Microsoft::Xna::Framework::Net
{
    class NetworkSession;
}

namespace NetworkStateManagement
{
    /** @brief Pauses gameplay and offers resume or quit. */
    class PauseMenuScreen final : public MenuScreen
    {
    public:
        /** @brief Constructs the pause-menu entries. */
        explicit PauseMenuScreen(std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession);
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        void QuitGameMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void ConfirmQuitMessageBoxAccepted(System::Object* sender, const PlayerIndexEventArgs& e);
        void ReturnToLobbyMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void LeaveSessionMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);

        std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession;
    };
}
