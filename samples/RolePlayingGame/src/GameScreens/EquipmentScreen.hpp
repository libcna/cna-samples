#pragma once

// EquipmentScreen.hpp -- C++ port of GameScreens/EquipmentScreen.cs.

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "System/ArgumentNullException.hpp"

#include "../Data/Characters/FightingCharacter.hpp"
#include "../Data/Gear/Armor.hpp"
#include "../Data/Gear/Equipment.hpp"
#include "../Data/Gear/Weapon.hpp"
#include "../Session/Session.hpp"
#include "ListScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Armor;
using RolePlayingGameData::Equipment;
using RolePlayingGameData::FightingCharacter;
using RolePlayingGameData::Weapon;

// Lists the player's equipped gear, and allows the user to unequip them.
class EquipmentScreen : public ListScreen<std::shared_ptr<Equipment>> {
public:
    // Creates a new EquipmentScreen object for the given player.
    explicit EquipmentScreen(const std::shared_ptr<FightingCharacter>& fightingCharacter)
        : fightingCharacter_(fightingCharacter) {
        // check the parameter
        if (fightingCharacter == nullptr) {
            throw System::ArgumentNullException("fightingCharacter");
        }

        // sort the player's equipment
        std::stable_sort(
            fightingCharacter_->EquippedEquipment().begin(),
            fightingCharacter_->EquippedEquipment().end(),
            [](const std::shared_ptr<Equipment>& equipment1,
               const std::shared_ptr<Equipment>& equipment2) {
                // handle null values
                if (equipment1 == nullptr) {
                    return false;
                }
                if (equipment2 == nullptr) {
                    return true;
                }

                // handle weapons - they're always first in the list
                auto weapon1 = std::dynamic_pointer_cast<Weapon>(equipment1);
                auto weapon2 = std::dynamic_pointer_cast<Weapon>(equipment2);
                if (weapon1 != nullptr) {
                    return weapon2 != nullptr ? equipment1->Name < equipment2->Name : true;
                }
                if (weapon2 != nullptr) {
                    return false;
                }

                // compare armor slots
                auto armor1 = std::dynamic_pointer_cast<Armor>(equipment1);
                auto armor2 = std::dynamic_pointer_cast<Armor>(equipment2);
                if (armor1 != nullptr && armor2 != nullptr) {
                    return armor1->Slot < armor2->Slot;
                }

                return false;
            });

        // configure the menu text
        titleText_ = "Equipped Gear";
        selectButtonText_.clear();
        backButtonText_ = "Back";
        xButtonText_ = "Unequip";
        yButtonText_.clear();
        leftTriggerText_.clear();
        rightTriggerText_.clear();
    }

    // Get the list that this screen displays.
    const std::vector<std::shared_ptr<Equipment>>& GetDataList() override {
        return fightingCharacter_->EquippedEquipment();
    }

protected:
    // Respond to the triggering of the X button (and related key).
    void ButtonXPressed(const std::shared_ptr<Equipment>& entry) override {
        // remove the equipment from the player's equipped list
        fightingCharacter_->Unequip(entry);

        // add the equipment back to the party's inventory
        Session::GetParty()->AddToInventory(entry, 1);
    }

    // Draw the equipment at the given position in the list.
    void DrawEntry(const std::shared_ptr<Equipment>& entry, const Vector2& position,
                   bool isSelected) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;

        // draw the icon
        spriteBatch.Draw(*entry->IconTexture, drawPosition + iconOffset_, Color::White);

        // draw the name
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)NameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), entry->Name, drawPosition, color);

        // draw the power
        drawPosition.X += (float)PowerColumnInterval;
        std::string powerText = entry->GetPowerText();
        Vector2 powerTextSize = Fonts::GearInfoFont().MeasureString(powerText);
        Vector2 powerPosition = drawPosition;
        powerPosition.Y -= std::ceil((powerTextSize.Y - 30.0f) / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), powerText, powerPosition, color);

        // draw the slot
        drawPosition.X += (float)SlotColumnInterval;
        if (std::dynamic_pointer_cast<Weapon>(entry) != nullptr) {
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Weapon", drawPosition, color);
        } else if (auto armor = std::dynamic_pointer_cast<Armor>(entry); armor != nullptr) {
            spriteBatch.DrawString(Fonts::GearInfoFont(),
                                   RolePlayingGameData::ArmorSlotToString(armor->Slot),
                                   drawPosition, color);
        }

        // turn on or off the unequip button
        if (isSelected) {
            xButtonText_ = "Unequip";
        }
    }

    // Draw the description of the selected item.
    void DrawSelectedDescription(const std::shared_ptr<Equipment>& entry) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position = descriptionTextPosition_;

        // draw the description
        // -- it's up to the content owner to fit the description
        std::string text = entry->Description;
        if (!text.empty()) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                   Fonts::DescriptionColor);
            position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }

        // draw the modifiers
        text = entry->OwnerBuffStatistics.GetModifierString();
        if (!text.empty()) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                   Fonts::DescriptionColor);
            position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }

        // draw the restrictions
        text = entry->GetRestrictionsText();
        if (!text.empty()) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                   Fonts::DescriptionColor);
            position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }
    }

    // Draw the column headers above the list.
    void DrawColumnHeaders() override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position = listEntryStartPosition_;

        position.X += (float)NameColumnInterval;
        if (!nameColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), nameColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)PowerColumnInterval;
        if (!powerColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), powerColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)SlotColumnInterval;
        if (!slotColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), slotColumnText_, position,
                                   Fonts::CaptionColor);
        }
    }

    std::string nameColumnText_ = "Name";
    static constexpr int NameColumnInterval = 80;

    std::string powerColumnText_ = "Power (min, max)";
    static constexpr int PowerColumnInterval = 270;

    std::string slotColumnText_ = "Slot";
    static constexpr int SlotColumnInterval = 400;

private:
    // The FightingCharacter object whose equipment is displayed.
    std::shared_ptr<FightingCharacter> fightingCharacter_;
};

} // namespace RolePlaying
