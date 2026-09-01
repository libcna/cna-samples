// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "System/Globalization/CultureInfo.hpp"
#include "System/Resources/ResourceManager.hpp"

namespace Localization
{
    /** @brief Strongly typed access to the sample's localized string resources. */
    class Strings final
    {
    public:
        /** @brief Gets the culture used for resource lookups. */
        [[nodiscard]] static const std::optional<System::Globalization::CultureInfo>&
        getCultureProperty();

        /** @brief Sets the culture used for resource lookups. */
        static void setCultureProperty(const System::Globalization::CultureInfo& value);

        /** @brief Gets the localized current-locale format string. */
        [[nodiscard]] static std::string getCurrentLocaleProperty();

        /** @brief Gets the localized instructions for changing the system locale. */
        [[nodiscard]] static std::string getHowToChangeProperty();

        /** @brief Gets the localized welcome text. */
        [[nodiscard]] static std::string getWelcomeProperty();

    private:
        static System::Resources::ResourceManager resourceMan_;
        static std::optional<System::Globalization::CultureInfo> culture_;
    };
}
