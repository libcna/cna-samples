#pragma once

// MenuBodies.hpp -- the bodies of MenuEntry and MenuScreen that need ScreenManager complete.
//
// A menu entry draws with the screen manager's sprite batch, font and blank texture, and the
// screen manager owns the screens, so the two types reference each other. C# resolves that for
// itself; in C++ the cycle is broken by declaring the members where they belong and closing
// them here, once both classes are whole. Included from the end of ScreenManager.hpp.

#include <algorithm>
#include <cmath>

#include "../ScreenManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "MenuEntry.hpp"
#include "MenuScreen.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::Graphics::SpriteFont;

inline void MenuEntry::Update(MenuScreen& screen, bool isSelected, const GameTime& gameTime)
{
    (void)screen;
    // There is no selected entry on a touch menu, so the fade never starts.
    isSelected = false;

    // When the menu selection changes, entries gradually fade between their selected and
    // deselected appearance, rather than instantly popping to the new state.
    const auto fadeSpeed =
        static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()) * 4.0f;

    if (isSelected) {
        selectionFade_ = std::min(selectionFade_ + fadeSpeed, 1.0f);
    } else {
        selectionFade_ = std::max(selectionFade_ - fadeSpeed, 0.0f);
    }
}

inline void MenuEntry::Draw(MenuScreen& screen, bool isSelected, const GameTime& gameTime)
{
    (void)gameTime;

    // Every entry is drawn alike on a touch menu: there is nothing to highlight.
    isSelected = false;
    const Color tintColor = Color::White;
    const Color textColor = Color::White;

    auto* screenManager = screen.GetScreenManager();
    SpriteBatch& spriteBatch = screenManager->getSpriteBatch();

    spriteBatch.Draw(screenManager->getBlankTexture(), destination_, tintColor);
    spriteBatch.DrawString(screenManager->getFont(), text_, GetTextPosition(screen), textColor,
                           rotation_, Vector2::Zero, scale_, SpriteEffects::None, 0.0f);
}

inline int MenuEntry::GetHeight(MenuScreen& screen) const
{
    return static_cast<int>(screen.GetScreenManager()->getFont().getLineSpacingProperty());
}

inline int MenuEntry::GetWidth(MenuScreen& screen) const
{
    return static_cast<int>(screen.GetScreenManager()->getFont().MeasureString(text_).X);
}

inline Vector2 MenuEntry::GetTextPosition(MenuScreen& screen) const
{
    if (scale_ == 1.0f) {
        return Vector2(static_cast<float>(destination_.X + destination_.Width / 2 -
                                          GetWidth(screen) / 2),
                       static_cast<float>(destination_.Y));
    }

    return Vector2(static_cast<float>(destination_.X) +
                       (static_cast<float>(destination_.Width) / 2 -
                        (static_cast<float>(GetWidth(screen)) / 2) * scale_),
                   static_cast<float>(destination_.Y) +
                       (static_cast<float>(GetHeight(screen)) -
                        static_cast<float>(GetHeight(screen)) * scale_) /
                           2);
}

inline Rectangle MenuScreen::GetMenuEntryHitBounds(const MenuEntry& entry) const
{
    auto* screenManager = GetScreenManager();
    return Rectangle(0, entry.getDestinationProperty().Y - menuEntryPadding_,
                     screenManager->getGraphicsDeviceProperty().getViewportProperty()
                         .getWidthProperty(),
                     const_cast<MenuEntry&>(entry).GetHeight(const_cast<MenuScreen&>(*this)) +
                         menuEntryPadding_ * 2);
}

inline void MenuScreen::LoadContent()
{
    bounds_ = GetScreenManager()->SafeArea();
    Yacht::GameScreen::LoadContent();
}

inline void MenuScreen::UpdateMenuEntryLocations()
{
    // Make the menu slide into place during transitions, using a power curve to make things
    // look more interesting (this makes the movement slow down as it nears the end).
    const auto transitionOffset = static_cast<float>(std::pow(TransitionPosition(), 2));

    auto* screenManager = GetScreenManager();
    if (menuEntries_.empty()) {
        return;
    }

    // Start at Y = 175; each X value is generated per entry.
    Vector2 position(
        0.0f,
        static_cast<float>(
            screenManager->getGameProperty().getWindowProperty().getClientBoundsProperty().Height /
                2 -
            (menuEntries_[0]->GetHeight(*this) +
             menuEntryPadding_ * 2 * static_cast<int>(menuEntries_.size()))));

    // Update each menu entry's location in turn.
    for (auto& menuEntry : menuEntries_) {
        // Each entry is to be centered horizontally.
        position.X = static_cast<float>(
            screenManager->getGraphicsDeviceProperty().getViewportProperty().getWidthProperty() /
                2 -
            menuEntry->GetWidth(*this) / 2);

        if (GetScreenState() == Yacht::ScreenState::TransitionOn) {
            position.X -= transitionOffset * 256;
        } else {
            position.X += transitionOffset * 512;
        }

        // Move down for the next entry the size of this entry.
        position.Y += static_cast<float>(menuEntry->GetHeight(*this) + menuEntryPadding_ * 2);
    }
}

inline void MenuScreen::Draw(const GameTime& gameTime)
{
    auto* screenManager = GetScreenManager();
    auto& graphics = screenManager->getGraphicsDeviceProperty();
    SpriteBatch& spriteBatch = screenManager->getSpriteBatch();
    SpriteFont& font = screenManager->getFont();

    spriteBatch.Begin();

    // Draw each menu entry in turn.
    for (std::size_t i = 0; i < menuEntries_.size(); i++) {
        const bool isSelected = IsActive() && (static_cast<int>(i) == selectedEntry_);
        menuEntries_[i]->Draw(*this, isSelected, gameTime);
    }

    // Make the menu slide into place during transitions, using a power curve to make things
    // look more interesting (this makes the movement slow down as it nears the end).
    const auto transitionOffset = static_cast<float>(std::pow(TransitionPosition(), 2));

    // Draw the menu title centered on the screen.
    Vector2 titlePosition(
        static_cast<float>(graphics.getViewportProperty().getWidthProperty() / 2), 375.0f);
    const Vector2 titleOrigin = font.MeasureString(menuTitle_) / 2;
    const Color titleColor = Color(192, 192, 192) * TransitionAlpha();
    const float titleScale = 1.25f;

    titlePosition.Y -= transitionOffset * 100;

    spriteBatch.DrawString(font, menuTitle_, titlePosition, titleColor, 0.0f, titleOrigin,
                           titleScale, SpriteEffects::None, 0.0f);

    spriteBatch.End();
}

inline void MenuScreen::UpdateMenuEntryDestination()
{
    const Rectangle bounds = GetScreenManager()->SafeArea();

    for (std::size_t i = 0; i < menuEntries_.size(); i++) {
        const int width = menuEntries_[i]->GetWidth(*this) + 20;
        menuEntries_[i]->setDestinationProperty(Rectangle(
            bounds.getCenterProperty().X - width / 2,
            bounds.getCenterProperty().Y +
                (static_cast<int>(i) - static_cast<int>(menuEntries_.size()) / 2) * 100,
            width, 50));
    }
}

} // namespace GameStateManagement
