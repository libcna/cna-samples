// SPDX-License-Identifier: MS-PL
#include "Button.hpp"

#include <utility>

namespace DynamicMenu::Controls
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input::Touch;

    const std::string& Button::getPressedTextureNameProperty() const { return pressedTextureName_; }
    void Button::setPressedTextureNameProperty(std::string value) { pressedTextureName_ = std::move(value); }
    Texture2D* Button::getPressedTextureProperty() { return pressedTexture_ ? &*pressedTexture_ : nullptr; }
    bool Button::getPressedProperty() const { return pressed_; }
    void Button::setPressedProperty(bool value) { pressed_ = value; }

    void Button::LoadContent(GraphicsDevice& graphics, ContentManager& content)
    {
        TextControl::LoadContent(graphics, content);
        if (!pressedTextureName_.empty())
            pressedTexture_.emplace(content.Load<Texture2D>(pressedTextureName_));
    }

    void Button::Update(const GameTime& gameTime, const std::vector<GestureSample>& gestures)
    {
        TextControl::Update(gameTime, gestures);
        for (const GestureSample& sample : gestures)
        {
            if (sample.getGestureTypeProperty() != GestureType::Tap) continue;
            if (ContainsPos(sample.getPositionProperty()))
            {
                pressed_ = true;
                pressStartTime_ = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();
                HandlePressed();
                break;
            }
        }
        if (pressed_ && gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() >
                            pressStartTime_ + PressTimeSeconds)
        {
            pressed_ = false;
            Tapped.Raise(this, System::EventArgs::Empty);
        }
    }

    void Button::Draw(const GameTime& gameTime, SpriteBatch& spriteBatch)
    {
        TextControl::Draw(gameTime, spriteBatch);
        DrawCenteredText(spriteBatch, getFontProperty(), GetAbsoluteRect(),
                         getTextProperty(), getTextColorProperty());
    }

    Texture2D* Button::GetCurrTexture()
    {
        if (pressed_ && pressedTexture_) return &*pressedTexture_;
        return TextControl::GetCurrTexture();
    }

    void Button::HandlePressed() {}

    const std::string& Button::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.Button";
        return name;
    }
}
