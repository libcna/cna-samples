#pragma once

// LevelUpScreen.hpp -- C++ port of GameScreens/LevelUpScreen.cs.

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../AudioManager.hpp"
#include "../Data/Characters/CharacterLevelingStatistics.hpp"
#include "../Data/Characters/Player.hpp"
#include "../Data/Spell.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::CharacterLevelingStatistics;
using RolePlayingGameData::Player;
using RolePlayingGameData::Spell;

// Displays all the players that have leveled up.
class LevelUpScreen : public GameScreen {
public:
    // Constructs a new LevelUpScreen object.
    explicit LevelUpScreen(std::vector<std::shared_ptr<Player>> leveledUpPlayers)
        : leveledUpPlayers_(std::move(leveledUpPlayers)) {
        if (leveledUpPlayers_.empty()) {
            throw System::ArgumentNullException("leveledUpPlayers");
        }

        SetIsPopup(true);

        index_ = 0;

        GetSpellList();

        AudioManager::PushMusic("LevelUp");
        Exiting += [](System::Object*, const System::EventArgs&) { AudioManager::PopMusic(); };
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        backTexture_ = content.Load<Texture2D>("Textures/GameScreens/PopupScreen");
        selectIconTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        portraitBackTexture_ = content.Load<Texture2D>("Textures/GameScreens/PlayerSelected");
        headerTexture_ = content.Load<Texture2D>("Textures/GameScreens/Caption");
        lineTexture_ = content.Load<Texture2D>("Textures/GameScreens/SeparationLine");
        scrollUpTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollUp");
        scrollDownTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollDown");
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        backgroundPosition_.X =
            (float)((viewport.getWidthProperty() - backTexture_.getWidthProperty()) / 2);
        backgroundPosition_.Y =
            (float)((viewport.getHeightProperty() - backTexture_.getHeightProperty()) / 2);

        screenSize_ =
            Vector2((float)viewport.getWidthProperty(), (float)viewport.getHeightProperty());
        fadeDest_ = Rectangle(0, 0, viewport.getWidthProperty(), viewport.getHeightProperty());

        titlePosition_.X =
            (screenSize_.X - Fonts::HeaderFont().MeasureString(titleText_).X) / 2.0f;
        titlePosition_.Y = backgroundPosition_.Y + (float)LineSpacing;

        selectIconPosition_.X = screenSize_.X / 2.0f + 260.0f;
        selectIconPosition_.Y = backgroundPosition_.Y + 530.0f;
        selectPosition_.X = selectIconPosition_.X -
                            Fonts::ButtonNamesFont().MeasureString(selectString_).X - 10.0f;
        selectPosition_.Y = selectIconPosition_.Y;

        portraitPosition_ = backgroundPosition_ + Vector2(143.0f, 155.0f);
        backPosition_ = backgroundPosition_ + Vector2(140.0f, 135.0f);

        playerNamePosition_ = backgroundPosition_ + Vector2(230.0f, 160.0f);
        playerClassPosition_ = backgroundPosition_ + Vector2(230.0f, 185.0f);
        playerLvlPosition_ = backgroundPosition_ + Vector2(230.0f, 205.0f);

        topLinePosition_ = backgroundPosition_ + Vector2(380.0f, 160.0f);
        textPosition_ = backgroundPosition_ + Vector2(335.0f, 320.0f);
        levelPosition_ = backgroundPosition_ + Vector2(540.0f, 320.0f);
        iconPosition_ = backgroundPosition_ + Vector2(155.0f, 303.0f);
        linePosition_ = backgroundPosition_ + Vector2(142.0f, 285.0f);

        scrollUpPosition_ = backgroundPosition_ + Vector2(810.0f, 300.0f);
        scrollDownPosition_ = backgroundPosition_ + Vector2(810.0f, 480.0f);

        playerDamagePosition_ = backgroundPosition_ + Vector2(560.0f, 160.0f);
        spellUpgradePosition_ = backgroundPosition_ + Vector2(380.0f, 265.0f);

        headerPosition_ = backgroundPosition_ + Vector2(120.0f, 248.0f);
    }

    void HandleInput() override {
        // exit without bothering to see the rest
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
        }
        // advance to the next player to have leveled up
        else if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            if (leveledUpPlayers_.empty()) {
                // no players at all
                ExitScreen();
                return;
            }
            if (index_ < (int)leveledUpPlayers_.size() - 1) {
                // move to the next player
                index_++;
                GetSpellList();
            } else {
                // no more players
                ExitScreen();
                return;
            }
        }
        // Scroll up
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            if (startIndex_ > 0) {
                startIndex_--;
                endIndex_--;
            }
        }
        // Scroll down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            if (startIndex_ < (int)spellList_.size() - MaxLines) {
                endIndex_++;
                startIndex_++;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        Vector2 currentTextPosition = textPosition_;
        Vector2 currentIconPosition = iconPosition_;
        Vector2 currentLinePosition = linePosition_;
        Vector2 currentLevelPosition = levelPosition_;

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        // Draw the fading screen
        spriteBatch.Draw(fadeTexture_, fadeDest_, Color::White);

        // Draw the popup background
        spriteBatch.Draw(backTexture_, backgroundPosition_, Color::White);

        // Draw the title
        spriteBatch.DrawString(Fonts::HeaderFont(), titleText_, titlePosition_,
                               Fonts::TitleColor);

        DrawPlayerStats();

        // Draw the spell upgrades caption
        spriteBatch.Draw(headerTexture_, headerPosition_, Color::White);
        spriteBatch.DrawString(Fonts::PlayerNameFont(), "Spell Upgrades", spellUpgradePosition_,
                               colorClass_);

        // Draw the horizontal separating lines
        for (int i = 0; i <= MaxLines - 1; i++) {
            currentLinePosition.Y += (float)LineSpacing;
            spriteBatch.Draw(lineTexture_, currentLinePosition, Color::White);
        }

        // Draw the spell upgrade details
        for (int i = startIndex_; i < endIndex_; i++) {
            // Draw the spell icon
            spriteBatch.Draw(*spellList_[(std::size_t)i]->IconTexture, currentIconPosition,
                             Color::White);

            // Draw the spell name
            spriteBatch.DrawString(Fonts::GearInfoFont(), spellList_[(std::size_t)i]->Name,
                                   currentTextPosition, Fonts::CountColor);

            // Draw the spell level
            spriteBatch.DrawString(
                Fonts::GearInfoFont(),
                "Spell Level " + System::Int32::ToString(spellList_[(std::size_t)i]->Level()),
                currentLevelPosition, Fonts::CountColor);

            // Increment to next line position
            currentTextPosition.Y += (float)LineSpacing;
            currentLevelPosition.Y += (float)LineSpacing;
            currentIconPosition.Y += (float)LineSpacing;
        }

        // Draw the scroll bars
        spriteBatch.Draw(scrollUpTexture_, scrollUpPosition_, Color::White);
        spriteBatch.Draw(scrollDownTexture_, scrollDownPosition_, Color::White);

        // Draw the select button and its corresponding text
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectString_, selectPosition_,
                               Color::White);
        spriteBatch.Draw(selectIconTexture_, selectIconPosition_, Color::White);

        spriteBatch.End();
    }

private:
    // Get the spell list.
    void GetSpellList() {
        spellList_.clear();

        if (!leveledUpPlayers_.empty() &&
            leveledUpPlayers_[(std::size_t)index_]->CharacterLevel() <=
                (int)leveledUpPlayers_[(std::size_t)index_]->GetCharacterClass()->LevelEntries.size()) {
            const std::vector<std::shared_ptr<Spell>>& newSpells =
                leveledUpPlayers_[(std::size_t)index_]
                    ->GetCharacterClass()
                    ->LevelEntries[(std::size_t)(
                        leveledUpPlayers_[(std::size_t)index_]->CharacterLevel() - 1)]
                    .Spells;
            if (newSpells.empty()) {
                startIndex_ = 0;
                endIndex_ = 0;
            } else {
                const std::vector<std::shared_ptr<Spell>>& playerSpells =
                    leveledUpPlayers_[(std::size_t)index_]->Spells();
                spellList_.insert(spellList_.end(), playerSpells.begin(), playerSpells.end());
                spellList_.erase(
                    std::remove_if(spellList_.begin(), spellList_.end(),
                                   [&newSpells](const std::shared_ptr<Spell>& spell) {
                                       return std::none_of(
                                           newSpells.begin(), newSpells.end(),
                                           [&spell](const std::shared_ptr<Spell>& newSpell) {
                                               return spell->AssetName() == newSpell->AssetName();
                                           });
                                   }),
                    spellList_.end());
                startIndex_ = 0;
                endIndex_ = std::min(MaxLines, (int)spellList_.size());
            }
        } else {
            startIndex_ = 0;
            endIndex_ = 0;
        }
    }

    // Draw the player stats here.
    void DrawPlayerStats() {
        Vector2 position = topLinePosition_;
        Vector2 posDamage = playerDamagePosition_;
        Player& player = *leveledUpPlayers_[(std::size_t)index_];
        int level = player.CharacterLevel();
        const CharacterLevelingStatistics& levelingStatistics =
            player.GetCharacterClass()->LevelingStatistics;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // Draw the portrait
        spriteBatch.Draw(portraitBackTexture_, backPosition_, Color::White);

        spriteBatch.Draw(*player.ActivePortraitTexture, portraitPosition_, Color::White);

        // Print the character name
        spriteBatch.DrawString(Fonts::PlayerNameFont(), player.Name(), playerNamePosition_,
                               colorName_);

        // Draw the Class Name
        spriteBatch.DrawString(Fonts::PlayerNameFont(), player.GetCharacterClass()->Name,
                               playerClassPosition_, colorClass_);

        // Draw the character level
        spriteBatch.DrawString(Fonts::PlayerNameFont(),
                               "LEVEL: " + System::Int32::ToString(level), playerLvlPosition_,
                               Color::Gray);

        // Draw the character Health Points
        SetColor(levelingStatistics.LevelsPerHealthPointsIncrease == 0
                     ? 0
                     : (level % levelingStatistics.LevelsPerHealthPointsIncrease) *
                           levelingStatistics.HealthPointsIncrease);
        spriteBatch.DrawString(
            Fonts::PlayerStatisticsFont(),
            "HP: " + System::Int32::ToString(player.CurrentStatistics().HealthPoints) + "/" +
                System::Int32::ToString(player.CharacterStatistics().HealthPoints),
            position, color_);

        // Draw the character Mana Points
        position.Y += (float)Fonts::GearInfoFont().getLineSpacingProperty();
        SetColor(levelingStatistics.LevelsPerMagicPointsIncrease == 0
                     ? 0
                     : (level % levelingStatistics.LevelsPerMagicPointsIncrease) *
                           levelingStatistics.MagicPointsIncrease);
        spriteBatch.DrawString(
            Fonts::PlayerStatisticsFont(),
            "MP: " + System::Int32::ToString(player.CurrentStatistics().MagicPoints) + "/" +
                System::Int32::ToString(player.CharacterStatistics().MagicPoints),
            position, color_);

        // Draw the physical offense
        SetColor(levelingStatistics.LevelsPerPhysicalOffenseIncrease == 0
                     ? 0
                     : (level % levelingStatistics.LevelsPerPhysicalOffenseIncrease) *
                           levelingStatistics.PhysicalOffenseIncrease);
        spriteBatch.DrawString(
            Fonts::PlayerStatisticsFont(),
            "PO: " + System::Int32::ToString(player.CurrentStatistics().PhysicalOffense),
            posDamage, color_);

        // Draw the physical defense
        posDamage.Y += (float)Fonts::PlayerStatisticsFont().getLineSpacingProperty();
        SetColor(levelingStatistics.LevelsPerPhysicalDefenseIncrease == 0
                     ? 0
                     : (level % levelingStatistics.LevelsPerPhysicalDefenseIncrease) *
                           levelingStatistics.PhysicalDefenseIncrease);
        spriteBatch.DrawString(
            Fonts::PlayerStatisticsFont(),
            "PD: " + System::Int32::ToString(player.CurrentStatistics().PhysicalDefense),
            posDamage, color_);

        // Draw the Magic offense
        posDamage.Y += (float)Fonts::PlayerStatisticsFont().getLineSpacingProperty();
        SetColor(levelingStatistics.LevelsPerMagicalOffenseIncrease == 0
                     ? 0
                     : (level % levelingStatistics.LevelsPerMagicalOffenseIncrease) *
                           levelingStatistics.MagicalOffenseIncrease);
        spriteBatch.DrawString(
            Fonts::PlayerStatisticsFont(),
            "MO: " + System::Int32::ToString(player.CurrentStatistics().MagicalOffense),
            posDamage, color_);

        // Draw the Magical defense
        posDamage.Y += (float)Fonts::PlayerStatisticsFont().getLineSpacingProperty();
        SetColor(levelingStatistics.LevelsPerMagicalDefenseIncrease == 0
                     ? 0
                     : (level % levelingStatistics.LevelsPerMagicalDefenseIncrease) *
                           levelingStatistics.MagicalDefenseIncrease);
        spriteBatch.DrawString(
            Fonts::PlayerStatisticsFont(),
            "MD: " + System::Int32::ToString(player.CurrentStatistics().MagicalDefense),
            posDamage, color_);
    }

    // Set the current color based on whether the value has changed.
    void SetColor(int value) {
        if (value > 0) {
            color_ = Color::Green;
        } else if (value < 0) {
            color_ = Color::Red;
        } else {
            color_ = colorText_;
        }
    }

    int index_ = 0;
    std::vector<std::shared_ptr<Player>> leveledUpPlayers_;
    std::vector<std::shared_ptr<Spell>> spellList_;

    Texture2D backTexture_;
    Texture2D selectIconTexture_;
    Texture2D portraitBackTexture_;
    Texture2D headerTexture_;
    Texture2D lineTexture_;
    Texture2D scrollUpTexture_;
    Texture2D scrollDownTexture_;
    Texture2D fadeTexture_;
    Color color_;
    Color colorName_{241, 173, 10};
    Color colorClass_{207, 130, 42};
    Color colorText_{76, 49, 8};

    Vector2 backgroundPosition_;
    Vector2 textPosition_;
    Vector2 levelPosition_;
    Vector2 iconPosition_;
    Vector2 linePosition_;
    Vector2 selectPosition_;
    Vector2 selectIconPosition_;
    Vector2 screenSize_;
    Vector2 titlePosition_;
    Vector2 scrollUpPosition_;
    Vector2 scrollDownPosition_;
    Vector2 spellUpgradePosition_;
    Vector2 portraitPosition_;
    Vector2 playerNamePosition_;
    Vector2 playerLvlPosition_;
    Vector2 playerClassPosition_;
    Vector2 topLinePosition_;
    Vector2 playerDamagePosition_;
    Vector2 headerPosition_;
    Vector2 backPosition_;
    Rectangle fadeDest_;

    const std::string titleText_ = "Level Up";
    const std::string selectString_ = "Continue";

    int startIndex_ = 0;
    int endIndex_ = 0;
    static constexpr int MaxLines = 3;
    static constexpr int LineSpacing = 74;
};

} // namespace RolePlaying
