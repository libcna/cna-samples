#pragma once

// StoreBuyScreen.hpp -- C++ port of GameScreens/StoreBuyScreen.cs.

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../Data/Gear/Equipment.hpp"
#include "../Data/Gear/Gear.hpp"
#include "../Data/Map/Store.hpp"
#include "../Session/Session.hpp"
#include "ListScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Equipment;
using RolePlayingGameData::Gear;
using RolePlayingGameData::Store;

// Displays the gear from a particular store and allows the user to purchase them.
class StoreBuyScreen : public ListScreen<std::shared_ptr<Gear>> {
public:
    // Creates a new StoreBuyScreen object for the given store.
    explicit StoreBuyScreen(const std::shared_ptr<Store>& store) : store_(store) {
        // check the parameter
        if (store == nullptr || store->StoreCategories.empty()) {
            throw System::ArgumentNullException("store");
        }

        // configure the menu text
        selectButtonText_ = "Purchase";
        backButtonText_ = "Back";
        xButtonText_.clear();
        yButtonText_.clear();
        ResetMenu();

        ResetQuantities();
    }

    // Get the list that this screen displays.
    const std::vector<std::shared_ptr<Gear>>& GetDataList() override {
        return store_->StoreCategories[(std::size_t)currentCategoryIndex_].AvailableGear;
    }

    void LoadContent() override {
        ListScreen<std::shared_ptr<Gear>>::LoadContent();

        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        leftQuantityArrow_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowLeft");
        rightQuantityArrow_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowRight");
    }

protected:
    // Move the current selection up one entry.
    void MoveCursorUp() override {
        int oldIndex = SelectedIndex();
        ListScreen<std::shared_ptr<Gear>>::MoveCursorUp();
        if (SelectedIndex() != oldIndex) {
            ResetQuantities();
        }
    }

    // Move the current selection down one entry.
    void MoveCursorDown() override {
        int oldIndex = SelectedIndex();
        ListScreen<std::shared_ptr<Gear>>::MoveCursorDown();
        if (SelectedIndex() != oldIndex) {
            ResetQuantities();
        }
    }

    // Decrease the selected quantity by one.
    void MoveCursorLeft() override {
        if (maximumQuantity_ > 0) {
            // decrement the quantity, looping around if necessary
            selectedQuantity_ = (selectedQuantity_ > 1) ? selectedQuantity_ - 1 : maximumQuantity_;
        }
    }

    // Increase the selected quantity by one.
    void MoveCursorRight() override {
        if (maximumQuantity_ > 0) {
            // loop to one if the selected quantity is already at maximum.
            selectedQuantity_ =
                selectedQuantity_ < maximumQuantity_ ? selectedQuantity_ + 1 : 1;
        }
    }

    // Respond to the triggering of the Select action.
    void SelectTriggered(const std::shared_ptr<Gear>& entry) override {
        if (entry == nullptr) {
            return;
        }

        // purchase the items if possible
        int totalPrice = (int)std::floor((float)entry->GoldValue * (float)selectedQuantity_ *
                                          store_->BuyMultiplier);
        if (totalPrice <= Session::GetParty()->PartyGold()) {
            Session::GetParty()->SetPartyGold(Session::GetParty()->PartyGold() - totalPrice);
            Session::GetParty()->AddToInventory(entry, selectedQuantity_);
        }

        // reset the quantities - either gold has gone down or the total was bad
        ResetQuantities();
    }

    // Switch to the previous store category.
    void PageScreenLeft() override {
        currentCategoryIndex_--;
        if (currentCategoryIndex_ < 0) {
            currentCategoryIndex_ = (int)store_->StoreCategories.size() - 1;
        }
        ResetMenu();
        ResetQuantities();
    }

    // Switch to the next store category.
    void PageScreenRight() override {
        currentCategoryIndex_++;
        if (currentCategoryIndex_ >= (int)store_->StoreCategories.size()) {
            currentCategoryIndex_ = 0;
        }
        ResetMenu();
        ResetQuantities();
    }

    // Draw the entry at the given position in the list.
    void DrawEntry(const std::shared_ptr<Gear>& entry, const Vector2& position,
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

        // draw the quantity
        drawPosition.X += (float)QuantityColumnInterval;
        int priceSingle = (int)std::floor((float)entry->GoldValue * store_->BuyMultiplier);
        if (isSelected) {
            if (priceSingle <= Session::GetParty()->PartyGold()) {
                Vector2 quantityPosition = drawPosition;
                // draw the left selection arrow
                quantityPosition.X -= (float)leftQuantityArrow_.getWidthProperty();
                spriteBatch.Draw(leftQuantityArrow_,
                                 Vector2(quantityPosition.X, quantityPosition.Y - 4.0f),
                                 Color::White);
                quantityPosition.X += (float)leftQuantityArrow_.getWidthProperty();
                // draw the selected quantity ratio
                std::string quantityText = System::Int32::ToString(selectedQuantity_) + "/" +
                                           System::Int32::ToString(maximumQuantity_);
                spriteBatch.DrawString(Fonts::GearInfoFont(), quantityText, quantityPosition,
                                       color);
                quantityPosition.X += Fonts::GearInfoFont().MeasureString(quantityText).X;
                // draw the right selection arrow
                spriteBatch.Draw(rightQuantityArrow_,
                                 Vector2(quantityPosition.X, quantityPosition.Y - 4.0f),
                                 Color::White);
                quantityPosition.X += (float)rightQuantityArrow_.getWidthProperty();
                // draw the purchase button
                selectButtonText_ = "Purchase";
            } else {
                // turn off the purchase button
                selectButtonText_.clear();
            }
        }

        // draw the price
        drawPosition.X += (float)PriceColumnInterval;
        std::string priceText;
        if (isSelected) {
            int totalPrice =
                (int)std::floor((float)entry->GoldValue * store_->BuyMultiplier) * selectedQuantity_;
            priceText = System::Int32::ToString(totalPrice);
        } else {
            priceText = System::Int32::ToString(
                (int)std::floor((float)entry->GoldValue * store_->BuyMultiplier));
        }
        spriteBatch.DrawString(Fonts::GearInfoFont(), priceText, drawPosition, color);
    }

    // Draw the description of the selected item.
    void DrawSelectedDescription(const std::shared_ptr<Gear>& entry) override {
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
        auto equipment = std::dynamic_pointer_cast<Equipment>(entry);
        if (equipment != nullptr) {
            text = equipment->OwnerBuffStatistics.GetModifierString();
            if (!text.empty()) {
                spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                       Fonts::DescriptionColor);
                position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
            }
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

        position.X += (float)QuantityColumnInterval;
        if (!quantityColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), quantityColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)PriceColumnInterval;
        if (!priceColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), priceColumnText_, position,
                                   Fonts::CaptionColor);
        }
    }

private:
    // Resets the selected quantity to the maximum value for the selected entry.
    void ResetQuantities() {
        // check the indices before recalculating
        if (currentCategoryIndex_ < 0 ||
            currentCategoryIndex_ > (int)store_->StoreCategories.size() || SelectedIndex() < 0 ||
            SelectedIndex() >= (int)store_->StoreCategories[(std::size_t)currentCategoryIndex_]
                                   .AvailableGear.size()) {
            return;
        }

        // get the value of the selected gear
        const std::shared_ptr<Gear>& gear =
            store_->StoreCategories[(std::size_t)currentCategoryIndex_]
                .AvailableGear[(std::size_t)SelectedIndex()];
        if (gear == nullptr || gear->GoldValue <= 0) {
            selectedQuantity_ = maximumQuantity_ = 0;
        }

        selectedQuantity_ = 1;
        maximumQuantity_ = (int)std::floor((float)Session::GetParty()->PartyGold() /
                                            ((float)gear->GoldValue * store_->BuyMultiplier));
    }

    // Reset the menu title and trigger button text for the current store category.
    void ResetMenu() {
        // update the title
        titleText_ = store_->StoreCategories[(std::size_t)currentCategoryIndex_].Name;

        // get the left trigger text
        int index = currentCategoryIndex_ - 1;
        if (index < 0) {
            index = (int)store_->StoreCategories.size() - 1;
        }
        leftTriggerText_ = store_->StoreCategories[(std::size_t)index].Name;

        // get the right trigger text
        index = currentCategoryIndex_ + 1;
        if (index >= (int)store_->StoreCategories.size()) {
            index = 0;
        }
        rightTriggerText_ = store_->StoreCategories[(std::size_t)index].Name;
    }

    // The left-facing quantity arrow.
    Texture2D leftQuantityArrow_;

    // The right-facing quantity arrow.
    Texture2D rightQuantityArrow_;

    std::string nameColumnText_ = "Name";
    static constexpr int NameColumnInterval = 80;

    std::string powerColumnText_ = "Power (min, max)";
    static constexpr int PowerColumnInterval = 270;

    std::string quantityColumnText_ = "Qty";
    static constexpr int QuantityColumnInterval = 340;

    std::string priceColumnText_ = "Price";
    static constexpr int PriceColumnInterval = 120;

    // The store whose goods are being displayed.
    std::shared_ptr<Store> store_;

    // The index of the current StoreCategory.
    int currentCategoryIndex_ = 0;

    // The selected quantity of the current entry.
    int selectedQuantity_ = 0;

    // The maximum quantity of the current entry.
    int maximumQuantity_ = 0;
};

} // namespace RolePlaying
