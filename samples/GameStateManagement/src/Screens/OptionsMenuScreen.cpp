// SPDX-License-Identifier: MS-PL

#include "Screens/OptionsMenuScreen.hpp"

#include <array>
#include <memory>
#include <string>
#include <utility>

namespace GameStateManagement
{
    OptionsMenuScreen::Ungulate OptionsMenuScreen::currentUngulate_ = Ungulate::Dromedary;
    const std::array<std::string, 3> OptionsMenuScreen::languages_ = {
        "C#", "French", "Deoxyribonucleic acid"
    };
    int OptionsMenuScreen::currentLanguage_ = 0;
    bool OptionsMenuScreen::frobnicate_ = true;
    int OptionsMenuScreen::elf_ = 23;

    OptionsMenuScreen::OptionsMenuScreen()
        : MenuScreen("Options"),
          ungulateMenuEntry_(std::make_shared<MenuEntry>("")),
          languageMenuEntry_(std::make_shared<MenuEntry>("")),
          frobnicateMenuEntry_(std::make_shared<MenuEntry>("")),
          elfMenuEntry_(std::make_shared<MenuEntry>(""))
    {
        SetMenuEntryText();
        auto back = std::make_shared<MenuEntry>("Back");

        ungulateMenuEntry_->Selected += [this](System::Object* sender,
                                                const PlayerIndexEventArgs& e)
        {
            UngulateMenuEntrySelected(sender, e);
        };
        languageMenuEntry_->Selected += [this](System::Object* sender,
                                                const PlayerIndexEventArgs& e)
        {
            LanguageMenuEntrySelected(sender, e);
        };
        frobnicateMenuEntry_->Selected += [this](System::Object* sender,
                                                  const PlayerIndexEventArgs& e)
        {
            FrobnicateMenuEntrySelected(sender, e);
        };
        elfMenuEntry_->Selected += [this](System::Object* sender,
                                           const PlayerIndexEventArgs& e)
        {
            ElfMenuEntrySelected(sender, e);
        };
        back->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            MenuScreen::OnCancel(sender, e);
        };

        auto& entries = getMenuEntriesProperty();
        entries.push_back(ungulateMenuEntry_);
        entries.push_back(languageMenuEntry_);
        entries.push_back(frobnicateMenuEntry_);
        entries.push_back(elfMenuEntry_);
        entries.push_back(std::move(back));
    }

    void OptionsMenuScreen::SetMenuEntryText()
    {
        static constexpr std::array<const char*, 3> ungulateNames = {
            "BactrianCamel", "Dromedary", "Llama"
        };
        ungulateMenuEntry_->setTextProperty(
            "Preferred ungulate: " +
            std::string(ungulateNames[static_cast<std::size_t>(currentUngulate_)]));
        languageMenuEntry_->setTextProperty(
            "Language: " + languages_[static_cast<std::size_t>(currentLanguage_)]);
        frobnicateMenuEntry_->setTextProperty(
            std::string("Frobnicate: ") + (frobnicate_ ? "on" : "off"));
        elfMenuEntry_->setTextProperty("elf: " + std::to_string(elf_));
    }

    void OptionsMenuScreen::UngulateMenuEntrySelected(System::Object*,
                                                       const PlayerIndexEventArgs&)
    {
        const int next = static_cast<int>(currentUngulate_) + 1;
        currentUngulate_ = next > static_cast<int>(Ungulate::Llama)
            ? Ungulate::BactrianCamel
            : static_cast<Ungulate>(next);
        SetMenuEntryText();
    }

    void OptionsMenuScreen::LanguageMenuEntrySelected(System::Object*,
                                                       const PlayerIndexEventArgs&)
    {
        currentLanguage_ = (currentLanguage_ + 1) % static_cast<int>(languages_.size());
        SetMenuEntryText();
    }

    void OptionsMenuScreen::FrobnicateMenuEntrySelected(System::Object*,
                                                         const PlayerIndexEventArgs&)
    {
        frobnicate_ = !frobnicate_;
        SetMenuEntryText();
    }

    void OptionsMenuScreen::ElfMenuEntrySelected(System::Object*,
                                                  const PlayerIndexEventArgs&)
    {
        ++elf_;
        SetMenuEntryText();
    }

    const std::string& OptionsMenuScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.OptionsMenuScreen";
        return name;
    }
}
