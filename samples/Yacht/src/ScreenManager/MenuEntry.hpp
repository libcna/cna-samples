#pragma once

// MenuEntry.hpp -- C++ port of Yacht/ScreenManager/MenuEntry.cs.

#include <algorithm>
#include <string>
#include <utility>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"

#include "PlayerIndexEventArgs.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

class MenuScreen;

/**
 * @brief Helper class represents a single entry in a MenuScreen.
 *
 * By default this just draws the entry text string, but it can be customized to display menu
 * entries in different ways. This also provides an event that will be raised when the menu
 * entry is selected.
 *
 * @note **The selection highlight is deliberately dead on a phone.** The original fades an
 * entry in as it is selected, which is what a menu driven by a d-pad needs; a touch menu has no
 * "selected" entry to fade, so the phone build forces isSelected to false in both Update and
 * Draw and paints every entry alike. The fade is kept because the class keeps it -- what is
 * gone is any way to see it, and removing it would be removing a feature rather than porting
 * the one the sample ships.
 */
class MenuEntry : public System::Object {
public:
    /**
     * @brief Constructs a new menu entry with the specified text.
     *
     * @param text The text to display.
     */
    explicit MenuEntry(std::string text) : text_(std::move(text)) {}

    /** @brief The type's name. @return "MenuEntry". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "MenuEntry";
        return name;
    }

    /** @brief Destroys the entry. */
    ~MenuEntry() override = default;

    /**
     * @brief Gets the text rendered for this entry.
     *
     * @return The text.
     */
    [[nodiscard]] const std::string& getTextProperty() const { return text_; }

    /**
     * @brief Sets the text rendered for this entry.
     *
     * @param value The text.
     */
    void setTextProperty(std::string value) { text_ = std::move(value); }

    /**
     * @brief Gets the area where the entry is drawn.
     *
     * @return The destination rectangle.
     */
    [[nodiscard]] const Rectangle& getDestinationProperty() const { return destination_; }

    /**
     * @brief Sets the area where the entry is drawn.
     *
     * @param value The destination rectangle.
     */
    void setDestinationProperty(const Rectangle& value) { destination_ = value; }

    /**
     * @brief Gets the scale the entry's text is drawn at.
     *
     * @return The scale, one being unscaled.
     */
    [[nodiscard]] float getScaleProperty() const { return scale_; }

    /**
     * @brief Sets the scale the entry's text is drawn at.
     *
     * @param value The scale.
     */
    void setScaleProperty(float value) { scale_ = value; }

    /**
     * @brief Gets the rotation the entry's text is drawn at.
     *
     * @return The rotation in radians.
     */
    [[nodiscard]] float getRotationProperty() const { return rotation_; }

    /**
     * @brief Sets the rotation the entry's text is drawn at.
     *
     * @param value The rotation in radians.
     */
    void setRotationProperty(float value) { rotation_ = value; }

    /** @brief Event raised when the menu entry is selected. */
    System::EventHandler<PlayerIndexEventArgs> Selected;

    /**
     * @brief Method for raising the Selected event.
     *
     * @param playerIndex The player who selected the entry.
     */
    virtual void OnSelectEntry(PlayerIndex playerIndex)
    {
        PlayerIndexEventArgs args(playerIndex);
        Selected.Raise(this, args);
    }

    /**
     * @brief Updates the menu entry.
     *
     * @param screen     The screen the entry belongs to.
     * @param isSelected Whether the entry is the selected one.
     * @param gameTime   The elapsed time.
     */
    virtual void Update(MenuScreen& screen, bool isSelected, const GameTime& gameTime);

    /**
     * @brief Draws the menu entry.
     *
     * @param screen     The screen the entry belongs to.
     * @param isSelected Whether the entry is the selected one.
     * @param gameTime   The elapsed time.
     */
    virtual void Draw(MenuScreen& screen, bool isSelected, const GameTime& gameTime);

    /**
     * @brief Queries how much space this menu entry requires.
     *
     * @param screen The screen the entry belongs to.
     * @return The height in pixels.
     */
    [[nodiscard]] virtual int GetHeight(MenuScreen& screen) const;

    /**
     * @brief Queries how wide the entry is, used for centering on the screen.
     *
     * @param screen The screen the entry belongs to.
     * @return The width in pixels.
     */
    [[nodiscard]] virtual int GetWidth(MenuScreen& screen) const;

protected:
    /** @brief How far the entry has faded in as it is selected, from zero to one. */
    float selectionFade_ = 0.0f;

private:
    [[nodiscard]] Vector2 GetTextPosition(MenuScreen& screen) const;

    std::string text_;
    Rectangle destination_;
    float scale_ = 1.0f;
    float rotation_ = 0.0f;
};

} // namespace GameStateManagement
