// SPDX-License-Identifier: MS-PL
#include "ProgressBar.hpp"

#include <cmath>
#include <utility>

namespace DynamicMenu::Controls
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;

    ProgressBar::ProgressBar() = default;
    const std::string& ProgressBar::getLeftTextureNameProperty() const { return leftTextureName_; }
    void ProgressBar::setLeftTextureNameProperty(std::string value) { leftTextureName_ = std::move(value); }
    const std::string& ProgressBar::getRightTextureNameProperty() const { return rightTextureName_; }
    void ProgressBar::setRightTextureNameProperty(std::string value) { rightTextureName_ = std::move(value); }
    int ProgressBar::getPositionProperty() const { return position_; }
    void ProgressBar::setPositionProperty(int value) { position_ = value; }
    int ProgressBar::getMaxValueProperty() const { return maxValue_; }
    void ProgressBar::setMaxValueProperty(int value) { maxValue_ = value; }
    Color ProgressBar::getLeftColorProperty() const { return leftColor_; }
    void ProgressBar::setLeftColorProperty(Color value) { leftColor_ = value; }
    Color ProgressBar::getRightColorProperty() const { return rightColor_; }
    void ProgressBar::setRightColorProperty(Color value) { rightColor_ = value; }
    int ProgressBar::getBorderWidthProperty() const { return borderWidth_; }
    void ProgressBar::setBorderWidthProperty(int value) { borderWidth_ = value; }

    void ProgressBar::LoadContent(GraphicsDevice& graphics, ContentManager& content)
    {
        Control::LoadContent(graphics, content);
        if (!leftTextureName_.empty()) leftTexture_.emplace(content.Load<Texture2D>(leftTextureName_));
        if (!rightTextureName_.empty()) rightTexture_.emplace(content.Load<Texture2D>(rightTextureName_));
    }

    void ProgressBar::Draw(const GameTime& gameTime, SpriteBatch& spriteBatch)
    {
        Control::Draw(gameTime, spriteBatch);
        const int leftSideWidth = GetLeftSideWidth();

        Rectangle rect = GetAbsoluteRect();
        rect.Width = leftSideWidth;
        rect.X += borderWidth_;
        rect.Y += borderWidth_;
        rect.Height -= borderWidth_ * 2;
        Rectangle sourceRect;
        sourceRect.Width = rect.Width;
        sourceRect.Height = rect.Height;
        if (leftTexture_) spriteBatch.Draw(*leftTexture_, rect, sourceRect, leftColor_);

        rect = GetAbsoluteRect();
        rect.X += leftSideWidth + borderWidth_;
        rect.Width -= borderWidth_ * 2 + leftSideWidth;
        rect.Y += borderWidth_;
        rect.Height -= borderWidth_ * 2;
        sourceRect = Rectangle();
        sourceRect.Width = rect.Width;
        sourceRect.Height = rect.Height;
        sourceRect.X = leftSideWidth;
        if (rightTexture_) spriteBatch.Draw(*rightTexture_, rect, sourceRect, rightColor_);
    }

    int ProgressBar::GetLeftSideWidth() const
    {
        const float balance = static_cast<float>(position_) / static_cast<float>(maxValue_);
        const int progressBarWidth = getWidthProperty() - borderWidth_ * 2;
        return static_cast<int>(std::floor(static_cast<float>(progressBarWidth) * balance));
    }

    const std::string& ProgressBar::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.ProgressBar";
        return name;
    }
}
