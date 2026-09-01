// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "System/Object.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/HighScoreScreen.hpp"
#include "Screens/LevelSelectScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MenuEntry.hpp"
#include "Screens/MenuScreen.hpp"

namespace UserInterfaceSample {

class MainMenuScreen final : public MenuScreen {
public:
    MainMenuScreen() : MenuScreen("Main Menu") {
        auto levelSelect = std::make_shared<MenuEntry>("Select level");
        levelSelect->Selected += [this](System::Object* sender,
                                        const PlayerIndexEventArgs& e) {
            SelectLevelPressed(sender, e);
        };
        MenuEntries().push_back(std::move(levelSelect));

        auto highScores = std::make_shared<MenuEntry>("High scores");
        highScores->Selected += [this](System::Object* sender,
                                       const PlayerIndexEventArgs& e) {
            HighScoresPressed(sender, e);
        };
        MenuEntries().push_back(std::move(highScores));
    }

    CNAEXT [[nodiscard]] static const std::string& StaticAssemblyQualifiedName() {
        static const std::string name =
            "UserInterfaceSample.MainMenuScreen, AchievementUISample, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
        return name;
    }

    CNAEXT [[nodiscard]] const std::string& GetAssemblyQualifiedName() const override {
        return StaticAssemblyQualifiedName();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.MainMenuScreen";
        return name;
    }

protected:
    void OnCancel(PlayerIndex playerIndex) override {
        (void)playerIndex;
        GetScreenManager()->getGameProperty().Exit();
    }

private:
    void SelectLevelPressed(System::Object* sender, const PlayerIndexEventArgs& e) {
        (void)sender;
        std::vector<std::shared_ptr<GameScreen>> screens;
        screens.push_back(std::make_shared<BackgroundScreen>());
        screens.push_back(std::make_shared<MainMenuScreen>());
        screens.push_back(std::make_shared<LevelSelectScreen>());
        LoadingScreen::Load(*GetScreenManager(), true, e.getPlayerIndexProperty(),
                            std::move(screens));
    }

    void HighScoresPressed(System::Object* sender, const PlayerIndexEventArgs& e) {
        (void)sender;
        GetScreenManager()->AddScreen(std::make_shared<HighScoreScreen>(),
                                      e.getPlayerIndexProperty());
    }
};

} // namespace UserInterfaceSample
