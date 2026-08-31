// SPDX-License-Identifier: MS-PL
#pragma once

#include "Container.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Two-container phone layout supporting portrait and landscape orientation. */
    class PhoneScreen : public Control
    {
    public:
        /** @brief Gets the first container. @return Container reference. */
        [[nodiscard]] Container& getContainer1Property();
        /** @brief Gets the second container. @return Container reference. */
        [[nodiscard]] Container& getContainer2Property();
        /** @brief Gets current orientation. @return Orientation. */
        [[nodiscard]] Microsoft::Xna::Framework::DisplayOrientation getCurrentOrientationProperty() const;
        /** @brief Sets current orientation and updates layout. @param value Orientation. */
        void setCurrentOrientationProperty(Microsoft::Xna::Framework::DisplayOrientation value);
        /** @brief Initializes both containers and applies the current orientation. */
        void Initialize() override;
        /** @brief Loads this screen and both containers. @param graphics Graphics device. @param content Content manager. */
        void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content) override;
        /** @brief Updates visible containers. @param gameTime Current game time. @param gestures Gestures for the frame. */
        void Update(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            const std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample>& gestures) override;
        /** @brief Draws visible containers. @param gameTime Current game time. @param spriteBatch Sprite batch. */
        void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) override;
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        void UpdateOrientation();

        static constexpr int ContainerWidth = 400;
        static constexpr int ContainerHeight = 400;
        static constexpr int VerticalContainer1Left = 40;
        static constexpr int VerticalContainer1Top = 0;
        static constexpr int VerticalContainer2Left = 40;
        static constexpr int VerticalContainer2Top = 400;
        static constexpr int HorizontalContainer1Left = 0;
        static constexpr int HorizontalContainer1Top = 40;
        static constexpr int HorizontalContainer2Left = 400;
        static constexpr int HorizontalContainer2Top = 40;

        Container container1_;
        Container container2_;
        Microsoft::Xna::Framework::DisplayOrientation currentOrientation_ =
            Microsoft::Xna::Framework::DisplayOrientation::Portrait;
    };
}
