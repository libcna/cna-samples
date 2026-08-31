// SPDX-License-Identifier: MS-PL
#include "Image.hpp"

namespace DynamicMenu::Controls
{
    const std::string& Image::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.Image";
        return name;
    }
}
