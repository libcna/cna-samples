// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Screens/MenuScreen.hpp"

namespace GameStateManagement
{
    /** @brief Pauses gameplay and offers resume or quit. */
    class PauseMenuScreen final : public MenuScreen
    {
    public:
        /** @brief Constructs the pause-menu entries. */
        PauseMenuScreen();
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        void QuitGameMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void ConfirmQuitMessageBoxAccepted(System::Object* sender, const PlayerIndexEventArgs& e);
    };
}
