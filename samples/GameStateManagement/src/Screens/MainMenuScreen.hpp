// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Screens/MenuScreen.hpp"

namespace GameStateManagement
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
        void PlayGameMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void OptionsMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void ConfirmExitMessageBoxAccepted(System::Object* sender, const PlayerIndexEventArgs& e);
    };
}
