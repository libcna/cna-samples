// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Controls/Control.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace UserInterfaceSample {

class SingleControlScreen : public GameScreen {
public:
    void Draw(const GameTime& gameTime) override {
        if (rootControl_) {
            Controls::Control::BatchDraw(
                rootControl_.get(),
                GetScreenManager()->getGraphicsDeviceProperty(),
                GetScreenManager()->getSpriteBatchProperty(),
                Microsoft::Xna::Framework::Vector2::Zero,
                gameTime);
        }
        GameScreen::Draw(gameTime);
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        rootControl_->Update(gameTime);
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void HandleInput(InputState& input) override {
        PlayerIndex player;
        if (input.IsNewButtonPress(Microsoft::Xna::Framework::Input::Buttons::Back,
                                   std::nullopt, player)) {
            ExitScreen();
        }
        rootControl_->HandleInput(input);
        GameScreen::HandleInput(input);
    }

    CNAEXT [[nodiscard]] static const std::string& StaticAssemblyQualifiedName() {
        static const std::string name =
            "UserInterfaceSample.SingleControlScreen, AchievementUISample, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
        return name;
    }

    CNAEXT [[nodiscard]] const std::string& GetAssemblyQualifiedName() const override {
        return StaticAssemblyQualifiedName();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.SingleControlScreen";
        return name;
    }

protected:
    std::shared_ptr<Controls::Control> rootControl_;
};

} // namespace UserInterfaceSample
