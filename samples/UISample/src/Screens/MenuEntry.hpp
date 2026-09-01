// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"

#include "Screens/PlayerIndexEventArgs.hpp"

namespace UserInterfaceSample {

using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

class MenuScreen; // forward declaration (defined in MenuScreen.hpp)

// Represents a single entry in a MenuScreen. By default this just draws the
// entry text string, and provides an event raised when selected. Port of
// Screens/MenuEntry.cs.
class MenuEntry : public System::Object {
public:
    System::EventHandler<PlayerIndexEventArgs> Selected;

    explicit MenuEntry(std::string text) : text_(std::move(text)) {}
    ~MenuEntry() override = default;

    const std::string& Text() const { return text_; }
    void setText(std::string value) { text_ = std::move(value); }

    Vector2 Position() const { return position_; }
    void setPosition(Vector2 value) { position_ = value; }

    // Raises the Selected event.
    void OnSelectEntry(PlayerIndex playerIndex) {
        Selected.Raise(this, PlayerIndexEventArgs(playerIndex));
    }

    // Updates the menu entry's fading selection effect.
    virtual void Update(MenuScreen& screen, bool isSelected, const GameTime& gameTime) {
        (void)screen;
#if defined(WINDOWS_PHONE)
        isSelected = false;
#endif

        float fadeSpeed = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 4.0f;
        if (isSelected)
            selectionFade_ = std::min(selectionFade_ + fadeSpeed, 1.0f);
        else
            selectionFade_ = std::max(selectionFade_ - fadeSpeed, 0.0f);
    }

    // Draws the menu entry; can be overridden to customize the appearance.
    virtual void Draw(MenuScreen& screen, bool isSelected, const GameTime& gameTime);

    // Queries how much space this menu entry requires.
    virtual int GetHeight(MenuScreen& screen);

    // Queries how wide the entry is, used for centering on the screen.
    virtual int GetWidth(MenuScreen& screen);

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.MenuEntry";
        return name;
    }

private:
    std::string text_;
    float selectionFade_ = 0.0f;
    Vector2 position_;
};

} // namespace UserInterfaceSample
