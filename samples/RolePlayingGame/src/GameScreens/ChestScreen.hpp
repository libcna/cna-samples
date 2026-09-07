#pragma once

// ChestScreen.hpp -- C++ port of GameScreens/ChestScreen.cs.

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../AudioManager.hpp"
#include "../Data/Map/Chest.hpp"
#include "../Data/MapEntry.hpp"
#include "../Session/Session.hpp"
#include "InventoryScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Chest;
using RolePlayingGameData::MapEntry;

// Displays the contents of a chest, and lets the party take them.
class ChestScreen : public InventoryScreen {
public:
    // Creates a new ChestScreen object.
    explicit ChestScreen(const std::shared_ptr<MapEntry<Chest>>& chestEntry)
        : InventoryScreen(true), chestEntry_(chestEntry) {
        // check the parameter
        if (chestEntry == nullptr || chestEntry->Content == nullptr) {
            throw System::ArgumentNullException("chestEntry.Content");
        }

        // clean up any empty entries
        RemoveEmptyEntries();

        // sort the chest entries by name
        std::stable_sort(chestEntry_->Content->Entries.begin(),
                         chestEntry_->Content->Entries.end(),
                         [](const std::shared_ptr<ContentEntry<Gear>>& gearEntry1,
                            const std::shared_ptr<ContentEntry<Gear>>& gearEntry2) {
                             // handle null values
                             if (gearEntry1 == nullptr || gearEntry1->Content == nullptr) {
                                 return false;
                             }
                             if (gearEntry2 == nullptr || gearEntry2->Content == nullptr) {
                                 return true;
                             }
                             return gearEntry1->Content->Name < gearEntry2->Content->Name;
                         });

        // set up the initial selected-quantity values
        ResetSelectedQuantity();

        // configure the menu strings
        titleText_ = chestEntry_->Content->Name();
        selectButtonText_ = "Take";
        backButtonText_ = "Close";
        xButtonText_.clear();
        yButtonText_ = "Take All";
        leftTriggerText_.clear();
        rightTriggerText_.clear();
    }

    // Retrieve the list of gear shown in this menu.
    const std::vector<std::shared_ptr<ContentEntry<Gear>>>& GetDataList() override {
        return chestEntry_->Content->Entries;
    }

    void LoadContent() override {
        InventoryScreen::LoadContent();

        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        leftQuantityArrow_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowLeft");
        rightQuantityArrow_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowRight");
    }

    // If true, the phantom ContentEntry for the chest's gold is selected.
    bool IsGoldSelected() const {
        return chestEntry_ != nullptr && chestEntry_->Content != nullptr &&
               chestEntry_->Content->Gold > 0 && SelectedIndex() == 0;
    }

    // Retrieve the zero-based selection of the gear in the chest's contents. If there is gold in
    // the chest, its phantom ContentEntry shifts ListScreen::SelectedIndex by one.
    int SelectedGearIndex() const {
        return (chestEntry_ != nullptr && chestEntry_->Content != nullptr &&
                chestEntry_->Content->Gold > 0)
                   ? SelectedIndex() - 1
                   : SelectedIndex();
    }

    // Allows the screen to handle user input.
    void HandleInput() override {
        // if the chest's contents are empty, exit immediately
        if (chestEntry_->Content->IsEmpty()) {
            BackTriggered();
            return;
        }

        if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            MoveCursorUp();
        } else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            MoveCursorDown();
        } else if (InputManager::IsActionTriggered(InputManager::Action::IncreaseAmount)) {
            MoveCursorRight();
        } else if (InputManager::IsActionTriggered(InputManager::Action::DecreaseAmount)) {
            MoveCursorLeft();
        }
        // Close is pressed
        else if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            BackTriggered();
        }
        // Take is pressed
        else if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            if (IsGoldSelected()) {
                SelectTriggered(nullptr);
            } else {
                const std::vector<std::shared_ptr<ContentEntry<Gear>>>& dataList = GetDataList();
                SelectTriggered(dataList[(std::size_t)SelectedGearIndex()]);
            }
        }
        // Take All is pressed
        else if (InputManager::IsActionTriggered(InputManager::Action::TakeView)) {
            ButtonYPressed(nullptr); // take-all doesn't need an entry
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // get the content list
        const std::vector<std::shared_ptr<ContentEntry<Gear>>>& dataList = GetDataList();

        // fix the indices for the current list size
        int maximumCount =
            chestEntry_->Content->Gold > 0 ? (int)dataList.size() + 1 : (int)dataList.size();
        SetSelectedIndex(
            (int)MathHelper::Clamp((float)SelectedIndex(), 0.0f, (float)maximumCount - 1.0f));
        SetStartIndex((int)MathHelper::Clamp((float)StartIndex(), 0.0f,
                                             (float)(maximumCount - MaximumListEntries)));
        SetEndIndex(std::min(StartIndex() + MaximumListEntries, maximumCount));

        spriteBatch.Begin();

        DrawBackground();
        if (!dataList.empty()) {
            DrawListPosition(SelectedIndex() + 1, maximumCount);
        }
        DrawButtons();
        DrawPartyGold();
        DrawColumnHeaders();
        DrawTitle();

        // draw each item currently shown
        Vector2 position = listEntryStartPosition_ + Vector2(0.0f, (float)listLineSpacing / 2.0f);
        for (int index = StartIndex(); index < EndIndex(); index++) {
            if (index == 0 && chestEntry_->Content->Gold > 0) {
                if (index == SelectedIndex()) {
                    DrawSelection(position);
                    DrawGoldEntry(position, true);
                } else {
                    DrawGoldEntry(position, false);
                }
            } else {
                int currentIndex = chestEntry_->Content->Gold > 0 ? index - 1 : index;
                const std::shared_ptr<ContentEntry<Gear>>& entry =
                    dataList[(std::size_t)currentIndex];
                if (index == SelectedIndex()) {
                    DrawSelection(position);
                    DrawEntry(entry, position, true);
                    DrawSelectedDescription(entry);
                } else {
                    DrawEntry(entry, position, false);
                }
            }
            position.Y += (float)listLineSpacing;
        }

        spriteBatch.End();
    }

protected:
    // Move the current selection up one entry.
    void MoveCursorUp() override {
        InventoryScreen::MoveCursorUp();
        ResetSelectedQuantity();
    }

    // Move the current selection down one entry.
    void MoveCursorDown() override {
        InventoryScreen::MoveCursorDown();
        ResetSelectedQuantity();
    }

    // Decrease the selected quantity by one.
    void MoveCursorLeft() override {
        // decrement the quantity, looping around if necessary
        if (selectedQuantity_ > 0) {
            selectedQuantity_--;
        } else if (IsGoldSelected()) {
            selectedQuantity_ = chestEntry_->Content->Gold;
        } else if (SelectedGearIndex() < (int)chestEntry_->Content->Entries.size()) {
            selectedQuantity_ =
                chestEntry_->Content->Entries[(std::size_t)SelectedGearIndex()]->Count;
        }
    }

    // Increase the selected quantity by one.
    void MoveCursorRight() override {
        int maximumQuantity = 0;
        // get the maximum quantity for the selected entry
        if (IsGoldSelected()) {
            maximumQuantity = chestEntry_->Content->Gold;
        } else if (SelectedGearIndex() < (int)chestEntry_->Content->Entries.size()) {
            maximumQuantity =
                chestEntry_->Content->Entries[(std::size_t)SelectedGearIndex()]->Count;
        } else {
            return;
        }
        // loop to zero if the selected quantity is already at maximum.
        selectedQuantity_ = selectedQuantity_ < maximumQuantity ? selectedQuantity_ + 1 : 0;
    }

    // Respond to the triggering of the Back action.
    void BackTriggered() override {
        // clean up any empty entries
        RemoveEmptyEntries();

        // if the chest's contents are empty, remove it from the game and exit
        if (chestEntry_->Content->IsEmpty()) {
            Session::RemoveChest(chestEntry_);
        } else {
            // otherwise, store the modified chest
            Session::StoreModifiedChest(chestEntry_);
        }

        // exit the screen
        InventoryScreen::BackTriggered();
    }

    // Respond to the triggering of the Select action.
    void SelectTriggered(const std::shared_ptr<ContentEntry<Gear>>& entry) override {
        // if the quantity is zero, don't bother
        if (selectedQuantity_ <= 0) {
            return;
        }

        // check to see if gold is selected
        if (IsGoldSelected()) {
            // play the "pick up gold" cue
            AudioManager::PlayCue("Money");
            // add the gold to the party
            Session::GetParty()->AddPartyGold(selectedQuantity_);
            chestEntry_->Content->Gold -= selectedQuantity_;
            if (chestEntry_->Content->Gold > 0) {
                selectedQuantity_ = std::min(selectedQuantity_, chestEntry_->Content->Gold);
            } else {
                ResetSelectedQuantity();
            }
        } else {
            // remove the selected quantity of gear from the chest
            int quantity = selectedQuantity_;
            if (entry->Content != nullptr && quantity > 0) {
                Session::GetParty()->AddToInventory(entry->Content, quantity);
                entry->Count -= quantity;
            }
            if (entry->Count > 0) {
                selectedQuantity_ = std::min(entry->Count, selectedQuantity_);
            } else {
                // if the entry is now empty, remove it from the chest
                chestEntry_->Content->Entries.erase(chestEntry_->Content->Entries.begin() +
                                                    SelectedGearIndex());
                ResetSelectedQuantity();
            }
        }
    }

    // Respond to the triggering of the Y button (and related key).
    void ButtonYPressed(const std::shared_ptr<ContentEntry<Gear>>& entry) override {
        (void)entry;
        // add the entire amount of gold
        if (chestEntry_->Content->Gold > 0) {
            AudioManager::PlayCue("Money");
            Session::GetParty()->AddPartyGold(chestEntry_->Content->Gold);
            chestEntry_->Content->Gold = 0;
        }
        // add all items at full quantity
        // -- there is no limit to the party's inventory
        for (const std::shared_ptr<ContentEntry<Gear>>& gearEntry : GetDataList()) {
            Session::GetParty()->AddToInventory(gearEntry->Content, gearEntry->Count);
        }
        // clear the entries, as they're all gone now
        chestEntry_->Content->Entries.clear();
        selectedQuantity_ = 0;
    }

    // Draw the chest's gold at the given position in the list.
    virtual void DrawGoldEntry(const Vector2& position, bool isSelected) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;

        // draw the icon
        spriteBatch.Draw(goldTexture_, drawPosition + iconOffset_ + Vector2(0.0f, 7.0f),
                         Color::White);

        // draw the name
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)ChestNameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), "Gold", drawPosition, color);

        // skip the power text
        drawPosition.X += (float)ChestPowerColumnInterval;

        // draw the quantity
        drawPosition.X += (float)ChestQuantityColumnInterval;
        if (isSelected) {
            // draw the left selection arrow
            drawPosition.X -= (float)leftQuantityArrow_.getWidthProperty();
            spriteBatch.Draw(leftQuantityArrow_, Vector2(drawPosition.X, drawPosition.Y - 4.0f),
                             Color::White);
            drawPosition.X += (float)leftQuantityArrow_.getWidthProperty();
            // draw the selected quantity ratio
            std::string quantityText = System::Int32::ToString(selectedQuantity_) + "/" +
                                       System::Int32::ToString(chestEntry_->Content->Gold);
            spriteBatch.DrawString(Fonts::GearInfoFont(), quantityText, drawPosition, color);
            drawPosition.X += Fonts::GearInfoFont().MeasureString(quantityText).X;
            // draw the right selection arrow
            spriteBatch.Draw(rightQuantityArrow_, Vector2(drawPosition.X, drawPosition.Y - 4.0f),
                             Color::White);
            drawPosition.X += (float)rightQuantityArrow_.getWidthProperty();
        } else {
            // draw the remaining quantity
            spriteBatch.DrawString(Fonts::GearInfoFont(),
                                   System::Int32::ToString(chestEntry_->Content->Gold),
                                   drawPosition, color);
        }
    }

    // Draw the gear's content entry at the given position in the list.
    void DrawEntry(const std::shared_ptr<ContentEntry<Gear>>& entry, const Vector2& position,
                   bool isSelected) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }
        const std::shared_ptr<Gear>& gear = entry->Content;
        if (gear == nullptr) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;

        // draw the icon
        spriteBatch.Draw(*gear->IconTexture, drawPosition + iconOffset_, Color::White);

        // draw the name
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)ChestNameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), gear->Name, drawPosition, color);

        // draw the power
        drawPosition.X += (float)ChestPowerColumnInterval;
        std::string powerText = gear->GetPowerText();
        Vector2 powerTextSize = Fonts::GearInfoFont().MeasureString(powerText);
        Vector2 powerPosition = drawPosition;
        powerPosition.Y -= std::ceil((powerTextSize.Y - 30.0f) / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), powerText, powerPosition, color);

        // draw the quantity
        drawPosition.X += (float)ChestQuantityColumnInterval;
        if (isSelected) {
            // draw the left selection arrow
            drawPosition.X -= (float)leftQuantityArrow_.getWidthProperty();
            spriteBatch.Draw(leftQuantityArrow_, Vector2(drawPosition.X, drawPosition.Y - 4.0f),
                             Color::White);
            drawPosition.X += (float)leftQuantityArrow_.getWidthProperty();
            // draw the selected quantity ratio
            std::string quantityText = System::Int32::ToString(selectedQuantity_) + "/" +
                                       System::Int32::ToString(entry->Count);
            spriteBatch.DrawString(Fonts::GearInfoFont(), quantityText, drawPosition, color);
            drawPosition.X += Fonts::GearInfoFont().MeasureString(quantityText).X;
            // draw the right selection arrow
            spriteBatch.Draw(rightQuantityArrow_, Vector2(drawPosition.X, drawPosition.Y - 4.0f),
                             Color::White);
            drawPosition.X += (float)rightQuantityArrow_.getWidthProperty();
        } else {
            // draw the remaining quantity
            spriteBatch.DrawString(Fonts::GearInfoFont(), System::Int32::ToString(entry->Count),
                                   drawPosition, color);
        }
    }

private:
    // Resets the selected quantity to the maximum value for the selected entry.
    void ResetSelectedQuantity() {
        // safety-check on the chest
        if (chestEntry_ == nullptr || chestEntry_->Content == nullptr) {
            selectedQuantity_ = 0;
            return;
        }

        // set the quantity to the maximum
        if (IsGoldSelected()) {
            selectedQuantity_ = chestEntry_->Content->Gold;
        } else if (SelectedGearIndex() >= 0 &&
                   SelectedGearIndex() < (int)chestEntry_->Content->Entries.size()) {
            selectedQuantity_ =
                chestEntry_->Content->Entries[(std::size_t)SelectedGearIndex()]->Count;
        }
    }

    void RemoveEmptyEntries() {
        auto& entries = chestEntry_->Content->Entries;
        entries.erase(std::remove_if(entries.begin(), entries.end(),
                                     [](const std::shared_ptr<ContentEntry<Gear>>& contentEntry) {
                                         return contentEntry->Count <= 0;
                                     }),
                      entries.end());
    }

    // The left-facing quantity arrow.
    Texture2D leftQuantityArrow_;

    // The right-facing quantity arrow.
    Texture2D rightQuantityArrow_;

    static constexpr int ChestNameColumnInterval = 80;
    static constexpr int ChestPowerColumnInterval = 270;
    static constexpr int ChestQuantityColumnInterval = 450;

    // The chest entry that is displayed here.
    std::shared_ptr<MapEntry<Chest>> chestEntry_;

    // The selected quantity of the current entry.
    int selectedQuantity_ = 0;
};

} // namespace RolePlaying
