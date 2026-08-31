// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

namespace DynamicMenu::Controls
{
    /** @brief Interface for controls that show text. */
    class ITextControl
    {
    public:
        /** @brief Destroys the interface. */
        virtual ~ITextControl() = default;
        /** @brief Gets the displayed text. @return The text. */
        [[nodiscard]] virtual const std::string& getTextProperty() const = 0;
        /** @brief Sets the displayed text. @param value The text. */
        virtual void setTextProperty(std::string value) = 0;
    };
}
