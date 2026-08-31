// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "TextControl.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Touch button that raises Tapped after a short pressed interval. */
    class Button : public TextControl
    {
    public:
        /** @brief Indicates that the button was tapped. */
        System::EventHandler<System::EventArgs> Tapped;

        /** @brief Gets the pressed texture asset name. @return Asset name. */
        [[nodiscard]] const std::string& getPressedTextureNameProperty() const;
        /** @brief Sets the pressed texture asset name. @param value Asset name. */
        void setPressedTextureNameProperty(std::string value);
        /** @brief Gets the pressed texture. @return Texture or null. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* getPressedTextureProperty();
        /** @brief Gets whether the button is pressed. @return Pressed state. */
        [[nodiscard]] bool getPressedProperty() const;
        /** @brief Sets whether the button is pressed. @param value Pressed state. */
        void setPressedProperty(bool value);

        /** @brief Loads the normal, pressed, and text content. @param graphics Graphics device. @param content Content manager. */
        void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content) override;
        /** @brief Processes transitions and tap gestures. @param gameTime Current game time. @param gestures Gestures for the frame. */
        void Update(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            const std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample>& gestures) override;
        /** @brief Draws the button texture and centered text. @param gameTime Current game time. @param spriteBatch Sprite batch. */
        void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) override;
        /** @brief Gets the normal or pressed texture for the current state. @return Current texture or null. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* GetCurrTexture() override;
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Called immediately when a valid tap is detected. */
        virtual void HandlePressed();

    private:
        static constexpr double PressTimeSeconds = 0.2;
        double pressStartTime_ = 0.0;
        std::string pressedTextureName_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> pressedTexture_;
        bool pressed_ = false;
    };
}
