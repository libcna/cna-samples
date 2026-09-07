#pragma once

// QuestNpcScreen.hpp -- C++ port of GameScreens/QuestNpcScreen.cs.

#include <memory>

#include "System/ArgumentException.hpp"

#include "../Data/Characters/QuestNpc.hpp"
#include "../Data/Quests/Quest.hpp"
#include "../Session/Session.hpp"
#include "../TileEngine/TileEngine.hpp"
#include "NpcScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Quest;
using RolePlayingGameData::QuestNpc;

// Displays the quest NPC screen, shown when encountering a quest NPC on the map.
class QuestNpcScreen : public NpcScreen<QuestNpc> {
public:
    // Constructs a new QuestNpcScreen object.
    explicit QuestNpcScreen(const std::shared_ptr<MapEntry<QuestNpc>>& mapEntry)
        : NpcScreen<QuestNpc>(mapEntry) {
        // assign and check the parameter
        std::shared_ptr<QuestNpc> questNpc = std::dynamic_pointer_cast<QuestNpc>(character_);
        if (questNpc == nullptr) {
            throw System::ArgumentException(
                "QuestNpcScreen requires a MapEntry with a QuestNpc");
        }

        // check to see if this NPC is the current quest destination
        Quest* quest = Session::GetQuest();
        if (quest != nullptr && quest->Stage == Quest::QuestStage::RequirementsMet &&
            EndsWith(TileEngine::Map()->AssetName(), quest->DestinationMapContentName) &&
            quest->DestinationNpcContentName == mapEntry->ContentName) {
            // use the quest completion dialog
            SetDialogueText(quest->CompletionMessage);
            // mark the quest for completion
            // -- the session will not update until the pop-up screens are cleared
            quest->Stage = Quest::QuestStage::Completed;
        } else {
            // this NPC is not the destination, so use the npc's welcome text
            SetDialogueText(questNpc->IntroductionDialogue);
        }
    }

private:
    // C#'s string.EndsWith, which the destination test uses.
    static bool EndsWith(const std::string& value, const std::string& suffix) {
        if (suffix.empty()) return false;
        if (suffix.size() > value.size()) return false;
        return value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
};

} // namespace RolePlaying
