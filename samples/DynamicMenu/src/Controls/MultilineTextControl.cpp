// SPDX-License-Identifier: MS-PL
#include "MultilineTextControl.hpp"

#include <utility>

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "System/String.hpp"

namespace DynamicMenu::Controls
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;

    const std::vector<std::string>& MultilineTextControl::getLinesProperty() const { return lines_; }
    int MultilineTextControl::getTopSpaceProperty() const { return topSpace_; }
    void MultilineTextControl::setTopSpaceProperty(int value) { topSpace_ = value; }
    int MultilineTextControl::getLeftSpaceProperty() const { return leftSpace_; }
    void MultilineTextControl::setLeftSpaceProperty(int value) { leftSpace_ = value; }

    void MultilineTextControl::LoadContent(GraphicsDevice& graphics, ContentManager& content)
    {
        TextControl::LoadContent(graphics, content);
        CalculateLines();
    }

    void MultilineTextControl::Draw(const GameTime& gameTime, SpriteBatch& spriteBatch)
    {
        TextControl::Draw(gameTime, spriteBatch);
        SpriteFont* font = getFontProperty();
        if (font == nullptr) return;
        const Vector2 extents = font->MeasureString("A");
        const Point topLeft = GetAbsoluteTopLeft();
        int currY = topLeft.Y + topSpace_ + VertSpace;
        const int left = topLeft.X + leftSpace_ + HorzSpace;
        for (const std::string& line : lines_)
        {
            if (line.empty()) continue;
            spriteBatch.DrawString(*font, line,
                Vector2(static_cast<float>(left), static_cast<float>(currY)),
                getTextColorProperty(), 0.0f, Vector2::Zero, 1.0f, SpriteEffects::None, 1.0f);
            currY += static_cast<int>(extents.Y) + VertSpace;
        }
    }

    void MultilineTextControl::CalculateLines()
    {
        lines_.clear();
        if (getTextProperty().empty()) return;
        (void) System::String::Trim(getTextProperty());
        const int lineWidth = getWidthProperty() - HorzSpace * 2 - leftSpace_;
        const auto words = System::String::Split(getTextProperty(), std::vector<char>{' ', '\n'});
        std::string line;
        SpriteFont* font = getFontProperty();
        if (font == nullptr) return;
        for (const std::string& word : words)
        {
            if (word.empty()) continue;
            std::string candidate = line;
            if (!candidate.empty()) candidate += " ";
            candidate += word;
            if (font->MeasureString(candidate).X > static_cast<float>(lineWidth))
            {
                lines_.push_back(line);
                line = word;
            }
            else
            {
                line = std::move(candidate);
            }
        }
        if (!line.empty()) lines_.push_back(line);
    }

    const std::string& MultilineTextControl::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.MultilineTextControl";
        return name;
    }
}
