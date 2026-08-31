// SPDX-License-Identifier: MS-PL
#include "DynamicMenuSample.hpp"

#include <any>
#include <stdexcept>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Int32.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"

#include "Controls/ContentReaders.hpp"
#include "Controls/IControl.hpp"
#include "Controls/ProgressBar.hpp"
#include "Transitions/Transition.hpp"

namespace DynamicMenuSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Input::Touch;
    using DynamicMenu::Controls::Button;
    using DynamicMenu::Controls::Container;
    using DynamicMenu::Controls::IControl;
    using DynamicMenu::Controls::ProgressBar;
    using DynamicMenu::Transitions::Transition;

    DynamicMenuSample::DynamicMenuSample()
        : graphics_(this)
    {
        CNAEXT DynamicMenu::Controls::RegisterDynamicMenuContentReaders();
        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics_.setSupportedOrientationsProperty(
            DisplayOrientation::Portrait |
            DisplayOrientation::LandscapeLeft |
            DisplayOrientation::LandscapeRight);
        graphics_.setPreferredBackBufferWidthProperty(480);
        graphics_.setPreferredBackBufferHeightProperty(800);
        graphics_.setIsFullScreenProperty(true);
        TouchPanel::setEnabledGesturesProperty(GestureType::Tap);
        phoneScreen_.setCurrentOrientationProperty(DisplayOrientation::Portrait);
        getWindowProperty().OrientationChanged +=
            [this](System::Object* sender, const System::EventArgs& e) {
                Window_OrientationChanged(sender, e);
            };
    }

    void DynamicMenuSample::Window_OrientationChanged(System::Object*, const System::EventArgs&)
    {
        phoneScreen_.setCurrentOrientationProperty(
            getWindowProperty().getCurrentOrientationProperty());
    }

    void DynamicMenuSample::Initialize()
    {
        AddPageButton(phoneScreen_.getContainer1Property(), 0, "Page 1", EDynamicControlPage::Page1);
        AddPageButton(phoneScreen_.getContainer1Property(), 135, "Page 2", EDynamicControlPage::Page2);
        AddPageButton(phoneScreen_.getContainer1Property(), 270, "Page 3", EDynamicControlPage::Page3);

        dynamicControlsContainer_ = std::make_shared<Container>();
        dynamicControlsContainer_->setLeftProperty(0);
        dynamicControlsContainer_->setTopProperty(0);
        dynamicControlsContainer_->setWidthProperty(400);
        dynamicControlsContainer_->setHeightProperty(400);

        hueChangeButton_ = std::make_shared<Button>();
        hueChangeButton_->setLeftProperty(100);
        hueChangeButton_->setTopProperty(10);
        hueChangeButton_->setWidthProperty(200);
        hueChangeButton_->setHeightProperty(80);
        hueChangeButton_->setHueProperty(Color::Green);
        hueChangeButton_->setTextProperty("Change Hue");
        hueChangeButton_->setTextColorProperty(Color::White);
        hueChangeButton_->setFontNameProperty("Fonts\\ControlFont");
        hueChangeButton_->setBackTextureNameProperty("Textures\\button");
        hueChangeButton_->setPressedTextureNameProperty("Textures\\buttonpressed");
        hueChangeButton_->Tapped += [this](System::Object* sender, const System::EventArgs& e) {
            hueChangeButton_Tapped(sender, e);
        };
        dynamicControlsContainer_->AddControl(hueChangeButton_);

        textChangeButton_ = std::make_shared<Button>();
        textChangeButton_->setLeftProperty(100);
        textChangeButton_->setTopProperty(100);
        textChangeButton_->setWidthProperty(200);
        textChangeButton_->setHeightProperty(80);
        textChangeButton_->setHueProperty(Color::Red);
        textChangeButton_->setTextProperty("Index: " + System::Int32::ToString(textButtonIndex_));
        textChangeButton_->setTextColorProperty(Color::White);
        textChangeButton_->setFontNameProperty("Fonts\\ControlFont");
        textChangeButton_->setBackTextureNameProperty("Textures\\button");
        textChangeButton_->setPressedTextureNameProperty("Textures\\buttonpressed");
        textChangeButton_->Tapped += [this](System::Object* sender, const System::EventArgs& e) {
            textChangeButton_Tapped(sender, e);
        };
        dynamicControlsContainer_->AddControl(textChangeButton_);

        bouncingButton_ = std::make_shared<Button>();
        bouncingButton_->setLeftProperty(100);
        bouncingButton_->setTopProperty(190);
        bouncingButton_->setWidthProperty(200);
        bouncingButton_->setHeightProperty(80);
        bouncingButton_->setHueProperty(Color::Blue);
        bouncingButton_->setTextProperty("Bounce");
        bouncingButton_->setTextColorProperty(Color::White);
        bouncingButton_->setFontNameProperty("Fonts\\ControlFont");
        bouncingButton_->setBackTextureNameProperty("Textures\\button");
        bouncingButton_->setPressedTextureNameProperty("Textures\\buttonpressed");
        bouncingButton_->Tapped += [this](System::Object* sender, const System::EventArgs& e) {
            bouncingButton_Tapped(sender, e);
        };
        dynamicControlsContainer_->AddControl(bouncingButton_);

        getBigButton_ = std::make_shared<Button>();
        getBigButton_->setLeftProperty(100);
        getBigButton_->setTopProperty(280);
        getBigButton_->setWidthProperty(200);
        getBigButton_->setHeightProperty(80);
        getBigButton_->setHueProperty(Color::Purple);
        getBigButton_->setTextProperty("Get big");
        getBigButton_->setTextColorProperty(Color::White);
        getBigButton_->setFontNameProperty("Fonts\\ControlFont");
        getBigButton_->setBackTextureNameProperty("Textures\\button");
        getBigButton_->setPressedTextureNameProperty("Textures\\buttonpressed");
        getBigButton_->Tapped += [this](System::Object* sender, const System::EventArgs& e) {
            getBigButton_Tapped(sender, e);
        };
        dynamicControlsContainer_->AddControl(getBigButton_);

        phoneScreen_.getContainer2Property().AddControl(dynamicControlsContainer_);
        phoneScreen_.getContainer2Property().setBackTextureNameProperty("Textures\\checkerboard");

        loadedControlsContainer_ = std::make_shared<Container>();
        loadedControlsContainer_->setLeftProperty(0);
        loadedControlsContainer_->setTopProperty(0);
        loadedControlsContainer_->setWidthProperty(400);
        loadedControlsContainer_->setHeightProperty(400);
        phoneScreen_.getContainer2Property().AddControl(loadedControlsContainer_);

        phoneScreen_.Initialize();
        Game::Initialize();
        ShowPage(EDynamicControlPage::Page1);
    }

    void DynamicMenuSample::AddPageButton(
        Container& container, int left, const std::string& text, EDynamicControlPage page)
    {
        auto button = std::make_shared<Button>();
        button->setLeftProperty(left);
        button->setTopProperty(10);
        button->setWidthProperty(130);
        button->setTextProperty(text);
        button->setHeightProperty(80);
        button->setFontNameProperty("Fonts\\ControlFont");
        button->setBackTextureNameProperty("Textures\\button");
        button->setPressedTextureNameProperty("Textures\\buttonpressed");
        button->setTagProperty(page);
        button->Tapped += [this](System::Object* sender, const System::EventArgs& e) {
            pageButton_Tapped(sender, e);
        };
        container.AddControl(button);
        pageButtons_.push_back(button);
    }

    void DynamicMenuSample::ShowPage(EDynamicControlPage page)
    {
        for (const auto& pageButton : pageButtons_)
        {
            const auto buttonPage = std::any_cast<EDynamicControlPage>(pageButton->getTagProperty());
            if (buttonPage == page)
            {
                pageButton->setTextColorProperty(Color::Black);
                pageButton->setHueProperty(Color::Yellow);
            }
            else
            {
                pageButton->setTextColorProperty(Color::LightGray);
                pageButton->setHueProperty(Color::DarkGray);
            }
        }
        dynamicControlsContainer_->setVisibleProperty(false);
        loadedControlsContainer_->setVisibleProperty(false);
        switch (page)
        {
            case EDynamicControlPage::Page1:
                dynamicControlsContainer_->setVisibleProperty(true);
                break;
            case EDynamicControlPage::Page2:
                loadedControlsContainer_->setVisibleProperty(true);
                (void) LoadControls("Menus\\MenuPage2");
                break;
            case EDynamicControlPage::Page3:
            {
                loadedControlsContainer_->setVisibleProperty(true);
                const auto container = LoadControls("Menus\\MenuPage3");
                SetupPage3(container);
                break;
            }
        }
    }

    std::shared_ptr<Container> DynamicMenuSample::LoadControls(const std::string& sampleName)
    {
        loadedControlsContainer_->getControlsProperty().clear();
        const auto loaded = getContentProperty().Load<std::shared_ptr<IControl>>(sampleName);
        const auto loadedContainer = std::dynamic_pointer_cast<Container>(loaded);
        loadedContainer->Initialize();
        loadedContainer->LoadContent(getGraphicsDeviceProperty(), getContentProperty());
        loadedControlsContainer_->AddControl(loadedContainer);
        return loadedContainer;
    }

    void DynamicMenuSample::SetupPage3(const std::shared_ptr<Container>& container)
    {
        auto control = container->FindControlByName("AdvanceButton");
        auto advanceButton = std::dynamic_pointer_cast<Button>(control);
        if (!advanceButton)
            throw std::runtime_error("Failed to find the control named AdvanceButton in MenuPage3.xml");
        control = container->FindControlByName("ProgressBar");
        if (!control)
            throw std::runtime_error("Failed to find the control nameed ProgressBar in MenuPage3.xml");
        advanceButton->setTagProperty(control);
        advanceButton->Tapped += [this](System::Object* sender, const System::EventArgs& e) {
            advanceButton_Tapped(sender, e);
        };
    }

    void DynamicMenuSample::LoadContent()
    {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        phoneScreen_.LoadContent(getGraphicsDeviceProperty(), getContentProperty());
    }

    void DynamicMenuSample::UnloadContent() {}

    void DynamicMenuSample::Update(GameTime& gameTime)
    {
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
            ButtonState::Pressed)
        {
            Exit();
        }
        gestureList_.clear();
        while (TouchPanel::getIsGestureAvailableProperty())
            gestureList_.push_back(TouchPanel::ReadGesture());

        if (bouncingButtonActive_)
        {
            bouncingButtonLeft_ += static_cast<int>(bouncingButtonChange_ *
                gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
            if (bouncingButtonLeft_ + bouncingButton_->getWidthProperty() > 400 &&
                bouncingButtonChange_ > 0)
            {
                bouncingButtonLeft_ = 400 - bouncingButton_->getWidthProperty();
                bouncingButtonChange_ *= -1;
            }
            else if (bouncingButtonChange_ < 0 && bouncingButtonLeft_ < bouncingButtonStartLeft_)
            {
                bouncingButtonLeft_ = bouncingButtonStartLeft_;
                bouncingButtonActive_ = false;
            }
            bouncingButton_->setLeftProperty(bouncingButtonLeft_);
        }

        phoneScreen_.Update(gameTime, gestureList_);
        Game::Update(gameTime);
    }

    void DynamicMenuSample::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        spriteBatch_->Begin();
        phoneScreen_.Draw(gameTime, *spriteBatch_);
        spriteBatch_->End();
        Game::Draw(gameTime);
    }

    void DynamicMenuSample::pageButton_Tapped(System::Object* sender, const System::EventArgs&)
    {
        auto* button = dynamic_cast<Button*>(sender);
        ShowPage(std::any_cast<EDynamicControlPage>(button->getTagProperty()));
    }

    void DynamicMenuSample::hueChangeButton_Tapped(System::Object* sender, const System::EventArgs&)
    {
        auto* button = dynamic_cast<Button*>(sender);
        System::Random random;
        const float r = static_cast<float>(random.NextDouble());
        const float g = static_cast<float>(random.NextDouble());
        const float b = static_cast<float>(random.NextDouble());
        const float a = static_cast<float>(random.NextDouble() * 0.5 + 0.5);
        const Color newColor = Color(r, g, b) * a;
        const Color fontColor(1.0f - r, 1.0f - g, 1.0f - b);
        auto transition = std::make_shared<Transition>(
            std::nullopt, std::nullopt, std::nullopt, std::nullopt,
            button->getHueProperty(), newColor);
        transition->setTransitionLengthProperty(2.0f);
        button->ApplyTransition(transition);
        button->setTextColorProperty(fontColor);
    }

    void DynamicMenuSample::textChangeButton_Tapped(System::Object* sender, const System::EventArgs&)
    {
        auto* button = dynamic_cast<Button*>(sender);
        ++textButtonIndex_;
        button->setTextProperty("Index: " + System::Int32::ToString(textButtonIndex_));
    }

    void DynamicMenuSample::bouncingButton_Tapped(System::Object* sender, const System::EventArgs&)
    {
        auto* button = dynamic_cast<Button*>(sender);
        if (bouncingButtonActive_) return;
        bouncingButtonActive_ = true;
        if (bouncingButtonChange_ < 0) bouncingButtonChange_ *= -1;
        bouncingButtonStartLeft_ = button->getLeftProperty();
        bouncingButtonLeft_ = button->getLeftProperty();
    }

    void DynamicMenuSample::getBigButton_Tapped(System::Object* sender, const System::EventArgs&)
    {
        auto* button = dynamic_cast<Button*>(sender);
        auto transition = std::make_shared<Transition>(
            std::nullopt, Point(0, 240), std::nullopt, Point(400, 160),
            std::nullopt, std::nullopt);
        transition->TransitionComplete +=
            [this](System::Object* eventSender, const System::EventArgs& e) {
                getBig_TransitionComplete(eventSender, e);
            };
        button->ApplyTransition(transition);
    }

    void DynamicMenuSample::getBig_TransitionComplete(System::Object* sender, const System::EventArgs&)
    {
        auto* oldTransition = dynamic_cast<Transition*>(sender);
        auto* button = dynamic_cast<Button*>(oldTransition->getControlProperty());
        auto newTransition = std::make_shared<Transition>(
            std::nullopt, Point(100, 280), std::nullopt, Point(200, 80),
            std::nullopt, std::nullopt);
        button->ApplyTransition(newTransition);
    }

    void DynamicMenuSample::advanceButton_Tapped(System::Object* sender, const System::EventArgs&)
    {
        auto* button = dynamic_cast<Button*>(sender);
        auto progressBar = std::any_cast<std::shared_ptr<IControl>>(button->getTagProperty());
        auto concreteProgress = std::dynamic_pointer_cast<ProgressBar>(progressBar);
        int curProgress = concreteProgress->getPositionProperty() + 10;
        if (curProgress > concreteProgress->getMaxValueProperty()) curProgress = 0;
        concreteProgress->setPositionProperty(curProgress);
    }

    const std::string& DynamicMenuSample::GetTypeName() const
    {
        static const std::string name = "DynamicMenuSample.DynamicMenuSample";
        return name;
    }
}
