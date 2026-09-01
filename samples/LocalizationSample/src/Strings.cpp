// SPDX-License-Identifier: MS-PL

#include "Strings.hpp"

#include <array>
#include <optional>
#include <string>
#include <string_view>

namespace Localization
{
    namespace
    {
        struct ResourceEntry
        {
            std::string_view cultureName;
            std::string_view resourceName;
            std::string_view value;
        };

        constexpr std::array<ResourceEntry, 15> Resources{{
            {"", "CurrentLocale", "Current culture: {0}"},
            {"", "HowToChange", "To change this, alter your system settings, then restart the sample"},
            {"", "Welcome", "Welcome to the localization sample!"},
            {"da", "CurrentLocale", "Nuværende kultur: {0}"},
            {"da", "HowToChange", "For at ændre denne, skift system instillinger og genstart eksemplet"},
            {"da", "Welcome", "Velkommen til lokaliserings eksemplet!"},
            {"fr", "CurrentLocale", "Localisé en: {0}"},
            {"fr", "HowToChange", "Pour changer cette valeur modifiez vos paramètres\nsystème puis redémarrez cet exemple"},
            {"fr", "Welcome", "Bienvenue dans l'exemple de localisation!"},
            {"ja", "CurrentLocale", "現在のローケル: {0}"},
            {"ja", "HowToChange", "この設定を変更するにはシステム設定を変更し、サンプルを再実行してください。"},
            {"ja", "Welcome", "ローカライズサンプルへようこそ！"},
            {"ko", "CurrentLocale", "현재 언어: {0}"},
            {"ko", "HowToChange", "변경을 위해서는, 시스템 설정을 바꿔야 합니다, 그리고 샘플을 재시동하십시오"},
            {"ko", "Welcome", "번역 샘플을 환영합니다!"},
        }};

        [[nodiscard]] std::optional<std::string> LookupResource(
            const std::string_view baseName,
            const std::string_view cultureName,
            const std::string_view resourceName)
        {
            if (baseName != "Localization.Strings")
            {
                return std::nullopt;
            }

            for (const ResourceEntry& entry : Resources)
            {
                if (entry.cultureName == cultureName && entry.resourceName == resourceName)
                {
                    return std::string(entry.value);
                }
            }

            return std::nullopt;
        }
    }

    System::Resources::ResourceManager Strings::resourceMan_{
        "Localization.Strings",
        LookupResource,
    };
    std::optional<System::Globalization::CultureInfo> Strings::culture_;

    const std::optional<System::Globalization::CultureInfo>& Strings::getCultureProperty()
    {
        return culture_;
    }

    void Strings::setCultureProperty(const System::Globalization::CultureInfo& value)
    {
        culture_ = value;
    }

    std::string Strings::getCurrentLocaleProperty()
    {
        return resourceMan_.GetString("CurrentLocale", culture_).value();
    }

    std::string Strings::getHowToChangeProperty()
    {
        return resourceMan_.GetString("HowToChange", culture_).value();
    }

    std::string Strings::getWelcomeProperty()
    {
        return resourceMan_.GetString("Welcome", culture_).value();
    }
}
