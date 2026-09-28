// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Screens/MenuScreen.hpp"

namespace NetworkStateManagement
{
    /** @brief Displays the initial Play, Options and Exit menu. */
    class MainMenuScreen final : public MenuScreen
    {
    public:
        /** @brief Constructs and wires the main-menu entries. */
        MainMenuScreen();
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Asks the player to confirm leaving the sample. @param playerIndex Triggering player. */
        void OnCancel(Microsoft::Xna::Framework::PlayerIndex playerIndex) override;

    private:
        void SinglePlayerMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void LiveMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void SystemLinkMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void CreateOrFindSession(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType,
                                 Microsoft::Xna::Framework::PlayerIndex playerIndex);
        void ConfirmExitMessageBoxAccepted(System::Object* sender, const PlayerIndexEventArgs& e);
    };
}
