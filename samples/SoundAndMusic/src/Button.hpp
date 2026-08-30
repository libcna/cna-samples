// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class SpriteBatch;
    class Texture2D;
}

namespace SoundAndMusicSample
{
    /**
     * @brief Reusable touch button with press, click, and optional drag behavior.
     */
    class Button : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /** @brief Raised when the button is released after being touched. */
        System::EventHandler<System::EventArgs> Click;

        /** @brief Raised when a touch first enters the button. */
        System::EventHandler<System::EventArgs> TouchDown;

        /** @brief Raised when PositionOfOrigin changes. */
        System::EventHandler<System::EventArgs> PositionChanged;

        /**
         * @brief Constructs a button that draws the specified texture asset.
         * @param textureName Content asset name of the button texture.
         * @param game Game that owns this component.
         */
        Button(const std::string& textureName, Microsoft::Xna::Framework::Game& game);

        /** @brief Destructor. */
        ~Button() override;

        /**
         * @brief Gets the bounds restricting drag movement.
         * @return The drag restriction rectangle.
         */
        [[nodiscard]] const Microsoft::Xna::Framework::Rectangle& getDragRestrictionsProperty() const;

        /**
         * @brief Sets the bounds restricting drag movement.
         * @param value The drag restriction rectangle.
         */
        void setDragRestrictionsProperty(const Microsoft::Xna::Framework::Rectangle& value);

        /**
         * @brief Gets the screen location at which the texture origin is drawn.
         * @return The origin's screen position.
         */
        [[nodiscard]] const Microsoft::Xna::Framework::Vector2& getPositionOfOriginProperty() const;

        /**
         * @brief Sets the screen location at which the texture origin is drawn.
         * @param value The origin's screen position.
         */
        void setPositionOfOriginProperty(const Microsoft::Xna::Framework::Vector2& value);

        /**
         * @brief Gets the origin coordinates within the texture.
         * @return The texture-space origin.
         */
        [[nodiscard]] const Microsoft::Xna::Framework::Vector2& getPositionOriginProperty() const;

        /**
         * @brief Sets the origin coordinates within the texture.
         * @param value The texture-space origin.
         */
        void setPositionOriginProperty(const Microsoft::Xna::Framework::Vector2& value);

        /**
         * @brief Gets the top-left position used to draw the texture.
         * @return The texture's draw position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionForDrawProperty() const;

        /**
         * @brief Gets the top-left screen position of the button.
         * @return The button's top-left position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getTopLeftPositionProperty() const;

        /**
         * @brief Sets the top-left screen position of the button.
         * @param value The button's top-left position.
         */
        void setTopLeftPositionProperty(const Microsoft::Xna::Framework::Vector2& value);

        /**
         * @brief Gets the tint for the current enabled and touched state.
         * @return The tint to use while drawing.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Color getTintColorProperty() const;

        /**
         * @brief Sets the normal-state tint.
         * @param value The normal-state tint.
         */
        void setTintColorProperty(const Microsoft::Xna::Framework::Color& value);

        /**
         * @brief Gets the center of the button texture.
         * @return The texture center in pixels.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getTextureCenterProperty() const;

        /**
         * @brief Gets the button's screen bounds.
         * @return The button bounds in screen coordinates.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle getScreenBoundsProperty() const;

        /**
         * @brief Gets the optional tint used while touched.
         * @return The touched tint, or no value to use the normal tint.
         */
        [[nodiscard]] const std::optional<Microsoft::Xna::Framework::Color>&
        getTintWhenTouchedProperty() const;

        /**
         * @brief Sets the optional tint used while touched.
         * @param value The touched tint, or no value to use the normal tint.
         */
        void setTintWhenTouchedProperty(
            const std::optional<Microsoft::Xna::Framework::Color>& value);

        /**
         * @brief Gets the optional tint used while disabled.
         * @return The disabled tint, or no value to use the normal tint.
         */
        [[nodiscard]] const std::optional<Microsoft::Xna::Framework::Color>&
        getTintWhenDisabledProperty() const;

        /**
         * @brief Sets the optional tint used while disabled.
         * @param value The disabled tint, or no value to use the normal tint.
         */
        void setTintWhenDisabledProperty(
            const std::optional<Microsoft::Xna::Framework::Color>& value);

        /**
         * @brief Gets whether the button may be dragged.
         * @return True when dragging is enabled.
         */
        [[nodiscard]] bool getAllowDragProperty() const;

        /**
         * @brief Sets whether the button may be dragged.
         * @param value True to enable dragging.
         */
        void setAllowDragProperty(bool value);

        /** @brief Initializes the component and centers its texture origin. */
        void Initialize() override;

        /**
         * @brief Updates touch, release, and drag state.
         * @param gameTime Current game timing snapshot.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the button.
         * @param gameTime Current game timing snapshot.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Returns the fully-qualified logical type name.
         * @return The type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /**
         * @brief Raises the Click event.
         * @param e Event data.
         */
        virtual void OnClick(const System::EventArgs& e);

        /**
         * @brief Raises the TouchDown event.
         * @param e Event data.
         */
        virtual void OnTouchDown(const System::EventArgs& e);

        /**
         * @brief Raises the PositionChanged event.
         * @param e Event data.
         */
        virtual void OnPositionChanged(const System::EventArgs& e);

        /** @brief Loads the button texture and sprite batch. */
        void LoadContent() override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        bool isTouched_;
        bool dragStarted_;
        std::string assetName_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
        Microsoft::Xna::Framework::Rectangle dragRestrictions_;

    protected:
        Microsoft::Xna::Framework::Vector2 positionOfOrigin_;

    private:
        Microsoft::Xna::Framework::Vector2 positionOrigin_;

    protected:
        Microsoft::Xna::Framework::Color tintColor_;

    private:
        std::optional<Microsoft::Xna::Framework::Color> tintWhenTouched_;
        std::optional<Microsoft::Xna::Framework::Color> tintWhenDisabled_;
        bool allowDrag_;

        [[nodiscard]] std::optional<Microsoft::Xna::Framework::Rectangle> GetTouchRect() const;
        [[nodiscard]] bool CheckIfFirstTouchDown(
            const std::optional<Microsoft::Xna::Framework::Rectangle>& touchRect) const;
        [[nodiscard]] bool CheckIfStillTouching(
            const std::optional<Microsoft::Xna::Framework::Rectangle>& touchRect) const;
        [[nodiscard]] bool CheckIfTouchRelease(
            const std::optional<Microsoft::Xna::Framework::Rectangle>& touchRect) const;
        void DoOnTouchDown();
        void DoOnNotTouching();
        void DoOnTouchRelease();
        void DoOnDrag(const Microsoft::Xna::Framework::Rectangle& touchRect);
    };
}
