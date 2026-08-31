// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "Control.hpp"
#include "ITextControl.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Abstract control that contains text and a font. */
    class TextControl : public Control, public ITextControl
    {
    public:
        /** @brief Constructs a black-text control. */
        TextControl();
        /** @brief Destroys the abstract text-control base. */
        ~TextControl() override = 0;

        /** @brief Gets the displayed text. @return Text content. */
        [[nodiscard]] const std::string& getTextProperty() const override;
        /** @brief Sets the displayed text. @param value Text content. */
        void setTextProperty(std::string value) override;
        /** @brief Gets the font asset name. @return Font asset name. */
        [[nodiscard]] const std::string& getFontNameProperty() const;
        /** @brief Sets the font asset name. @param value Font asset name. */
        void setFontNameProperty(std::string value);
        /** @brief Gets the loaded font. @return Font or null. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::SpriteFont* getFontProperty();
        /** @brief Gets the text color. @return Text color. */
        [[nodiscard]] Microsoft::Xna::Framework::Color getTextColorProperty() const;
        /** @brief Sets the text color. @param value Text color. */
        void setTextColorProperty(Microsoft::Xna::Framework::Color value);
        /** @brief Picks the width from the current text measurement. */
        void AutoPickWidth();
        /** @brief Loads the background and configured font. @param graphics Graphics device. @param content Content manager. */
        void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content) override;

    private:
        static constexpr int Space = 10;
        std::string text_;
        std::string fontName_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font_;
        Microsoft::Xna::Framework::Color textColor_ = Microsoft::Xna::Framework::Color::Black;
    };
}
