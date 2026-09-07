#pragma once

// QuestLogScreen.hpp -- C++ port of GameScreens/QuestLogScreen.cs.

#include <memory>
#include <string>
#include <vector>

#include "System/ArgumentNullException.hpp"

#include "../Data/Quests/Quest.hpp"
#include "../Session/Session.hpp"
#include "ListScreen.hpp"
#include "QuestDetailsScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Quest;

class InventoryScreen;  // fwd decl -- constructed in PageScreenLeft
class StatisticsScreen; // fwd decl -- constructed in PageScreenRight

// Displays all of the quests completed by the party.
class QuestLogScreen : public ListScreen<std::shared_ptr<Quest>> {
public:
    // Creates a new QuestLogScreen object, optionally showing one quest's details at once.
    explicit QuestLogScreen(const std::shared_ptr<Quest>& initialDetailQuest)
        : initialDetailQuest_(initialDetailQuest) {
        // configure the menu text
        titleText_ = Session::GetQuestLine()->Name;
        selectButtonText_ = "Select";
        backButtonText_ = "Back";
        xButtonText_.clear();
        yButtonText_.clear();
        leftTriggerText_ = "Equipment";
        rightTriggerText_ = "Statistics";

        // select the current quest
        SetSelectedIndex(Session::CurrentQuestIndex());
    }

    // Get the list that this screen displays.
    const std::vector<std::shared_ptr<Quest>>& GetDataList() override {
        quests_.clear();
        for (int i = 0; i <= Session::CurrentQuestIndex(); i++) {
            if (i < (int)Session::GetQuestLine()->Quests.size()) {
                quests_.push_back(Session::GetQuestLine()->Quests[(std::size_t)i]);
            }
        }

        return quests_;
    }

    void HandleInput() override {
        // open the initial QuestDetailScreen, if any
        // -- this is the first opportunity to add another screen
        if (initialDetailQuest_ != nullptr) {
            GetScreenManager()->AddScreen(std::make_shared<QuestDetailsScreen>(initialDetailQuest_));
            // if the selected quest is in the list, make sure it's visible
            const auto& quests = Session::GetQuestLine()->Quests;
            for (int i = 0; i < (int)quests.size(); i++) {
                if (quests[(std::size_t)i] == initialDetailQuest_) {
                    SetSelectedIndex(i);
                    break;
                }
            }
            // only open the screen once
            initialDetailQuest_ = nullptr;
        }

        ListScreen<std::shared_ptr<Quest>>::HandleInput();
    }

protected:
    // Respond to the triggering of the Select action (and related key).
    void SelectTriggered(const std::shared_ptr<Quest>& entry) override {
        GetScreenManager()->AddScreen(std::make_shared<QuestDetailsScreen>(entry));
    }

    // Switch to the screen to the "left" of this one in the UI.
    // Defined out-of-line -- it constructs an InventoryScreen.
    void PageScreenLeft() override;

    // Switch to the screen to the "right" of this one in the UI.
    // Defined out-of-line -- it constructs a StatisticsScreen.
    void PageScreenRight() override;

    // Draw the quest at the given position in the list.
    void DrawEntry(const std::shared_ptr<Quest>& entry, const Vector2& position,
                   bool isSelected) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;

        // draw the name
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)NameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), entry->Name, drawPosition, color);

        // draw the stage
        drawPosition.X += (float)StageColumnInterval;
        std::string stageText;
        switch (entry->Stage) {
        case Quest::QuestStage::Completed:
            stageText = "Completed";
            break;

        case Quest::QuestStage::InProgress:
            stageText = "In Progress";
            break;

        case Quest::QuestStage::NotStarted:
            stageText = "Not Started";
            break;

        case Quest::QuestStage::RequirementsMet:
            stageText = "Requirements Met";
            break;
        }
        spriteBatch.DrawString(Fonts::GearInfoFont(), stageText, drawPosition, color);

        // turn on or off the select button
        if (isSelected) {
            selectButtonText_ = "Select";
        }
    }

    // Draw the description of the selected item.
    void DrawSelectedDescription(const std::shared_ptr<Quest>& entry) override { (void)entry; }

    // Draw the column headers above the list.
    void DrawColumnHeaders() override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position = listEntryStartPosition_;

        position.X += (float)NameColumnInterval;
        if (!nameColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), nameColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)StageColumnInterval;
        if (!stageColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), stageColumnText_, position,
                                   Fonts::CaptionColor);
        }
    }

    std::string nameColumnText_ = "Name";
    static constexpr int NameColumnInterval = 20;

    std::string stageColumnText_ = "Stage";
    static constexpr int StageColumnInterval = 450;

private:
    // The quest that is shown when the screen is created. Stored because new screens can't be
    // added until the first update.
    std::shared_ptr<Quest> initialDetailQuest_;

    std::vector<std::shared_ptr<Quest>> quests_;
};

} // namespace RolePlaying
