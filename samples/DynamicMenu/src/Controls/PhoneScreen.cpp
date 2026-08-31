// SPDX-License-Identifier: MS-PL
#include "PhoneScreen.hpp"

namespace DynamicMenu::Controls
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;

    Container& PhoneScreen::getContainer1Property() { return container1_; }
    Container& PhoneScreen::getContainer2Property() { return container2_; }
    DisplayOrientation PhoneScreen::getCurrentOrientationProperty() const { return currentOrientation_; }
    void PhoneScreen::setCurrentOrientationProperty(DisplayOrientation value)
    {
        currentOrientation_ = value;
        UpdateOrientation();
    }

    void PhoneScreen::Initialize()
    {
        Control::Initialize();
        container1_.Initialize();
        container2_.Initialize();
        UpdateOrientation();
    }

    void PhoneScreen::LoadContent(GraphicsDevice& graphics, ContentManager& content)
    {
        Control::LoadContent(graphics, content);
        container1_.LoadContent(graphics, content);
        container2_.LoadContent(graphics, content);
    }

    void PhoneScreen::Update(const GameTime& gameTime, const std::vector<GestureSample>& gestures)
    {
        Control::Update(gameTime, gestures);
        if (container1_.getVisibleProperty()) container1_.Update(gameTime, gestures);
        if (container2_.getVisibleProperty()) container2_.Update(gameTime, gestures);
    }

    void PhoneScreen::Draw(const GameTime& gameTime, SpriteBatch& spriteBatch)
    {
        Control::Draw(gameTime, spriteBatch);
        if (container1_.getVisibleProperty()) container1_.Draw(gameTime, spriteBatch);
        if (container2_.getVisibleProperty()) container2_.Draw(gameTime, spriteBatch);
    }

    void PhoneScreen::UpdateOrientation()
    {
        container1_.setWidthProperty(ContainerWidth);
        container1_.setHeightProperty(ContainerHeight);
        container2_.setWidthProperty(ContainerWidth);
        container2_.setHeightProperty(ContainerHeight);
        switch (currentOrientation_)
        {
            case DisplayOrientation::Portrait:
                container1_.setLeftProperty(VerticalContainer1Left);
                container1_.setTopProperty(VerticalContainer1Top);
                container2_.setLeftProperty(VerticalContainer2Left);
                container2_.setTopProperty(VerticalContainer2Top);
                break;
            case DisplayOrientation::LandscapeLeft:
            case DisplayOrientation::LandscapeRight:
                container1_.setLeftProperty(HorizontalContainer1Left);
                container1_.setTopProperty(HorizontalContainer1Top);
                container2_.setLeftProperty(HorizontalContainer2Left);
                container2_.setTopProperty(HorizontalContainer2Top);
                break;
            default:
                break;
        }
    }

    const std::string& PhoneScreen::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.PhoneScreen";
        return name;
    }
}
