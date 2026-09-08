#pragma once

// Button.hpp -- C++ port of Yacht/Misc/Button.cs.

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;

/**
 * @brief A tappable image, optionally with a caption centred on it.
 */
class Button : public System::Object {
public:
    /** @brief Where on screen the button is, and how big. */
    Rectangle Position;

    /** @brief The button's image. */
    Texture2D* Texture = nullptr;

    /** @brief Whether the button responds to taps; a disabled button draws grey. */
    bool Enabled = true;

    /** @brief Raised when the button is tapped. */
    System::EventHandler<System::EventArgs> Click;

    /**
     * @brief Creates a button the size of its texture.
     *
     * @param texture  The button's image.
     * @param position The button's top-left corner.
     * @param font     The font for the caption, or null for no caption.
     * @param text     The caption, or empty for no caption.
     */
    Button(Texture2D* texture, const Vector2& position, SpriteFont* font, std::string text)
        : Button(texture,
                 Rectangle(static_cast<int>(position.X), static_cast<int>(position.Y),
                           texture->getWidthProperty(), texture->getHeightProperty()),
                 font, std::move(text))
    {
    }

    /**
     * @brief Creates a button of a given size.
     *
     * @param texture  The button's image.
     * @param position The button's bounds.
     * @param font     The font for the caption, or null for no caption.
     * @param text     The caption, or empty for no caption.
     */
    Button(Texture2D* texture, const Rectangle& position, SpriteFont* font, std::string text)
        : Position(position), Texture(texture), font_(font), text_(std::move(text))
    {
    }

    /** @brief The type's name. @return "Button". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "Button";
        return name;
    }

    /**
     * @brief Raises Click when the gesture taps this button.
     *
     * @param sample The gesture to handle.
     * @return True when the tap was this button's, so the caller stops looking.
     */
    bool HandleInput(const GestureSample& sample)
    {
        if (Enabled && sample.getGestureTypeProperty() == GestureType::Tap) {
            // Create the touch rectangle
            const Rectangle touchRect(static_cast<int>(sample.getPositionProperty().X) - 1,
                                      static_cast<int>(sample.getPositionProperty().Y) - 1, 2, 2);

            // Create the button bounds rectangle
            Rectangle bounds = Texture->getBoundsProperty();
            bounds.X += Position.X;
            bounds.Y += Position.Y;

            // Check for intersection between the rectangles
            if (bounds.Intersects(touchRect)) {
                System::EventArgs empty;
                Click.Raise(this, empty);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Draws the button and its caption.
     *
     * @param spriteBatch The batch to draw into; must already be begun.
     */
    void Draw(SpriteBatch& spriteBatch) const
    {
        spriteBatch.Draw(*Texture, Position, Enabled ? Color::White : Color::Gray);

        if (!text_.empty() && font_ != nullptr) {
            DrawString(spriteBatch);
        }
    }

private:
    void DrawString(SpriteBatch& spriteBatch) const
    {
        const Vector2 textSize = font_->MeasureString(text_);
        spriteBatch.DrawString(
            *font_, text_,
            Vector2(static_cast<float>(Position.X) + static_cast<float>(Position.Width) / 2 -
                        textSize.X / 2,
                    static_cast<float>(Position.Y) + static_cast<float>(Position.Height) / 2 -
                        textSize.Y / 2 - 5),
            Color::White);
    }

    SpriteFont* font_ = nullptr;
    std::string text_;
};

} // namespace Yacht
