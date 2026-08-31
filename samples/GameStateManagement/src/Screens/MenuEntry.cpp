// SPDX-License-Identifier: MS-PL

#include "Screens/MenuEntry.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Screens/MenuScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    MenuEntry::MenuEntry(std::string text) : text_(std::move(text)) {}
    const std::string& MenuEntry::getTextProperty() const { return text_; }
    void MenuEntry::setTextProperty(std::string value) { text_ = std::move(value); }
    Vector2 MenuEntry::getPositionProperty() const { return position_; }
    void MenuEntry::setPositionProperty(Vector2 value) { position_ = value; }

    void MenuEntry::OnSelectEntry(PlayerIndex playerIndex)
    {
        Selected.Raise(this, PlayerIndexEventArgs(playerIndex));
    }

    void MenuEntry::Update(MenuScreen&, bool isSelected, GameTime& gameTime)
    {
#if defined(WINDOWS_PHONE)
        isSelected = false;
#endif
        const float fadeSpeed = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()) * 4.0f;
        selectionFade_ = isSelected
            ? std::min(selectionFade_ + fadeSpeed, 1.0f)
            : std::max(selectionFade_ - fadeSpeed, 0.0f);
    }

    void MenuEntry::Draw(MenuScreen& screen, bool isSelected, const GameTime& gameTime)
    {
#if defined(WINDOWS_PHONE)
        isSelected = false;
#endif
        Color color = isSelected ? Color::Yellow : Color::White;
        const double time = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();
        const float pulsate = static_cast<float>(std::sin(time * 6.0)) + 1.0f;
        const float scale = 1.0f + pulsate * 0.05f * selectionFade_;
        color = color * screen.getTransitionAlphaProperty();

        auto& screenManager = screen.getScreenManagerProperty();
        auto& spriteBatch = screenManager.getSpriteBatchProperty();
        auto& font = screenManager.getFontProperty();
        const Vector2 origin(0.0f, font.getLineSpacingProperty() / 2.0f);
        spriteBatch.DrawString(font, text_, position_, color, 0.0f, origin, scale,
                               SpriteEffects::None, 0.0f);
    }

    int MenuEntry::GetHeight(MenuScreen& screen) const
    { return screen.getScreenManagerProperty().getFontProperty().getLineSpacingProperty(); }
    int MenuEntry::GetWidth(MenuScreen& screen) const
    {
        return static_cast<int>(screen.getScreenManagerProperty().getFontProperty()
            .MeasureString(text_).X);
    }

    const std::string& MenuEntry::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.MenuEntry";
        return name;
    }
}
