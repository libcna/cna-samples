// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "System/EventArgs.hpp"

#include "Controls/Button.hpp"
#include "Controls/Container.hpp"
#include "Controls/PhoneScreen.hpp"

namespace DynamicMenuSample
{
    /** @brief Demonstrates the DynamicMenu library on a Windows Phone-style layout. */
    class DynamicMenuSample : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs and configures the sample. */
        DynamicMenuSample();
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Builds the original page controls and initializes the phone screen. */
        void Initialize() override;
        /** @brief Creates the sprite batch and loads the screen's content. */
        void LoadContent() override;
        /** @brief Releases non-ContentManager content; the original has none. */
        void UnloadContent() override;
        /** @brief Processes Back, touch gestures, animations, and control updates. @param gameTime Current game time. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the complete phone-screen control tree. @param gameTime Current game time. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        enum class EDynamicControlPage
        {
            Page1,
            Page2,
            Page3
        };

        void Window_OrientationChanged(System::Object* sender, const System::EventArgs& e);
        void AddPageButton(
            DynamicMenu::Controls::Container& container,
            int left,
            const std::string& text,
            EDynamicControlPage page);
        void ShowPage(EDynamicControlPage page);
        [[nodiscard]] std::shared_ptr<DynamicMenu::Controls::Container> LoadControls(
            const std::string& sampleName);
        void SetupPage3(const std::shared_ptr<DynamicMenu::Controls::Container>& container);
        void pageButton_Tapped(System::Object* sender, const System::EventArgs& e);
        void hueChangeButton_Tapped(System::Object* sender, const System::EventArgs& e);
        void textChangeButton_Tapped(System::Object* sender, const System::EventArgs& e);
        void bouncingButton_Tapped(System::Object* sender, const System::EventArgs& e);
        void getBigButton_Tapped(System::Object* sender, const System::EventArgs& e);
        void getBig_TransitionComplete(System::Object* sender, const System::EventArgs& e);
        void advanceButton_Tapped(System::Object* sender, const System::EventArgs& e);

        Microsoft::Xna::Framework::GraphicsDeviceManager graphics_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        DynamicMenu::Controls::PhoneScreen phoneScreen_;
        std::shared_ptr<DynamicMenu::Controls::Container> dynamicControlsContainer_;
        std::shared_ptr<DynamicMenu::Controls::Container> loadedControlsContainer_;
        std::vector<std::shared_ptr<DynamicMenu::Controls::Button>> pageButtons_;
        std::shared_ptr<DynamicMenu::Controls::Button> hueChangeButton_;
        std::shared_ptr<DynamicMenu::Controls::Button> textChangeButton_;
        std::shared_ptr<DynamicMenu::Controls::Button> bouncingButton_;
        std::shared_ptr<DynamicMenu::Controls::Button> getBigButton_;
        bool bouncingButtonActive_ = false;
        int bouncingButtonChange_ = 200;
        int bouncingButtonLeft_ = 0;
        int bouncingButtonStartLeft_ = 0;
        int textButtonIndex_ = 1;
        std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample> gestureList_;
    };
}
