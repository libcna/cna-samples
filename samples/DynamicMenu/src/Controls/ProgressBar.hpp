// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "Control.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Control that shows a split-texture progress indicator. */
    class ProgressBar : public Control
    {
    public:
        /** @brief Constructs a zero-of-100 white progress bar. */
        ProgressBar();
        /** @brief Gets the left texture asset name. @return Asset name. */
        [[nodiscard]] const std::string& getLeftTextureNameProperty() const;
        /** @brief Sets the left texture asset name. @param value Asset name. */
        void setLeftTextureNameProperty(std::string value);
        /** @brief Gets the right texture asset name. @return Asset name. */
        [[nodiscard]] const std::string& getRightTextureNameProperty() const;
        /** @brief Sets the right texture asset name. @param value Asset name. */
        void setRightTextureNameProperty(std::string value);
        /** @brief Gets current position. @return Position. */
        [[nodiscard]] int getPositionProperty() const;
        /** @brief Sets current position. @param value Position. */
        void setPositionProperty(int value);
        /** @brief Gets maximum value. @return Maximum. */
        [[nodiscard]] int getMaxValueProperty() const;
        /** @brief Sets maximum value. @param value Maximum. */
        void setMaxValueProperty(int value);
        /** @brief Gets the left tint. @return Tint color. */
        [[nodiscard]] Microsoft::Xna::Framework::Color getLeftColorProperty() const;
        /** @brief Sets the left tint. @param value Tint color. */
        void setLeftColorProperty(Microsoft::Xna::Framework::Color value);
        /** @brief Gets the right tint. @return Tint color. */
        [[nodiscard]] Microsoft::Xna::Framework::Color getRightColorProperty() const;
        /** @brief Sets the right tint. @param value Tint color. */
        void setRightColorProperty(Microsoft::Xna::Framework::Color value);
        /** @brief Gets border width. @return Width in pixels. */
        [[nodiscard]] int getBorderWidthProperty() const;
        /** @brief Sets border width. @param value Width in pixels. */
        void setBorderWidthProperty(int value);
        /** @brief Loads the background and both progress textures. @param graphics Graphics device. @param content Content manager. */
        void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content) override;
        /** @brief Draws the filled and unfilled progress regions. @param gameTime Current game time. @param spriteBatch Sprite batch. */
        void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) override;
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        [[nodiscard]] int GetLeftSideWidth() const;

        std::string leftTextureName_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> leftTexture_;
        std::string rightTextureName_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> rightTexture_;
        int position_ = 0;
        int maxValue_ = 100;
        Microsoft::Xna::Framework::Color leftColor_ = Microsoft::Xna::Framework::Color::White;
        Microsoft::Xna::Framework::Color rightColor_ = Microsoft::Xna::Framework::Color::White;
        int borderWidth_ = 0;
    };
}
