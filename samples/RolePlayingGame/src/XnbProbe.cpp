// TEMPORARY audit probe for SAMPLE-070 -- not part of the port, delete before committing.
//
// Loads one real .xnb produced by the unchanged official content pipeline through ContentManager,
// using the sample's own ported ContentTypeReaders, and prints what came back. It exists to prove
// the reader mechanism end to end before the rest of the data layer is written against it.

#include <algorithm>
#include <filesystem>
#include <system_error>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"

#include <string>

#include "Data/Characters/CharacterClass.hpp"
#include "Data/Characters/Monster.hpp"
#include "Data/Characters/Player.hpp"
#include "Data/Gear/Weapon.hpp"
#include "Data/Map/Inn.hpp"
#include "Data/Map/Map.hpp"
#include "Data/Map/Store.hpp"
#include "Data/Quests/Quest.hpp"
#include "Data/Quests/QuestLine.hpp"
#include "Data/Characters/QuestNpc.hpp"
#include "Data/Spell.hpp"
#include "Data/RolePlayingGameDataContentReaders.hpp"

using namespace Microsoft::Xna::Framework;
using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;

namespace {

class XnbProbeGame final : public Game {
public:
    XnbProbeGame() : graphics_(std::make_unique<GraphicsDeviceManager>(this)) {
        getContentProperty().setRootDirectoryProperty(
            "/rv/tmp/samples/SAMPLE-070-RolePlayingGame_4_0_Win_Xbox/"
            "xna4-build/windows-hidef-compressed/Content");

        RolePlayingGameData::RegisterContentTypeReaders();
    }

    int Result() const noexcept { return result_; }

    // Loads every .xnb in one content directory as T and reports how many came back.
    template <typename T>
    void SweepDirectory(const std::string& directory) {
        const std::string root = getContentProperty().getRootDirectoryProperty();
        std::vector<std::string> names;
        std::error_code ec;
        for (const auto& entry :
             std::filesystem::directory_iterator(root + "/" + directory, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".xnb") {
                names.push_back(entry.path().stem().string());
            }
        }
        std::sort(names.begin(), names.end());
        int loaded = 0;
        std::string firstError;
        for (const std::string& name : names) {
            if (std::getenv("CNA_PROBE_TRACE") != nullptr) {
                std::fprintf(stderr, "loading %s/%s\n", directory.c_str(), name.c_str());
                std::fflush(stderr);
            }
            try {
                if (getContentProperty().Load<std::shared_ptr<T>>(directory + "/" + name) !=
                    nullptr) {
                    ++loaded;
                } else if (firstError.empty()) {
                    firstError = name + " -> null";
                }
            } catch (const std::exception& error) {
                if (firstError.empty()) firstError = name + " -> " + error.what();
            }
        }
        if (loaded == static_cast<int>(names.size()) && !names.empty()) {
            std::printf("[SWEEP] %-24s %3d/%zu\n", directory.c_str(), loaded, names.size());
        } else {
            std::fprintf(stderr, "[SWEEP-FAIL] %-24s %d/%zu  first: %s\n", directory.c_str(),
                         loaded, names.size(), firstError.c_str());
            result_ = 1;
        }
    }

    // Loads one asset and prints what came back; a throw or a null is a failure, not a warning.
    template <typename T, typename Describe>
    void LoadOne(const std::string& assetName, Describe describe) {
        try {
            auto value = getContentProperty().Load<std::shared_ptr<T>>(assetName);
            if (value == nullptr) {
                std::fprintf(stderr, "[FAIL] %s -> null\n", assetName.c_str());
                result_ = 1;
                return;
            }
            std::printf("[PASS] %s: %s\n", assetName.c_str(), describe(value).c_str());
        } catch (const std::exception& error) {
            std::fprintf(stderr, "[FAIL] %s -> %s\n", assetName.c_str(), error.what());
            result_ = 1;
        }
    }

protected:
    void Initialize() override {
        Game::Initialize();
        try {
            auto inn = getContentProperty().Load<std::shared_ptr<RolePlayingGameData::Inn>>(
                "Maps/Inns/HealingInn");
            if (inn == nullptr) {
                std::fprintf(stderr, "[FAIL] Load returned null\n");
            } else {
                std::printf("[PASS] asset=%s name=%s charge=%d\n", inn->AssetName().c_str(),
                            inn->Name().c_str(), inn->ChargePerPlayer);
                std::printf("       welcome=%s\n", inn->WelcomeMessage.c_str());
                std::printf("       paid=%s\n", inn->PaidMessage.c_str());
                std::printf("       poor=%s\n", inn->NotEnoughGoldMessage.c_str());
                std::printf("       portrait=%s texture=%s\n", inn->ShopkeeperTextureName.c_str(),
                            inn->ShopkeeperTexture ? "loaded" : "NULL");
                result_ = (inn->ShopkeeperTexture != nullptr) ? 0 : 1;
            }
        } catch (const std::exception& error) {
            std::fprintf(stderr, "[FAIL] %s\n", error.what());
        }

        // One real asset per reader family that has landed, so registration is never mistaken for
        // working: each is loaded from the unchanged official output and its own fields checked.
        // Loaded as Gear, the base every gear reader targets, then narrowed -- the same shape the
        // sample's own call sites use.
        LoadOne<RolePlayingGameData::Gear>("Gear/Weapons/PersianSword", [](const auto& g) {
            const auto w = std::dynamic_pointer_cast<RolePlayingGameData::Weapon>(g);
            if (!w) return std::string("NOT A WEAPON");
            return w->Name + " dmg=" + std::to_string(w->TargetDamageRange.Minimum) + "-" +
                   std::to_string(w->TargetDamageRange.Maximum) +
                   " overlay=" + (w->Overlay && w->Overlay->Texture ? "yes" : "NO") +
                   " icon=" + (w->IconTexture ? "yes" : "NO");
        });

        LoadOne<RolePlayingGameData::CharacterClass>("CharacterClasses/Wizard", [](const auto& c) {
            std::string spells;
            for (const auto& entry : c->LevelEntries) {
                for (const auto& spell : entry.Spells) {
                    if (!spells.empty()) spells += ",";
                    spells += spell->Name;
                }
            }
            return c->Name + " hp=" + std::to_string(c->InitialStatistics.HealthPoints) +
                   " levels=" + std::to_string(c->LevelEntries.size()) + " spells=[" + spells + "]";
        });

        LoadOne<RolePlayingGameData::Player>("Characters/Players/Kolatt", [](const auto& p) {
            return p->Name() + " lvl=" + std::to_string(p->CharacterLevel()) +
                   " gold=" + std::to_string(p->Gold) +
                   " class=" + (p->GetCharacterClass() ? p->GetCharacterClass()->Name : "NONE") +
                   " inventory=" + std::to_string(p->Inventory.size()) +
                   " portraits=" + (p->ActivePortraitTexture && p->InactivePortraitTexture &&
                                    p->UnselectablePortraitTexture ? "3" : "INCOMPLETE") +
                   " shadow=" + (p->ShadowTexture ? "yes" : "NO");
        });
        LoadOne<RolePlayingGameData::Monster>("Characters/Monsters/BanditGuard", [](const auto& m) {
            return m->Name() + " lvl=" + std::to_string(m->CharacterLevel()) +
                   " defend=" + std::to_string(m->DefendPercentage) +
                   " drops=" + std::to_string(m->GearDrops.size());
        });

        // A whole map: the deepest graph this content has -- layers, portals, chests, fixed and
        // random combats, quest NPCs, player NPCs, inns and stores, each resolving its own assets.
        LoadOne<RolePlayingGameData::Map>("Maps/Map001", [](const auto& m) {
            return m->Name + " dim=" + std::to_string(m->MapDimensions.X) + "x" +
                   std::to_string(m->MapDimensions.Y) +
                   " tiles=" + std::to_string(m->BaseLayer.size()) +
                   " portals=" + std::to_string(m->Portals.size()) +
                   " chests=" + std::to_string(m->ChestEntries.size()) +
                   " fixed=" + std::to_string(m->FixedCombatEntries.size()) +
                   " npcs=" + std::to_string(m->QuestNpcEntries.size()) +
                   " inns=" + std::to_string(m->InnEntries.size()) +
                   " stores=" + std::to_string(m->StoreEntries.size()) +
                   " texture=" + (m->Texture ? "yes" : "NO");
        });

        // The map reader uses the original's spelling, Characters\\QuestNpcs, while the content
        // directory is QuestNPCs. Windows resolves that; check what CNA does.
        LoadOne<RolePlayingGameData::QuestNpc>("Characters/QuestNpcs/Sheddarr",
                                               [](const auto& n) { return n->Name(); });
        LoadOne<RolePlayingGameData::QuestNpc>("Characters/QuestNPCs/Sheddarr",
                                               [](const auto& n) { return n->Name(); });

        // The corpus sweep: every asset of every top-level kind the game loads by name, so a
        // reader that is registered but wrong cannot hide behind a sample of three.
        SweepDirectory<RolePlayingGameData::Map>("Maps");
        SweepDirectory<RolePlayingGameData::Quest>("Quests");
        SweepDirectory<RolePlayingGameData::QuestLine>("Quests/QuestLines");
        SweepDirectory<RolePlayingGameData::CharacterClass>("CharacterClasses");
        SweepDirectory<RolePlayingGameData::Player>("Characters/Players");
        SweepDirectory<RolePlayingGameData::Monster>("Characters/Monsters");
        SweepDirectory<RolePlayingGameData::QuestNpc>("Characters/QuestNPCs");
        SweepDirectory<RolePlayingGameData::Gear>("Gear/Armor");
        SweepDirectory<RolePlayingGameData::Gear>("Gear/Weapons");
        SweepDirectory<RolePlayingGameData::Gear>("Gear/Items");
        SweepDirectory<RolePlayingGameData::Spell>("Spells");
        SweepDirectory<RolePlayingGameData::Chest>("Maps/Chests");
        SweepDirectory<RolePlayingGameData::Inn>("Maps/Inns");
        SweepDirectory<RolePlayingGameData::Store>("Maps/Stores");
        SweepDirectory<RolePlayingGameData::FixedCombat>("Maps/FixedCombats");

        // The definitive checklist for this port: every reader name the official .xnb corpus
        // records, how many files need it, and whether this sample has one yet.
        auto usage = getContentProperty().GetXnbReaderUsageSummary();
        std::sort(usage.begin(), usage.end(), [](const auto& a, const auto& b) {
            return a.fileCount > b.fileCount;
        });
        int missing = 0, missingFiles = 0;
        for (const auto& row : usage) {
            if (!row.isRegistered) { ++missing; missingFiles += row.fileCount; }
        }
        std::printf("\n[READERS] %zu distinct, %d unregistered (used by %d files)\n",
                    usage.size(), missing, missingFiles);
        for (const auto& row : usage) {
            if (row.isRegistered) continue;
            std::printf("  %5d  %s\n", row.fileCount, row.readerName.c_str());
        }
        Exit();
    }

private:
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    int result_ = 1;
};

} // namespace

int main() {
    XnbProbeGame game;
    game.Run();
    return game.Result();
}
