// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>
#include <vector>

#include "TextControl.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Text control that wraps and draws multiple lines. */
    class MultilineTextControl : public TextControl
    {
    public:
        /** @brief Gets the wrapped lines. @return Lines. */
        [[nodiscard]] const std::vector<std::string>& getLinesProperty() const;
        /** @brief Gets top padding. @return Padding in pixels. */
        [[nodiscard]] int getTopSpaceProperty() const;
        /** @brief Sets top padding. @param value Padding in pixels. */
        void setTopSpaceProperty(int value);
        /** @brief Gets left padding. @return Padding in pixels. */
        [[nodiscard]] int getLeftSpaceProperty() const;
        /** @brief Sets left padding. @param value Padding in pixels. */
        void setLeftSpaceProperty(int value);
        /** @brief Loads content and calculates wrapped lines. @param graphics Graphics device. @param content Content manager. */
        void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content) override;
        /** @brief Draws the background and wrapped lines. @param gameTime Current game time. @param spriteBatch Sprite batch. */
        void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) override;
        /** @brief Recalculates wrapped lines from the current size and font. */
        virtual void CalculateLines();
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        static constexpr int HorzSpace = 10;
        static constexpr int VertSpace = 5;

    private:
        int topSpace_ = 0;
        int leftSpace_ = 0;
        std::vector<std::string> lines_;
    };
}
