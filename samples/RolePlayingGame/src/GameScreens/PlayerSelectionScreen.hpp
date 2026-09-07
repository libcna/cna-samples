#pragma once

// PlayerSelectionScreen.hpp -- C++ port of GameScreens/PlayerSelectionScreen.cs.

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../Combat/CombatEngine.hpp"
#include "../Data/Characters/Player.hpp"
#include "../Data/Gear/Armor.hpp"
#include "../Data/Gear/Equipment.hpp"
#include "../Data/Gear/Gear.hpp"
#include "../Data/Gear/Item.hpp"
#include "../Data/Gear/Weapon.hpp"
#include "../Data/Int32Range.hpp"
#include "../Data/StatisticsValue.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/Session.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::Armor;
using RolePlayingGameData::Equipment;
using RolePlayingGameData::Gear;
using RolePlayingGameData::Int32Range;
using RolePlayingGameData::Item;
using RolePlayingGameData::Player;
using RolePlayingGameData::StatisticsValue;
using RolePlayingGameData::Weapon;

// Shows a list of players and allows the user to equip or use items.
class PlayerSelectionScreen : public GameScreen {
public:
    // Creates a new PlayerSelectionScreen object.
    explicit PlayerSelectionScreen(const std::shared_ptr<Gear>& gear) : usedGear_(gear) {
        // check the parameter
        if (gear == nullptr) {
            throw System::ArgumentNullException("gear");
        }

        SetIsPopup(true);

        isGearUsed_ = false;
        drawMaximum_ = 3;

        ResetValues();
        Reset();
    }

    void LoadContent() override {
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        // Display screens
        playerInfoScreen_ = content.Load<Texture2D>("Textures/GameScreens/PopupScreen");
        popupPosition_ = Vector2((float)viewport.getWidthProperty() / 2.0f,
                                 (float)viewport.getHeightProperty() / 2.0f);
        popupPosition_.X -= (float)(playerInfoScreen_.getWidthProperty() / 2);
        popupPosition_.Y -= (float)(playerInfoScreen_.getHeightProperty() / 2);

        scoreBoard_ = content.Load<Texture2D>("Textures/GameScreens/CountShieldWithArrow");
        lineTexture_ = content.Load<Texture2D>("Textures/GameScreens/SeparationLine");
        selectButton_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        backButton_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        tickMarkTexture_ = content.Load<Texture2D>("Textures/GameScreens/TickMark");
        playerSelTexture_ = content.Load<Texture2D>("Textures/GameScreens/PlayerSelected");
        playerUnSelTexture_ = content.Load<Texture2D>("Textures/GameScreens/PlayerUnSelected");

        titlePosition_ = Vector2(
            ((float)viewport.getWidthProperty() -
             Fonts::HeaderFont().MeasureString("Choose Player").X) / 2.0f,
            (float)(viewport.getHeightProperty() - playerInfoScreen_.getHeightProperty()) / 2.0f +
                70.0f);
    }

    // Reset the selection and player data.
    void Reset() {
        if (selectionMark_ != -1) {
            isUseAllowed_ = true;
            if (usedGear_ != nullptr) {
                isUseAllowed_ = usedGear_->CheckRestrictions(
                    *Session::GetParty()->Players[(std::size_t)selectionMark_]);
            }

            CalculateSelectedPlayers();
            CalculateForPreview();
        }
    }

    void HandleInput() override {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }
        // use the item or close the screen
        else if (isUseAllowed_ && InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            if (isGearUsed_) {
                ExitScreen();
                return;
            } else {
                if (auto equipment = std::dynamic_pointer_cast<Equipment>(usedGear_);
                    equipment != nullptr) {
                    std::shared_ptr<Equipment> oldEquipment;
                    if (Session::GetParty()->Players[(std::size_t)selectionMark_]->Equip(
                            equipment, oldEquipment)) {
                        Session::GetParty()->RemoveFromInventory(usedGear_, 1);
                        if (oldEquipment != nullptr) {
                            Session::GetParty()->AddToInventory(oldEquipment, 1);
                        }
                        isGearUsed_ = true;
                    }
                } else if (auto item = std::dynamic_pointer_cast<Item>(usedGear_);
                           item != nullptr) {
                    if ((item->Usage & Item::ItemUsage::NonCombat) > 0) {
                        if (Session::GetParty()->RemoveFromInventory(item, 1)) {
                            Session::GetParty()
                                ->Players[(std::size_t)selectionMark_]
                                ->StatisticsModifiers +=
                                item->TargetEffectRange.GenerateValue(Session::GetRandom());
                            Session::GetParty()
                                ->Players[(std::size_t)selectionMark_]
                                ->StatisticsModifiers.ApplyMaximum(StatisticsValue());
                            isGearUsed_ = true;
                        } else {
                            ExitScreen();
                            return;
                        }
                    }
                }
            }
            return;
        }
        // cursor up
        else if (!isGearUsed_ && InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            if (selectionMark_ > 0) {
                ResetFromPreview();
                selectionMark_--;

                if (selectionMark_ < startIndex_) {
                    startIndex_--;
                    endIndex_--;
                }
                isUseAllowed_ = true;
                if (usedGear_ != nullptr) {
                    isUseAllowed_ = usedGear_->CheckRestrictions(
                        *Session::GetParty()->Players[(std::size_t)selectionMark_]);
                }

                CalculateSelectedPlayers();
                CalculateForPreview();
            }
        }
        // cursor down
        else if (!isGearUsed_ &&
                 InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            isGearUsed_ = false;
            if (selectionMark_ < (int)Session::GetParty()->Players.size() - 1) {
                ResetFromPreview();

                selectionMark_++;

                if (selectionMark_ == endIndex_) {
                    endIndex_++;
                    startIndex_++;
                }
                isUseAllowed_ = true;
                if (usedGear_ != nullptr) {
                    isUseAllowed_ = usedGear_->CheckRestrictions(
                        *Session::GetParty()->Players[(std::size_t)selectionMark_]);
                }

                CalculateSelectedPlayers();
                CalculateForPreview();
            }
        }
    }

    // Draw the character stats screen and text.
    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        spriteBatch.Draw(fadeTexture_, Rectangle(0, 0, 1280, 720), Color::White);

        currentTextPosition_ = textPosition_;

        spriteBatch.Draw(playerInfoScreen_, popupPosition_, Color::White);

        // DrawButtons
        DrawButtons();

        // Draw Heros
        DrawViewablePlayers();

        // Display Title of the Screen
        spriteBatch.DrawString(Fonts::HeaderFont(), "Choose Player", titlePosition_,
                               Fonts::TitleColor);

        spriteBatch.End();
    }

private:
    // Reset the Variables to the Initial values.
    void ResetValues() {
        startIndex_ = 0;
        if (drawMaximum_ > (int)Session::GetParty()->Players.size()) {
            endIndex_ = (int)Session::GetParty()->Players.size();
        } else {
            endIndex_ = drawMaximum_;
        }
        selectionMark_ = 0;
        CalculateSelectedPlayers();
    }

    // Draw a player's Details.
    void DrawPlayerDetails(Player& player, bool isSelected) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        Vector2 position;
        Vector2 equipEffectPosition;
        Color textColor;
        Color nameColor, classColor, levelColor;
        std::string text;
        int length;

        if (isSelected) {
            textColor = Color::Black;
            nameColor = Color(241, 173, 10);
            classColor = Color(207, 131, 42);
            levelColor = Color(151, 150, 148);
        } else {
            textColor = Color::DarkGray;
            nameColor = Color(117, 88, 18);
            classColor = Color(125, 78, 24);
            levelColor = Color(110, 106, 99);
        }

        position = currentTextPosition_;
        position.Y -= 5.0f;
        if (isSelected) {
            spriteBatch.Draw(playerSelTexture_, position, Color::White);
        } else {
            spriteBatch.Draw(playerUnSelTexture_, position, Color::White);
        }
        position.Y += 5.0f;

        // Draw portrait
        portraitPosition_.X = position.X + 3.0f;
        portraitPosition_.Y = position.Y + 16.0f;
        spriteBatch.Draw(*player.ActivePortraitTexture, portraitPosition_, Color::White);
        if (isGearUsed_ && isSelected) {
            spriteBatch.Draw(tickMarkTexture_, position, Color::White);
        }

        // Draw Player Name
        playerNamePosition_.X = position.X + 90.0f;
        playerNamePosition_.Y = position.Y + 15.0f;
        spriteBatch.DrawString(Fonts::PlayerNameFont(), ToUpper(player.Name()),
                               playerNamePosition_, nameColor);

        // Draw Player Class
        playerNamePosition_.Y += 25.0f;
        spriteBatch.DrawString(Fonts::PlayerNameFont(), player.GetCharacterClass()->Name,
                               playerNamePosition_, classColor);

        // Draw Player Level
        playerNamePosition_.Y += 26.0f;
        spriteBatch.DrawString(Fonts::PlayerNameFont(),
                               "LEVEL: " + System::Int32::ToString(player.CharacterLevel()),
                               playerNamePosition_, levelColor);
        position = currentTextPosition_;
        position.X += (float)playerSelTexture_.getWidthProperty() + 5.0f;
        DrawPlayerStats(player, isSelected, position);

        equipEffectPosition = position;
        equipEffectPosition.X += 100.0f;
        equipEffectPosition.Y = currentTextPosition_.Y;

        text = "Weapon Atk: (";
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition,
                               Fonts::CountColor);
        equipEffectPosition.X += (float)length;

        // calculate weapon damage
        previewDamageRange_ = Int32Range();
        previewHealthDefenseRange_ = Int32Range();
        previewMagicDefenseRange_ = Int32Range();
        if (isSelected && isUseAllowed_ && !isGearUsed_) {
            if (auto equipment = std::dynamic_pointer_cast<Equipment>(usedGear_);
                equipment != nullptr) {
                if (auto weapon = std::dynamic_pointer_cast<Weapon>(equipment); weapon != nullptr) {
                    previewDamageRange_ = weapon->TargetDamageRange;
                    std::shared_ptr<Weapon> equippedWeapon = player.GetEquippedWeapon();
                    if (equippedWeapon != nullptr) {
                        previewDamageRange_ -= equippedWeapon->TargetDamageRange;
                        previewDamageRange_ -= equippedWeapon->OwnerBuffStatistics.PhysicalOffense;
                        previewHealthDefenseRange_ -=
                            equippedWeapon->OwnerBuffStatistics.PhysicalDefense;
                        previewMagicDefenseRange_ -=
                            equippedWeapon->OwnerBuffStatistics.MagicalDefense;
                    }
                } else if (auto armor = std::dynamic_pointer_cast<Armor>(equipment);
                           armor != nullptr) {
                    previewHealthDefenseRange_ = armor->OwnerHealthDefenseRange;
                    previewMagicDefenseRange_ = armor->OwnerMagicDefenseRange;
                    std::shared_ptr<Armor> equippedArmor = player.GetEquippedArmor(armor->Slot);
                    if (equippedArmor != nullptr) {
                        previewHealthDefenseRange_ -= equippedArmor->OwnerHealthDefenseRange;
                        previewMagicDefenseRange_ -= equippedArmor->OwnerMagicDefenseRange;
                        previewDamageRange_ -= equippedArmor->OwnerBuffStatistics.PhysicalOffense;
                        previewHealthDefenseRange_ -=
                            equippedArmor->OwnerBuffStatistics.PhysicalDefense;
                        previewMagicDefenseRange_ -=
                            equippedArmor->OwnerBuffStatistics.MagicalDefense;
                    }
                }
                previewDamageRange_ += equipment->OwnerBuffStatistics.PhysicalOffense;
                previewHealthDefenseRange_ += equipment->OwnerBuffStatistics.PhysicalDefense;
                previewMagicDefenseRange_ += equipment->OwnerBuffStatistics.MagicalDefense;
            }
        }

        Int32Range drawWeaponDamageRange = player.TargetDamageRange() + previewDamageRange_ +
                                           player.CharacterStatistics().PhysicalOffense;
        text = System::Int32::ToString(drawWeaponDamageRange.Minimum);
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        textColor = GetRangeColor(previewDamageRange_.Minimum, isSelected);
        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition, textColor);
        equipEffectPosition.X += (float)length;

        text = ",";
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition,
                               Fonts::CountColor);
        equipEffectPosition.X += (float)length;

        text = System::Int32::ToString(drawWeaponDamageRange.Maximum);
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        textColor = GetRangeColor(previewDamageRange_.Maximum, isSelected);
        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition, textColor);
        equipEffectPosition.X += (float)length;

        spriteBatch.DrawString(Fonts::DescriptionFont(), ")", equipEffectPosition,
                               Fonts::CountColor);

        equipEffectPosition.X = position.X + 100.0f;
        equipEffectPosition.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        text = "Weapon Def: (";
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition,
                               Fonts::CountColor);
        equipEffectPosition.X += (float)length;

        Int32Range drawHealthDefenseRange = player.HealthDefenseRange() +
                                            previewHealthDefenseRange_ +
                                            player.CharacterStatistics().PhysicalDefense;
        text = System::Int32::ToString(drawHealthDefenseRange.Minimum);
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        textColor = GetRangeColor(previewHealthDefenseRange_.Minimum, isSelected);
        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition, textColor);
        equipEffectPosition.X += (float)length;

        text = ",";
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition,
                               Fonts::CountColor);
        equipEffectPosition.X += (float)length;

        text = System::Int32::ToString(drawHealthDefenseRange.Maximum);
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        textColor = GetRangeColor(previewHealthDefenseRange_.Maximum, isSelected);
        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition, textColor);
        equipEffectPosition.X += (float)length;

        spriteBatch.DrawString(Fonts::DescriptionFont(), ")", equipEffectPosition,
                               Fonts::CountColor);

        equipEffectPosition.X = position.X + 100.0f;
        equipEffectPosition.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        text = "Spell Def: (";
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition,
                               Fonts::CountColor);
        equipEffectPosition.X += (float)length;

        Int32Range drawMagicDefenseRange = player.MagicDefenseRange() +
                                           previewMagicDefenseRange_ +
                                           player.CharacterStatistics().MagicalDefense;
        text = System::Int32::ToString(drawMagicDefenseRange.Minimum);
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        textColor = GetRangeColor(previewMagicDefenseRange_.Minimum, isSelected);
        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition, textColor);
        equipEffectPosition.X += (float)length;

        text = ",";
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition,
                               Fonts::CountColor);
        equipEffectPosition.X += (float)length;

        text = System::Int32::ToString(drawMagicDefenseRange.Maximum);
        length = (int)Fonts::DescriptionFont().MeasureString(text).X;

        textColor = GetRangeColor(previewMagicDefenseRange_.Maximum, isSelected);
        spriteBatch.DrawString(Fonts::DescriptionFont(), text, equipEffectPosition, textColor);
        equipEffectPosition.X += (float)length;

        spriteBatch.DrawString(Fonts::DescriptionFont(), ")", equipEffectPosition,
                               Fonts::CountColor);

        currentTextPosition_.Y = position.Y + 3.0f;

        spriteBatch.Draw(lineTexture_, currentTextPosition_, Color::White);

        currentTextPosition_.Y += 20.0f;
    }

    // Draw a Player's stats.
    void DrawPlayerStats(Player& player, bool isSelected, Vector2& position) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        Color color;

        StatisticsValue playersStatisticsModifier;
        if (isSelected && isUseAllowed_ && !isGearUsed_) {
            playersStatisticsModifier = previewStatisticsModifier_;
            if (auto armor = std::dynamic_pointer_cast<Armor>(usedGear_); armor != nullptr) {
                std::shared_ptr<Armor> existingArmor = player.GetEquippedArmor(armor->Slot);
                if (existingArmor != nullptr) {
                    playersStatisticsModifier -= existingArmor->OwnerBuffStatistics;
                }
            } else if (auto weapon = std::dynamic_pointer_cast<Weapon>(usedGear_);
                       weapon != nullptr) {
                std::shared_ptr<Weapon> existingWeapon = player.GetEquippedWeapon();
                if (existingWeapon != nullptr) {
                    playersStatisticsModifier -= existingWeapon->OwnerBuffStatistics;
                }
            }
        }

        StatisticsValue drawCurrentStatistics = player.CurrentStatistics();
        StatisticsValue drawCharacterStatistics = player.CharacterStatistics();
        if (isSelected) {
            drawCurrentStatistics += playersStatisticsModifier;
            drawCharacterStatistics += playersStatisticsModifier;
        }

        // Draw the character Health Points
        color = GetStatColor(playersStatisticsModifier.HealthPoints, isSelected);
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "HP: " + System::Int32::ToString(drawCurrentStatistics.HealthPoints) + "/" +
                System::Int32::ToString(drawCharacterStatistics.HealthPoints),
            position, color);

        // Draw the character Mana Points
        position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        color = GetStatColor(playersStatisticsModifier.MagicPoints, isSelected);
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "MP: " + System::Int32::ToString(drawCurrentStatistics.MagicPoints) + "/" +
                System::Int32::ToString(drawCharacterStatistics.MagicPoints),
            position, color);

        // Draw the physical offense
        position.X += 150.0f;
        position.Y -= (float)Fonts::DescriptionFont().getLineSpacingProperty();
        color = GetStatColor(playersStatisticsModifier.PhysicalOffense, isSelected);
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "PO: " + System::Int32::ToString(drawCurrentStatistics.PhysicalOffense), position,
            color);

        // Draw the physical defense
        position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        color = GetStatColor(playersStatisticsModifier.PhysicalDefense, isSelected);
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "PD: " + System::Int32::ToString(drawCurrentStatistics.PhysicalDefense), position,
            color);

        // Draw the Magic offense
        position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        color = GetStatColor(playersStatisticsModifier.MagicalOffense, isSelected);
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "MO: " + System::Int32::ToString(drawCurrentStatistics.MagicalOffense), position,
            color);

        // Draw the Magical defense
        position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        color = GetStatColor(playersStatisticsModifier.MagicalDefense, isSelected);
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "MD: " + System::Int32::ToString(drawCurrentStatistics.MagicalDefense), position,
            color);

        position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
    }

    // Draw the Character Stats and Character Icons.
    void DrawViewablePlayers() {
        bool isSelectedPlayer = false;

        // Compute Start Index
        if (startIndex_ < 0) {
            startIndex_ = 0;
            selectionMark_ = 0;
            CalculateSelectedPlayers();
        }
        // Compute EndIndex
        if (endIndex_ > (int)Session::GetParty()->Players.size()) {
            endIndex_ = (int)Session::GetParty()->Players.size();
            selectionMark_ = endIndex_ - 1;
            CalculateSelectedPlayers();
        }
        for (int playerIndex = startIndex_; playerIndex < endIndex_; playerIndex++) {
            isSelectedPlayer = false;
            for (int selected : selectedPlayers_) {
                if (playerIndex == selected) {
                    isSelectedPlayer = true;
                    break;
                }
            }
            DrawPlayerDetails(*Session::GetParty()->Players[(std::size_t)playerIndex],
                              isSelectedPlayer);
        }
        // Draw the Scroll button only if player count exceed the Max items
        if (selectionMark_ != -1) {
            if ((int)Session::GetParty()->Players.size() > drawMaximum_) {
                DrawCharacterCount();
            }
        }
    }

    // Draw the Current player Selected and total number of players in the list.
    void DrawCharacterCount() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position;

        // Draw the ScoreBoard
        spriteBatch.Draw(scoreBoard_, scoreBoardPosition_, Color::White);

        position = scoreBoardPosition_;
        position.X += 29.0f;
        position.Y += 100.0f;

        // Display Current Selected Player
        spriteBatch.DrawString(Fonts::GearInfoFont(), System::Int32::ToString(selectionMark_ + 1),
                               position, Fonts::CountColor);
        position.Y += 30.0f;
        // Display Total Players count
        spriteBatch.DrawString(Fonts::GearInfoFont(),
                               System::Int32::ToString((int)Session::GetParty()->Players.size()),
                               position, Fonts::CountColor);
    }

    // Draw Select and Drop Button.
    void DrawButtons() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position;
        Vector2 placeTextMid;
        std::string selectText;

        if (usedGear_ == nullptr) {
            selectText = "Use";
        } else if (std::dynamic_pointer_cast<Item>(usedGear_) != nullptr) {
            selectText = "Use";
        } else {
            selectText = "Equip";
        }
        if (CombatEngine::IsActive()) {
            if (selectionMark_ != -1) {
                isUseAllowed_ = true;
            }
        }

        if (isUseAllowed_ && !isGearUsed_) {
            // Draw Select Button
            spriteBatch.Draw(selectButton_, selectButtonPosition_, Color::White);
            // Display Text
            position = selectButtonPosition_;
            placeTextMid = Fonts::ButtonNamesFont().MeasureString(selectText);
            position.X -= placeTextMid.X + 10.0f;
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectText, position, Color::White);
        }

        // Draw Back Button
        spriteBatch.Draw(backButton_, backButtonPosition_, Color::White);
        // Display Back Text
        position = backButtonPosition_;
        position.X += (float)backButton_.getWidthProperty() + 10.0f;
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back", position, Color::White);
    }

    // Gets font color for stat display based on whether the stat has changed.
    Color GetStatColor(int change, bool isSelected) const {
        if (isSelected && isUseAllowed_) {
            if (change < 0) {
                return Color::Red;
            } else if (change > 0) {
                return Color::Green;
            }
            // fall through when == 0
        }
        return Fonts::CountColor;
    }

    // Decides min/max colour of Weapon Attack/Weapon Def/Spell Def of player.
    Color GetRangeColor(int value, bool isSelected) const {
        if (isSelected && isUseAllowed_) {
            if (value > 0) {
                return Color::Green;
            } else if (value < 0) {
                return Color::Red;
            } else {
                return Fonts::CountColor;
            }
        }
        return Fonts::CountColor;
    }

    // Calculate selected players around the selection mark based on the range for items. In case
    // of equipment the range is considered 0.
    void CalculateSelectedPlayers() {
        int range = 0;
        int selMark = selectionMark_;

        selectedPlayers_.clear();

        auto item = std::dynamic_pointer_cast<Item>(usedGear_);
        if (item != nullptr) {
            range = item->AdjacentTargets;
        }

        selectedPlayers_.push_back(selMark);
        for (int i = 1; i <= range; i++) {
            if (selMark >= i &&
                !Session::GetParty()->Players[(std::size_t)(selMark - i)]->IsDeadOrDying()) {
                selectedPlayers_.push_back(selMark - i);
            }
            if (selMark < (int)Session::GetParty()->Players.size() - i &&
                !Session::GetParty()->Players[(std::size_t)(selMark + i)]->IsDeadOrDying()) {
                selectedPlayers_.push_back(selMark + i);
            }
        }
    }

    // Calculate selected players' stats for preview.
    void CalculateForPreview() {
        previewStatisticsModifier_ = StatisticsValue();
        previewDamageRange_ = Int32Range();
        previewHealthDefenseRange_ = Int32Range();
        previewMagicDefenseRange_ = Int32Range();
        if (isUseAllowed_ && !isGearUsed_) {
            if (std::dynamic_pointer_cast<Item>(usedGear_) != nullptr) {
                // no preview for items
            } else if (auto armor = std::dynamic_pointer_cast<Armor>(usedGear_);
                       armor != nullptr) {
                previewStatisticsModifier_ = armor->OwnerBuffStatistics;
                previewHealthDefenseRange_ = armor->OwnerHealthDefenseRange;
                previewMagicDefenseRange_ = armor->OwnerMagicDefenseRange;
            } else if (auto weapon = std::dynamic_pointer_cast<Weapon>(usedGear_);
                       weapon != nullptr) {
                previewStatisticsModifier_ = weapon->OwnerBuffStatistics;
                previewDamageRange_ = weapon->TargetDamageRange;
            }
        }
    }

    // Reset stats of previously selected players from preview.
    void ResetFromPreview() {
        previewStatisticsModifier_ = StatisticsValue();
        previewDamageRange_ = Int32Range();
        previewHealthDefenseRange_ = Int32Range();
        previewMagicDefenseRange_ = Int32Range();
    }

    // C#'s string.ToUpper(), for the player name banner.
    static std::string ToUpper(const std::string& value) {
        std::string result = value;
        for (char& c : result) {
            if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
        }
        return result;
    }

    std::shared_ptr<Gear> usedGear_;

    bool isUseAllowed_ = false;
    std::vector<int> selectedPlayers_;
    StatisticsValue previewStatisticsModifier_;
    Int32Range previewDamageRange_;
    Int32Range previewHealthDefenseRange_;
    Int32Range previewMagicDefenseRange_;

    Texture2D playerInfoScreen_;
    Texture2D backButton_;
    Texture2D selectButton_;
    Texture2D scoreBoard_;
    Texture2D fadeTexture_;
    Texture2D tickMarkTexture_;
    Texture2D lineTexture_;
    Texture2D playerSelTexture_;
    Texture2D playerUnSelTexture_;

    const Vector2 textPosition_{264.0f, 199.0f};
    Vector2 currentTextPosition_;
    const Vector2 namePosition_{394.0f, 39.0f};
    Vector2 titlePosition_;
    const Vector2 scoreBoardPosition_{972.0f, 235.0f};
    const Vector2 selectButtonPosition_{891.0f, 550.0f};
    const Vector2 backButtonPosition_{331.0f, 550.0f};
    Vector2 popupPosition_;
    Vector2 playerNamePosition_;
    Vector2 portraitPosition_;
    const Point startPositionScreen_{204, 44};
    const Rectangle screenRect_{0, 0, 872, 633};

    int selectionMark_ = 0;
    bool isGearUsed_ = false;
    int startIndex_ = 0;
    int endIndex_ = 0;
    int drawMaximum_ = 0;
};

// ---- InventoryScreen selection, which opens this screen ----

inline void InventoryScreen::SelectTriggered(const std::shared_ptr<ContentEntry<Gear>>& entry) {
    // check the parameter
    if (entry == nullptr || entry->Content == nullptr) {
        return;
    }

    // if the event is valid, fire it and exit this screen
    if (GearSelected) {
        GearSelected(entry->Content);
        ExitScreen();
        return;
    }

    // otherwise, open the selection screen over this screen
    GetScreenManager()->AddScreen(std::make_shared<PlayerSelectionScreen>(entry->Content));
}

} // namespace RolePlaying
