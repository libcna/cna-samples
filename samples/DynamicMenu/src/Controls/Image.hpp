// SPDX-License-Identifier: MS-PL
#pragma once

#include "Control.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Image control whose behavior is supplied entirely by Control. */
    class Image : public Control
    {
    public:
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;
    };
}
