// SPDX-License-Identifier: MS-PL
#include "Label.hpp"

namespace DynamicMenu::Controls
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;

    void Label::Draw(const GameTime& gameTime, SpriteBatch& spriteBatch)
    {
        TextControl::Draw(gameTime, spriteBatch);
        DrawCenteredText(spriteBatch, getFontProperty(), GetAbsoluteRect(),
                         getTextProperty(), getTextColorProperty());
    }

    const std::string& Label::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.Label";
        return name;
    }
}
