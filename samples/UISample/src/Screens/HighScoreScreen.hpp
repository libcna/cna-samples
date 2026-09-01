// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Controls/HighScorePanel.hpp"
#include "Controls/ScrollTracker.hpp"
#include "Screens/SingleControlScreen.hpp"

namespace UserInterfaceSample {

class HighScoreScreen final : public SingleControlScreen {
public:
    void LoadContent() override {
        setEnabledGestures(Controls::ScrollTracker::GesturesNeeded);
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        rootControl_ = std::make_shared<Controls::HighScorePanel>(content);
        SingleControlScreen::LoadContent();
    }

    CNAEXT [[nodiscard]] static const std::string& StaticAssemblyQualifiedName() {
        static const std::string name =
            "UserInterfaceSample.HighScoreScreen, AchievementUISample, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
        return name;
    }

    CNAEXT [[nodiscard]] const std::string& GetAssemblyQualifiedName() const override {
        return StaticAssemblyQualifiedName();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.HighScoreScreen";
        return name;
    }
};

} // namespace UserInterfaceSample
