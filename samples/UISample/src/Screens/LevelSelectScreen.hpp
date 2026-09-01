// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Controls/ImageControl.hpp"
#include "Controls/PageFlipControl.hpp"
#include "Controls/PageFlipTracker.hpp"
#include "Controls/PanelControl.hpp"
#include "Controls/TextControl.hpp"
#include "Screens/SingleControlScreen.hpp"

namespace UserInterfaceSample {

class LevelInfo {
public:
    std::string Name;
    std::string Description;
    std::string Image;
};

class LevelDescriptionPanel final : public Controls::PanelControl {
public:
    LevelDescriptionPanel(Microsoft::Xna::Framework::Content::ContentManager& content,
                          const LevelInfo& info) {
        using Microsoft::Xna::Framework::Color;
        using Microsoft::Xna::Framework::Vector2;
        using Microsoft::Xna::Framework::Graphics::SpriteFont;
        using Microsoft::Xna::Framework::Graphics::Texture2D;

        backgroundTexture_.emplace(content.Load<Texture2D>(info.Image));
        AddChild(std::make_shared<Controls::ImageControl>(&*backgroundTexture_, Vector2::Zero));

        titleFont_.emplace(content.Load<SpriteFont>("Font\\MenuTitle"));
        AddChild(std::make_shared<Controls::TextControl>(
            info.Name, &*titleFont_, Color::Black, Vector2(MarginLeft, MarginTop)));

        descriptionFont_.emplace(content.Load<SpriteFont>("Font\\MenuDetail"));
        AddChild(std::make_shared<Controls::TextControl>(
            info.Description, &*descriptionFont_, Color::Black,
            Vector2(MarginLeft, DescriptionTop)));
    }

private:
    static constexpr float MarginLeft = 20.0f;
    static constexpr float MarginTop = 20.0f;
    static constexpr float DescriptionTop = 440.0f;

    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> backgroundTexture_;
    std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> titleFont_;
    std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> descriptionFont_;
};

class LevelSelectScreen final : public SingleControlScreen {
public:
    void LoadContent() override {
        setEnabledGestures(Controls::PageFlipTracker::GesturesNeeded);
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        auto pageFlip = std::make_shared<Controls::PageFlipControl>();

        for (const LevelInfo& info : levelInfos_)
            pageFlip->AddChild(std::make_shared<LevelDescriptionPanel>(content, info));

        rootControl_ = std::move(pageFlip);
    }

    CNAEXT [[nodiscard]] static const std::string& StaticAssemblyQualifiedName() {
        static const std::string name =
            "UserInterfaceSample.LevelSelectScreen, AchievementUISample, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
        return name;
    }

    CNAEXT [[nodiscard]] const std::string& GetAssemblyQualifiedName() const override {
        return StaticAssemblyQualifiedName();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.LevelSelectScreen";
        return name;
    }

private:
    std::array<LevelInfo, 5> levelInfos_{{
        {"House", "Find a way out of your house--if you dare!", "Levels\\House"},
        {"Pasture", "Locate your magical cow", "Levels\\Pasture"},
        {"Hills", "Graze across the hills", "Levels\\Hills"},
        {"Castle", "Explore the old ruined castle", "Levels\\Castle"},
        {"Dungeon", "Conquer the dreaded Dungeon Critter", "Levels\\Dungeon"},
    }};
};

} // namespace UserInterfaceSample
