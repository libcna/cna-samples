// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Screens/MenuScreen.hpp"

namespace GameStateManagement
{
    /** @brief Demonstrates persistent selectable options over the main menu. */
    class OptionsMenuScreen final : public MenuScreen
    {
    public:
        /** @brief Constructs the options menu and its entries. */
        OptionsMenuScreen();
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        enum class Ungulate
        {
            BactrianCamel,
            Dromedary,
            Llama
        };

        void SetMenuEntryText();
        void UngulateMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void LanguageMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void FrobnicateMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);
        void ElfMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs& e);

        std::shared_ptr<MenuEntry> ungulateMenuEntry_;
        std::shared_ptr<MenuEntry> languageMenuEntry_;
        std::shared_ptr<MenuEntry> frobnicateMenuEntry_;
        std::shared_ptr<MenuEntry> elfMenuEntry_;

        static Ungulate currentUngulate_;
        static const std::array<std::string, 3> languages_;
        static int currentLanguage_;
        static bool frobnicate_;
        static int elf_;
    };
}
