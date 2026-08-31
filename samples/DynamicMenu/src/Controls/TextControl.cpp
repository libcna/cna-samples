// SPDX-License-Identifier: MS-PL
#include "TextControl.hpp"

#include <utility>

namespace DynamicMenu::Controls
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Vector2;

    TextControl::TextControl() = default;
    TextControl::~TextControl() = default;
    const std::string& TextControl::getTextProperty() const { return text_; }
    void TextControl::setTextProperty(std::string value) { text_ = std::move(value); }
    const std::string& TextControl::getFontNameProperty() const { return fontName_; }
    void TextControl::setFontNameProperty(std::string value) { fontName_ = std::move(value); }
    SpriteFont* TextControl::getFontProperty() { return font_ ? &*font_ : nullptr; }
    Color TextControl::getTextColorProperty() const { return textColor_; }
    void TextControl::setTextColorProperty(Color value) { textColor_ = value; }

    void TextControl::AutoPickWidth()
    {
        const Vector2 dim = font_->MeasureString(text_);
        setWidthProperty(Space * 2 + static_cast<int>(dim.X));
    }

    void TextControl::LoadContent(GraphicsDevice& graphics, ContentManager& content)
    {
        Control::LoadContent(graphics, content);
        if (!fontName_.empty()) font_.emplace(content.Load<SpriteFont>(fontName_));
    }
}
