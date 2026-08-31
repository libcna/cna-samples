// SPDX-License-Identifier: MS-PL
#pragma once

#include "TextControl.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Simple control that shows centered text. */
    class Label : public TextControl
    {
    public:
        /** @brief Draws the label and its centered text. @param gameTime Current game time. @param spriteBatch Sprite batch. */
        void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) override;
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;
    };
}
