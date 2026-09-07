#pragma once

// RolePlayingGame.hpp -- C++ port of RolePlayingGame.cs (the Game subclass)
// plus the final cross-referencing method bodies for Session, CombatEngine,
// TileEngine, and Party that could not be defined in their own headers due to
// mutual dependencies (Session <-> CombatEngine <-> screens, TileEngine <->
// Session, Party <-> Session/LevelUpScreen) -- consolidated here exactly like
// every other multi-file ScreenManager port in this repo (see e.g.
// NinjAcademy's/CardsStarterKit's "cross-referencing method definitions").
//
// AudioManager.Initialize in the original also wires up a GamerServicesComponent
// and an XACT AudioEngine from a compiled .xgs/.xwb/.xsb project; neither
// exists in this port (see AudioManager.hpp/missing.md) so those two lines are
// dropped.

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "AudioManager.hpp"
#include "Combat/ArtificialIntelligence.hpp"
#include "Combat/CombatEngine.hpp"
#include "Combat/Actions/DefendCombatAction.hpp"
#include "Combat/Actions/ItemCombatAction.hpp"
#include "Combat/Actions/MeleeCombatAction.hpp"
#include "Combat/Actions/SpellCombatAction.hpp"
#include "Data/RolePlayingGameDataContentReaders.hpp"
#include "Fonts.hpp"
#include "GameScreens/ChestScreen.hpp"
#include "GameScreens/DialogueScreen.hpp"
#include "GameScreens/GameOverScreen.hpp"
#include "GameScreens/EquipmentScreen.hpp"
#include "GameScreens/GameplayScreen.hpp"
#include "GameScreens/Hud.hpp"
#include "GameScreens/InnScreen.hpp"
#include "GameScreens/LevelUpScreen.hpp"
#include "GameScreens/PlayerNpcScreen.hpp"
#include "GameScreens/InventoryScreen.hpp"
#include "GameScreens/ListScreen.hpp"
#include "GameScreens/PlayerSelectionScreen.hpp"
#include "GameScreens/QuestDetailsScreen.hpp"
#include "GameScreens/QuestLogScreen.hpp"
#include "GameScreens/QuestNpcScreen.hpp"
#include "GameScreens/RewardsScreen.hpp"
#include "GameScreens/SpellbookScreen.hpp"
#include "GameScreens/StatisticsScreen.hpp"
#include "GameScreens/StoreBuyScreen.hpp"
#include "GameScreens/StoreScreen.hpp"
#include "GameScreens/StoreSellScreen.hpp"
#include "InputManager.hpp"
#include "MenuScreens/ControlsScreen.hpp"
#include "MenuScreens/HelpScreen.hpp"
#include "MenuScreens/MainMenuScreen.hpp"
#include "MenuScreens/MessageBoxScreen.hpp"
#include "MenuScreens/SaveLoadScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Session/Session.hpp"
#include "Session/SessionSaveLoad.hpp"
#include "TileEngine/TileEngine.hpp"

namespace RolePlaying {

class RolePlayingGame : public Microsoft::Xna::Framework::Game {
public:
    RolePlayingGame() {
        // initialize the graphics system
        graphics_ = std::make_unique<Microsoft::Xna::Framework::GraphicsDeviceManager>(this);
        graphics_->setPreferredBackBufferWidthProperty(1280);
        graphics_->setPreferredBackBufferHeightProperty(720);

        // configure the content manager
        getContentProperty().setRootDirectoryProperty("Content");

        // add a gamer-services component, which is required for the storage APIs
        gamerServicesComponent_ =
            std::make_shared<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent>(
                *this);
        getComponentsProperty().Add(gamerServicesComponent_.get());

        // CNAEXT -- not a line the original needs. Every .xnb names its reader by the
        // assembly-qualified type name XNA resolves with reflection over
        // RolePlayingGameDataWindows; C++ has none, so the game says once which reader answers
        // to each name. The readers themselves are the original's own. See diff.md.
        CNAEXT RolePlayingGameData::RegisterContentTypeReaders();

        // add the audio manager
        AudioManager::Initialize(*this, "Content/Audio/RPGAudio.xgs",
                                 "Content/Audio/Wave Bank.xwb", "Content/Audio/Sound Bank.xsb");

        // add the screen manager
        screenManager_ = std::make_shared<ScreenManager>(*this);
        getComponentsProperty().Add(screenManager_.get());
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "RolePlayingGame";
        return name;
    }

protected:
    void Initialize() override {
        InputManager::Initialize();

        Game::Initialize();

        TileEngine::SetViewport(getGraphicsDeviceProperty().getViewportProperty());

        screenManager_->AddScreen(std::make_shared<MainMenuScreen>());
    }

    void LoadContent() override {
        Fonts::LoadContent(getContentProperty());
        Game::LoadContent();

    }

    void UnloadContent() override {
        Fonts::UnloadContent();
        Game::UnloadContent();
    }

    void Update(GameTime& gameTime) override {
        InputManager::Update();

        Game::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Microsoft::Xna::Framework::Color(0, 0, 0, 0));
        Game::Draw(gameTime);
    }

private:
    std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics_;
    std::shared_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent>
        gamerServicesComponent_;
    std::shared_ptr<ScreenManager> screenManager_;
};

// ============================================================================
// Cross-referencing definitions
// ============================================================================

// ---- Session methods that depend on CombatEngine/screens ----

inline bool Session::EncounterTile(Microsoft::Xna::Framework::Point mapPosition) {
    if (singleton_->quest_ &&
        (singleton_->quest_->Stage == Quest::QuestStage::InProgress ||
         singleton_->quest_->Stage == Quest::QuestStage::RequirementsMet)) {
        for (auto& entry : singleton_->quest_->FixedCombatEntries) {
            if (EndsWith(TileEngine::Map()->AssetName(), entry->MapContentName) && entry->MapPosition == mapPosition) {
                EncounterFixedCombat(entry);
                return true;
            }
        }
    }
    for (auto& entry : TileEngine::Map()->FixedCombatEntries) {
        if (entry->MapPosition == mapPosition) { EncounterFixedCombat(entry); return true; }
    }
    if (singleton_->quest_) {
        for (auto& entry : singleton_->quest_->ChestEntries) {
            if (EndsWith(TileEngine::Map()->AssetName(), entry->MapContentName) && entry->MapPosition == mapPosition) {
                EncounterChest(entry);
                return true;
            }
        }
    }
    for (auto& entry : TileEngine::Map()->ChestEntries) {
        if (entry->MapPosition == mapPosition) { EncounterChest(entry); return true; }
    }
    for (auto& entry : TileEngine::Map()->PlayerNpcEntries) {
        if (entry->MapPosition == mapPosition) { EncounterPlayerNpc(entry); return true; }
    }
    for (auto& entry : TileEngine::Map()->QuestNpcEntries) {
        if (entry->MapPosition == mapPosition) { EncounterQuestNpc(entry); return true; }
    }
    for (auto& entry : TileEngine::Map()->PortalEntries) {
        if (entry->MapPosition == mapPosition) { EncounterPortal(entry); return true; }
    }
    for (auto& entry : TileEngine::Map()->InnEntries) {
        if (entry->MapPosition == mapPosition) { EncounterInn(entry); return true; }
    }
    for (auto& entry : TileEngine::Map()->StoreEntries) {
        if (entry->MapPosition == mapPosition) { EncounterStore(entry); return true; }
    }
    return false;
}

inline void Session::EncounterFixedCombat(const std::shared_ptr<MapEntry<FixedCombat>>& fixedCombatEntry) {
    if (!fixedCombatEntry || !fixedCombatEntry->Content) return;
    if (!CombatEngine::IsActive()) CombatEngine::StartNewCombat(fixedCombatEntry);
}

inline void Session::EncounterChest(const std::shared_ptr<MapEntry<Chest>>& chestEntry) {
    if (!chestEntry || !chestEntry->Content) return;
    singleton_->screenManager_->AddScreen(std::make_shared<ChestScreen>(chestEntry));
}

inline void Session::EncounterPlayerNpc(const std::shared_ptr<MapEntry<Player>>& playerEntry) {
    if (!playerEntry || !playerEntry->Content) return;
    singleton_->screenManager_->AddScreen(std::make_shared<PlayerNpcScreen>(playerEntry));
}

inline void Session::EncounterQuestNpc(const std::shared_ptr<MapEntry<QuestNpc>>& questNpcEntry) {
    if (!questNpcEntry || !questNpcEntry->Content) return;
    singleton_->screenManager_->AddScreen(std::make_shared<QuestNpcScreen>(questNpcEntry));
}

inline void Session::EncounterInn(const std::shared_ptr<MapEntry<Inn>>& innEntry) {
    if (!innEntry || !innEntry->Content) return;
    singleton_->screenManager_->AddScreen(std::make_shared<InnScreen>(innEntry->Content));
}

inline void Session::EncounterStore(const std::shared_ptr<MapEntry<Store>>& storeEntry) {
    if (!storeEntry || !storeEntry->Content) return;
    singleton_->screenManager_->AddScreen(std::make_shared<StoreScreen>(storeEntry->Content));
}

inline bool Session::CheckForRandomCombat(const std::shared_ptr<RandomCombat>& randomCombat) {
    if (!randomCombat || randomCombat->CombatProbability <= 0) return false;
    if (CombatEngine::IsActive()) return false;
    if ((int)random_.Next(0, 100) < randomCombat->CombatProbability) {
        CombatEngine::StartNewCombat(randomCombat);
        return true;
    }
    return false;
}

inline void Session::UpdateQuest() {
    if (!party_ || !questLine_) return;

    if (!quest_ && !questLine_->Quests.empty() && !IsQuestLineComplete()) {
        quest_ = questLine_->Quests[currentQuestIndex_];
        quest_->Stage = Quest::QuestStage::NotStarted;
        party_->ClearMonsterKills();
        modifiedQuestChests_.clear();
        removedQuestChests_.clear();
        removedQuestFixedCombats_.clear();
    }

    if (quest_ && !IsQuestLineComplete()) {
        switch (quest_->Stage) {
            case Quest::QuestStage::NotStarted:
                quest_->Stage = Quest::QuestStage::InProgress;
                if (!quest_->AreRequirementsMet()) screenManager_->AddScreen(std::make_shared<QuestLogScreen>(quest_));
                break;

            case Quest::QuestStage::InProgress: {
                for (auto& req : quest_->MonsterRequirements) {
                    req->CompletedCount = 0;
                    auto it = party_->MonsterKills().find(req->Content->AssetName());
                    if (it != party_->MonsterKills().end()) req->CompletedCount = it->second;
                }
                for (auto& req : quest_->GearRequirements) {
                    req->CompletedCount = 0;
                    for (auto& entry : party_->Inventory())
                        if (entry->Content == req->Content) req->CompletedCount += entry->Count;
                }
                if (quest_->AreRequirementsMet()) {
                    for (auto& req : quest_->GearRequirements) party_->RemoveFromInventory(req->Content, req->Count);
                    if (quest_->DestinationMapContentName.empty()) {
                        quest_->Stage = Quest::QuestStage::Completed;
                        if (!quest_->CompletionMessage.empty()) {
                            auto dialogueScreen = std::make_shared<DialogueScreen>();
                            dialogueScreen->SetTitleText("Quest Complete");
                            dialogueScreen->SetBackText(std::string());
                            dialogueScreen->SetDialogueText(quest_->CompletionMessage);
                            screenManager_->AddScreen(dialogueScreen);
                        }
                    } else {
                        quest_->Stage = Quest::QuestStage::RequirementsMet;
                        screenManager_->AddScreen(std::make_shared<QuestLogScreen>(quest_));
                    }
                }
                break;
            }

            case Quest::QuestStage::RequirementsMet:
                break;

            case Quest::QuestStage::Completed: {
                auto rewards = std::make_shared<RewardsScreen>(RewardsScreen::RewardScreenMode::Quest,
                                                                quest_->ExperienceReward, quest_->GoldReward,
                                                                quest_->GearRewards);
                screenManager_->AddScreen(rewards);
                currentQuestIndex_++;
                quest_.reset();
                break;
            }
        }
    }
}

inline void Session::Update(const GameTime& gameTime) {
    if (!singleton_) return;
    if (CombatEngine::IsActive()) {
        CombatEngine::Update(gameTime);
    } else {
        singleton_->UpdateQuest();
        TileEngine::Update(gameTime);
    }
}

inline void Session::Draw(const GameTime& gameTime) {
    auto& spriteBatch = singleton_->screenManager_->getSpriteBatch();

    if (CombatEngine::IsActive()) {
        if (TileEngine::Map()->CombatTexture) {
            spriteBatch.Begin();
            spriteBatch.Draw(*TileEngine::Map()->CombatTexture, Microsoft::Xna::Framework::Vector2::Zero,
                             Microsoft::Xna::Framework::Color(255, 255, 255, 255));
            spriteBatch.End();
        }
        spriteBatch.Begin(Microsoft::Xna::Framework::Graphics::SpriteSortMode::BackToFront,
                          Microsoft::Xna::Framework::Graphics::BlendState::AlphaBlend);
        CombatEngine::Draw(gameTime);
        spriteBatch.End();
    } else {
        singleton_->DrawNonCombat(gameTime);
    }

    if (singleton_->hud_) singleton_->hud_->Draw();
}

inline void Session::DrawNonCombat(const GameTime& gameTime) {
    auto& spriteBatch = screenManager_->getSpriteBatch();
    auto* map = TileEngine::Map().get();
    float elapsedSeconds = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    int viewportHeight = TileEngine::CurrentViewport().getHeightProperty();

    spriteBatch.Begin();
    if (map->Texture) {
        TileEngine::DrawLayers(spriteBatch, true, true, false);
        DrawShadows(spriteBatch);
    }
    spriteBatch.End();

    spriteBatch.Begin(); // BackToFront sort mode not used -- see missing.md

    auto& leader = *party_->Players[0];
    Microsoft::Xna::Framework::Vector2 position = TileEngine::PartyLeaderPosition().ScreenPosition();
    leader.CharacterDirection = TileEngine::PartyLeaderPosition().PositionDirection;
    leader.ResetAnimation(TileEngine::PartyLeaderPosition().IsMoving());
    auto* activeSprite =
        (leader.State == RolePlayingGameData::Character::CharacterState::Walking && leader.WalkingSprite)
            ? leader.WalkingSprite.get()
            : leader.MapSprite.get();
    if (activeSprite) {
        activeSprite->UpdateAnimation(elapsedSeconds);
        activeSprite->Draw(spriteBatch, position, 1.0f - position.Y / (float)viewportHeight);
    }

    for (auto& entry : map->PlayerNpcEntries) {
        if (!entry->Content) continue;
        Microsoft::Xna::Framework::Vector2 pos = TileEngine::GetScreenPosition(entry->MapPosition);
        entry->Content->ResetAnimation(false);
        if (entry->Content->MapSprite) {
            entry->Content->MapSprite->UpdateAnimation(elapsedSeconds);
            entry->Content->MapSprite->Draw(spriteBatch, pos, 1.0f - pos.Y / (float)viewportHeight);
        }
    }
    for (auto& entry : map->QuestNpcEntries) {
        if (!entry->Content) continue;
        Microsoft::Xna::Framework::Vector2 pos = TileEngine::GetScreenPosition(entry->MapPosition);
        entry->Content->ResetAnimation(false);
        if (entry->Content->MapSprite) {
            entry->Content->MapSprite->UpdateAnimation(elapsedSeconds);
            entry->Content->MapSprite->Draw(spriteBatch, pos, 1.0f - pos.Y / (float)viewportHeight);
        }
    }
    for (auto& entry : map->FixedCombatEntries) {
        if (!entry->Content || entry->Content->Entries.empty() || !entry->MapSprite) continue;
        Microsoft::Xna::Framework::Vector2 pos = TileEngine::GetScreenPosition(entry->MapPosition);
        entry->MapSprite->UpdateAnimation(elapsedSeconds);
        entry->MapSprite->Draw(spriteBatch, pos, 1.0f - pos.Y / (float)viewportHeight);
    }
    if (quest_ &&
        (quest_->Stage == Quest::QuestStage::InProgress || quest_->Stage == Quest::QuestStage::RequirementsMet)) {
        for (auto& entry : quest_->FixedCombatEntries) {
            if (!entry->Content || entry->Content->Entries.empty() || !entry->MapSprite ||
                !EndsWith(map->AssetName(), entry->MapContentName))
                continue;
            Microsoft::Xna::Framework::Vector2 pos = TileEngine::GetScreenPosition(entry->MapPosition);
            entry->MapSprite->UpdateAnimation(elapsedSeconds);
            entry->MapSprite->Draw(spriteBatch, pos, 1.0f - pos.Y / (float)viewportHeight);
        }
    }
    for (auto& entry : map->ChestEntries) {
        if (!entry->Content || !entry->Content->Texture) continue;
        Microsoft::Xna::Framework::Vector2 pos = TileEngine::GetScreenPosition(entry->MapPosition);
        spriteBatch.Draw(*entry->Content->Texture, pos, Microsoft::Xna::Framework::Color(255, 255, 255, 255));
    }
    if (quest_ &&
        (quest_->Stage == Quest::QuestStage::InProgress || quest_->Stage == Quest::QuestStage::RequirementsMet)) {
        for (auto& entry : quest_->ChestEntries) {
            if (!entry->Content || !entry->Content->Texture || !EndsWith(map->AssetName(), entry->MapContentName))
                continue;
            Microsoft::Xna::Framework::Vector2 pos = TileEngine::GetScreenPosition(entry->MapPosition);
            spriteBatch.Draw(*entry->Content->Texture, pos, Microsoft::Xna::Framework::Color(255, 255, 255, 255));
        }
    }
    spriteBatch.End();

    spriteBatch.Begin();
    if (map->Texture) TileEngine::DrawLayers(spriteBatch, false, false, true);
    spriteBatch.End();
}

inline void Session::DrawShadows(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) {
    auto* map = TileEngine::Map().get();
    auto drawShadow = [&](RolePlayingGameData::Character& character, Microsoft::Xna::Framework::Vector2 pos) {
        if (!character.ShadowTexture) return;
        spriteBatch.Draw(
            *character.ShadowTexture, pos, std::nullopt, Microsoft::Xna::Framework::Color(255, 255, 255, 255), 0.0f,
            Microsoft::Xna::Framework::Vector2(
                (float)(character.ShadowTexture->getWidthProperty() - map->TileSize.X) / 2.0f,
                (float)(character.ShadowTexture->getHeightProperty() - map->TileSize.Y) / 2.0f -
                    character.ShadowTexture->getHeightProperty() / 6.0f),
            1.0f, Microsoft::Xna::Framework::Graphics::SpriteEffects::None, 1.0f);
    };

    drawShadow(*party_->Players[0], TileEngine::PartyLeaderPosition().ScreenPosition());
    for (auto& entry : map->PlayerNpcEntries)
        if (entry->Content) drawShadow(*entry->Content, TileEngine::GetScreenPosition(entry->MapPosition));
    for (auto& entry : map->QuestNpcEntries)
        if (entry->Content) drawShadow(*entry->Content, TileEngine::GetScreenPosition(entry->MapPosition));
    for (auto& entry : map->FixedCombatEntries) {
        if (!entry->Content || entry->Content->Entries.empty()) continue;
        auto& monster = *entry->Content->Entries[0]->Content;
        drawShadow(monster, TileEngine::GetScreenPosition(entry->MapPosition));
    }
}

inline Session::Session(ScreenManager& screenManager, GameplayScreen& gameplayScreen)
    : screenManager_(&screenManager), gameplayScreen_(&gameplayScreen) {
    // create the HUD interface
    hud_ = new Hud(screenManager);
    hud_->LoadContent();
}

inline void Session::StartNewSession(const GameStartDescription& gameStartDescription,
                                     ScreenManager& screenManager,
                                     GameplayScreen& gameplayScreen) {
    // end any existing session
    EndSession();

    // create a new singleton
    singleton_ = new Session(screenManager, gameplayScreen);

    ChangeMap(gameStartDescription.MapContentName, nullptr);

    // set up the initial party
    auto& content = screenManager.getGameProperty().getContentProperty();
    singleton_->party_ = std::make_unique<Party>(gameStartDescription, content);

    // load the quest line
    singleton_->questLine_ =
        content
            .Load<std::shared_ptr<RolePlayingGameData::QuestLine>>(
                "Quests/QuestLines/" + gameStartDescription.QuestLineContentName)
            ->Clone();
}

inline void Session::EndSession() {
    if (!singleton_) return;
    GameplayScreen* gameplayScreen = singleton_->gameplayScreen_;
    singleton_->gameplayScreen_ = nullptr;
    AudioManager::PopMusic();
    Session* old = singleton_;
    singleton_ = nullptr;
    delete old;
    if (gameplayScreen) gameplayScreen->ExitScreen();
}

// ---- CombatEngine methods that depend on Session ----

inline CombatEngine::CombatEngine(std::vector<std::shared_ptr<CombatantPlayer>> players,
                                  std::vector<std::shared_ptr<CombatantMonster>> monsters,
                                  int fleeThreshold) {
    // check the parameters
    if (players.empty() || players.size() > PlayerPositionCount) {
        throw System::ArgumentException("players");
    }
    if (monsters.empty() || monsters.size() > MonsterPositionCount) {
        throw System::ArgumentException("monsters");
    }

    // assign the parameters
    players_ = std::move(players);
    monsters_ = std::move(monsters);
    fleeThreshold_ = fleeThreshold;

    // assign positions
    for (std::size_t i = 0; i < players_.size(); i++) {
        if (i >= PlayerPositionCount) {
            break;
        }
        players_[i]->SetPosition(PlayerPositions()[i]);
        players_[i]->SetOriginalPosition(PlayerPositions()[i]);
    }
    for (std::size_t i = 0; i < monsters_.size(); i++) {
        if (i >= MonsterPositionCount) {
            break;
        }
        monsters_[i]->SetPosition(MonsterPositions()[i]);
        monsters_[i]->SetOriginalPosition(MonsterPositions()[i]);
    }

    // sort the monsters by the y coordinates, descending
    std::stable_sort(monsters_.begin(), monsters_.end(),
                     [](const std::shared_ptr<CombatantMonster>& monster1,
                        const std::shared_ptr<CombatantMonster>& monster2) {
                         return monster1->OriginalPosition().Y > monster2->OriginalPosition().Y;
                     });

    // create the selection sprites
    CreateSelectionSprites();

    // create the combat effect sprites
    CreateCombatEffectSprites();

    // start the first combat turn after a delay
    delayType_ = DelayType::StartCombat;

    // start the combat music
    AudioManager::PushMusic(TileEngine::Map()->CombatMusicCueName);
}

// Generates a list of CombatantPlayer objects from the party members.
inline std::vector<std::shared_ptr<CombatantPlayer>> GenerateCombatantsFromParty() {
    std::vector<std::shared_ptr<CombatantPlayer>> generatedPlayers;

    for (const std::shared_ptr<Player>& player : Session::GetParty()->Players) {
        if (generatedPlayers.size() <= 5) {
            generatedPlayers.push_back(std::make_shared<CombatantPlayer>(player));
        }
    }

    return generatedPlayers;
}

inline void CombatEngine::StartNewCombat(
    const std::shared_ptr<MapEntry<FixedCombat>>& fixedCombatEntry) {
    // check the parameter
    if (fixedCombatEntry == nullptr) {
        throw System::ArgumentNullException("fixedCombatEntry");
    }
    const std::shared_ptr<FixedCombat>& fixedCombat = fixedCombatEntry->Content;
    if (fixedCombat == nullptr) {
        throw System::ArgumentException("fixedCombatEntry has no content.");
    }

    // generate the monster combatant list
    std::vector<std::shared_ptr<CombatantMonster>> generatedMonsters;
    for (const auto& entry : fixedCombat->Entries) {
        for (int i = 0; i < entry->Count; i++) {
            generatedMonsters.push_back(std::make_shared<CombatantMonster>(entry->Content));
        }
    }

    // randomize the list of monsters
    std::vector<std::shared_ptr<CombatantMonster>> randomizedMonsters;
    while (!generatedMonsters.empty() && randomizedMonsters.size() <= MonsterPositionCount) {
        int index = Session::GetRandom().Next((int)generatedMonsters.size());
        randomizedMonsters.push_back(generatedMonsters[(std::size_t)index]);
        generatedMonsters.erase(generatedMonsters.begin() + index);
    }

    // start the combat
    if (singleton_ != nullptr) {
        throw System::InvalidOperationException("There can only be one combat at a time.");
    }
    retiredSingleton_.reset();
    singleton_ = std::unique_ptr<CombatEngine>(
        new CombatEngine(GenerateCombatantsFromParty(), std::move(randomizedMonsters), 0));
    singleton_->fixedCombatEntry_ = fixedCombatEntry;
}

inline void CombatEngine::StartNewCombat(const std::shared_ptr<RandomCombat>& randomCombat) {
    // check the parameter
    if (randomCombat == nullptr) {
        throw System::ArgumentNullException("randomCombat");
    }

    // determine how many monsters will be in the combat
    int monsterCount = randomCombat->MonsterCountRange.GenerateValue(Session::GetRandom());

    // determine the total probability
    int totalWeight = 0;
    for (const auto& entry : randomCombat->Entries) {
        totalWeight += entry->Weight;
    }

    // generate each monster
    std::vector<std::shared_ptr<CombatantMonster>> generatedMonsters;
    for (int i = 0; i < monsterCount; i++) {
        int monsterChoice = Session::GetRandom().Next(totalWeight);
        for (const auto& entry : randomCombat->Entries) {
            if (monsterChoice < entry->Weight) {
                generatedMonsters.push_back(std::make_shared<CombatantMonster>(entry->Content));
                break;
            } else {
                monsterChoice -= entry->Weight;
            }
        }
    }

    // randomize the list of monsters
    std::vector<std::shared_ptr<CombatantMonster>> randomizedMonsters;
    while (!generatedMonsters.empty() && randomizedMonsters.size() <= MonsterPositionCount) {
        int index = Session::GetRandom().Next((int)generatedMonsters.size());
        randomizedMonsters.push_back(generatedMonsters[(std::size_t)index]);
        generatedMonsters.erase(generatedMonsters.begin() + index);
    }

    // start the combat
    if (singleton_ != nullptr) {
        throw System::InvalidOperationException("There can only be one combat at a time.");
    }
    retiredSingleton_.reset();
    singleton_ = std::unique_ptr<CombatEngine>(
        new CombatEngine(GenerateCombatantsFromParty(), std::move(randomizedMonsters),
                         randomCombat->FleeProbability));
}

inline void CombatEngine::BeginPlayerTurn(const std::shared_ptr<CombatantPlayer>& player) {
    // check the parameter
    if (player == nullptr) {
        throw System::ArgumentNullException("player");
    }

    // set the highlight sprite
    highlightedCombatant_ = player.get();
    primaryTargetedCombatant_ = nullptr;
    secondaryTargetedCombatants_.clear();

    Session::GetHud()->SetActionText("Choose an Action");
}

inline void CombatEngine::BeginPlayersTurn() {
    // set the player-turn
    isPlayersTurn_ = true;

    // reset each player for the next combat turn
    for (const std::shared_ptr<CombatantPlayer>& player : players_) {
        // reset the animation of living players
        if (!player->IsDeadOrDying()) {
            player->SetState(Character::CharacterState::Idle);
        }
        // reset the turn-taken flag
        player->SetIsTurnTaken(false);
        // clear the combat action
        player->SetCombatAction(nullptr);
        // advance each player
        player->AdvanceRound();
    }

    // set the action text on the HUD
    Session::GetHud()->SetActionText("Your Party's Turn");

    // find the first player who is alive
    highlightedPlayer_ = 0;
    std::shared_ptr<CombatantPlayer> firstPlayer = players_[(std::size_t)highlightedPlayer_];
    while (firstPlayer->IsTurnTaken() || firstPlayer->IsDeadOrDying()) {
        highlightedPlayer_ = (highlightedPlayer_ + 1) % (int)players_.size();
        firstPlayer = players_[(std::size_t)highlightedPlayer_];
    }

    // start the first player's turn
    BeginPlayerTurn(firstPlayer);
}

inline void CombatEngine::BeginMonstersTurn() {
    // set the monster-turn
    isPlayersTurn_ = false;

    // reset each monster for the next combat turn
    for (const std::shared_ptr<CombatantMonster>& monster : monsters_) {
        // reset the animations back to idle
        monster->SetState(Character::CharacterState::Idle);
        // reset the turn-taken flag
        monster->SetIsTurnTaken(false);
        // clear the combat action
        monster->SetCombatAction(nullptr);
        // advance the combatants
        monster->AdvanceRound();
    }

    // set the action text on the HUD
    Session::GetHud()->SetActionText("Enemy Party's Turn");

    // start a random monster's turn
    BeginMonsterTurn(nullptr);
}

inline void CombatEngine::AttemptFlee() {
    CheckSingleton();

    if (!IsPlayersTurn()) {
        throw System::InvalidOperationException("Only the players may flee.");
    }

    singleton_->delayType_ = DelayType::FleeAttempt;
    Session::GetHud()->SetActionText("Attempting to Escape...");
}

inline void CombatEngine::CreateCombatEffectSprites() {
    auto& content = Session::GetScreenManager()->getGameProperty().getContentProperty();

    damageCombatEffectTexture_ = content.Load<Texture2D>("Textures/Combat/DamageIcon");
    healingCombatEffectTexture_ = content.Load<Texture2D>("Textures/Combat/HealingIcon");
}

inline void CombatEngine::DrawCombatEffects(const GameTime& gameTime) {
    float elapsedSeconds = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    SpriteBatch& spriteBatch = Session::GetScreenManager()->getSpriteBatch();

    // update all effects
    for (CombatEffect& combatEffect : damageCombatEffects_) {
        combatEffect.Update(elapsedSeconds);
    }
    for (CombatEffect& combatEffect : healingCombatEffects_) {
        combatEffect.Update(elapsedSeconds);
    }

    // draw the damage effects
    for (CombatEffect& combatEffect : damageCombatEffects_) {
        combatEffect.Draw(spriteBatch, damageCombatEffectTexture_);
    }

    // draw the healing effects
    for (CombatEffect& combatEffect : healingCombatEffects_) {
        combatEffect.Draw(spriteBatch, healingCombatEffectTexture_);
    }

    // remove all complete effects
    auto removeCompleteEffects = [](const CombatEffect& combatEffect) {
        return combatEffect.IsRiseComplete();
    };
    damageCombatEffects_.erase(std::remove_if(damageCombatEffects_.begin(),
                                              damageCombatEffects_.end(), removeCompleteEffects),
                               damageCombatEffects_.end());
    healingCombatEffects_.erase(
        std::remove_if(healingCombatEffects_.begin(), healingCombatEffects_.end(),
                       removeCompleteEffects),
        healingCombatEffects_.end());
}

inline void CombatEngine::CreateSelectionSprites() {
    auto& content = Session::GetScreenManager()->getGameProperty().getContentProperty();

    Point frameDimensions(76, 58);
    highlightForegroundSprite_.FramesPerRow = 6;
    highlightForegroundSprite_.SetFrameDimensions(frameDimensions);
    highlightForegroundSprite_.AddAnimation(std::make_shared<Animation>("Selection", 1, 4, 100, true));
    highlightForegroundSprite_.PlayAnimation(0);
    highlightForegroundSprite_.SourceOffset = Vector2((float)frameDimensions.X / 2.0f, 40.0f);
    highlightForegroundSprite_.Texture = std::make_shared<Texture2D>(
        content.Load<Texture2D>("Textures/Combat/TilesheetSprangles"));

    frameDimensions = Point(102, 54);
    highlightBackgroundSprite_.FramesPerRow = 4;
    highlightBackgroundSprite_.SetFrameDimensions(frameDimensions);
    highlightBackgroundSprite_.AddAnimation(std::make_shared<Animation>("Selection", 1, 4, 100, true));
    highlightBackgroundSprite_.PlayAnimation(0);
    highlightBackgroundSprite_.SourceOffset =
        Vector2((float)frameDimensions.X / 2.0f, (float)frameDimensions.Y / 2.0f);
    highlightBackgroundSprite_.Texture = std::make_shared<Texture2D>(
        content.Load<Texture2D>("Textures/Combat/CharSelectionRing"));

    primaryTargetSprite_.FramesPerRow = 4;
    primaryTargetSprite_.SetFrameDimensions(frameDimensions);
    primaryTargetSprite_.AddAnimation(std::make_shared<Animation>("Selection", 1, 4, 100, true));
    primaryTargetSprite_.PlayAnimation(0);
    primaryTargetSprite_.SourceOffset =
        Vector2((float)frameDimensions.X / 2.0f, (float)frameDimensions.Y / 2.0f);
    primaryTargetSprite_.Texture = std::make_shared<Texture2D>(
        content.Load<Texture2D>("Textures/Combat/Target1SelectionRing"));

    secondaryTargetSprite_.FramesPerRow = 4;
    secondaryTargetSprite_.SetFrameDimensions(frameDimensions);
    secondaryTargetSprite_.AddAnimation(std::make_shared<Animation>("Selection", 1, 4, 100, true));
    secondaryTargetSprite_.PlayAnimation(0);
    secondaryTargetSprite_.SourceOffset =
        Vector2((float)frameDimensions.X / 2.0f, (float)frameDimensions.Y / 2.0f);
    secondaryTargetSprite_.Texture = std::make_shared<Texture2D>(
        content.Load<Texture2D>("Textures/Combat/Target2SelectionRing"));
}

inline void CombatEngine::DrawSelectionSprites(const GameTime& gameTime) {
    SpriteBatch& spriteBatch = Session::GetScreenManager()->getSpriteBatch();
    auto viewport = Session::GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

    // update the animations
    float elapsedSeconds = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
    highlightForegroundSprite_.UpdateAnimation(elapsedSeconds);
    highlightBackgroundSprite_.UpdateAnimation(elapsedSeconds);
    primaryTargetSprite_.UpdateAnimation(elapsedSeconds);
    secondaryTargetSprite_.UpdateAnimation(elapsedSeconds);

    // draw the highlighted-player sprite, if any
    if (highlightedCombatant_ != nullptr) {
        highlightBackgroundSprite_.Draw(
            spriteBatch, highlightedCombatant_->Position(),
            1.0f - (highlightedCombatant_->Position().Y - 1.0f) / (float)viewport.getHeightProperty());
        highlightForegroundSprite_.Draw(
            spriteBatch, highlightedCombatant_->Position(),
            1.0f - (highlightedCombatant_->Position().Y + 1.0f) / (float)viewport.getHeightProperty());
    }

    // draw the primary target sprite and name, if any
    if (primaryTargetedCombatant_ != nullptr) {
        primaryTargetSprite_.Draw(
            spriteBatch, primaryTargetedCombatant_->Position(),
            1.0f -
                (primaryTargetedCombatant_->Position().Y - 1.0f) / (float)viewport.getHeightProperty());
        if (dynamic_cast<Monster*>(&primaryTargetedCombatant_->GetCharacter()) != nullptr) {
            Fonts::DrawCenteredText(spriteBatch, Fonts::DamageFont(),
                                    primaryTargetedCombatant_->GetCharacter().Name(),
                                    primaryTargetedCombatant_->Position() + Vector2(0.0f, 42.0f),
                                    Microsoft::Xna::Framework::Color::White);
        }
    }

    // draw the secondary target sprites on live enemies, if any
    for (Combatant* combatant : secondaryTargetedCombatants_) {
        if (combatant->IsDeadOrDying()) {
            continue;
        }
        secondaryTargetSprite_.Draw(
            spriteBatch, combatant->Position(),
            1.0f - (combatant->Position().Y - 1.0f) / (float)viewport.getHeightProperty());
        if (dynamic_cast<Monster*>(&combatant->GetCharacter()) != nullptr) {
            Fonts::DrawCenteredText(spriteBatch, Fonts::DamageFont(),
                                    combatant->GetCharacter().Name(),
                                    combatant->Position() + Vector2(0.0f, 42.0f),
                                    Microsoft::Xna::Framework::Color::White);
        }
    }
}

inline void CombatEngine::UpdateDelay(int elapsedMilliseconds) {
    if (delayType_ == DelayType::NoDelay) {
        return;
    }

    // increment the delay
    currentDelay_ += elapsedMilliseconds;

    // if the delay is ongoing, then we're done
    if (currentDelay_ < TotalDelay) {
        return;
    }
    currentDelay_ = 0;

    // the delay has ended, so the operation implied by the DelayType happens
    switch (delayType_) {
    case DelayType::StartCombat: {
        // determine who goes first and start combat
        int whoseTurn = Session::GetRandom().Next(2);
        if (whoseTurn == 0) {
            BeginPlayersTurn();
        } else {
            BeginMonstersTurn();
        }
        delayType_ = DelayType::NoDelay;
    } break;

    case DelayType::EndCharacterTurn:
        if (IsPlayersTurn()) {
            // check to see if the players' turn is complete
            if (IsPlayersTurnComplete()) {
                delayType_ = DelayType::EndRound;
                break;
            }
            // find the next player
            int highlightedIndex = -1;
            for (int i = 0; i < (int)players_.size(); i++) {
                if (players_[(std::size_t)i].get() == highlightedCombatant_) {
                    highlightedIndex = i;
                    break;
                }
            }
            int nextIndex = (highlightedIndex + 1) % (int)players_.size();
            while (players_[(std::size_t)nextIndex]->IsDeadOrDying() ||
                   players_[(std::size_t)nextIndex]->IsTurnTaken()) {
                nextIndex = (nextIndex + 1) % (int)players_.size();
            }
            BeginPlayerTurn(players_[(std::size_t)nextIndex]);
        } else {
            // check to see if the monsters' turn is complete
            if (IsMonstersTurnComplete()) {
                delayType_ = DelayType::EndRound;
                break;
            }
            // find the next monster
            BeginMonsterTurn(nullptr);
        }
        delayType_ = DelayType::NoDelay;
        break;

    case DelayType::EndRound:
        // check for turn completion
        if (isPlayersTurn_ && IsPlayersTurnComplete()) {
            BeginMonstersTurn();
        } else if (!isPlayersTurn_ && IsMonstersTurnComplete()) {
            BeginPlayersTurn();
        }
        delayType_ = DelayType::NoDelay;
        break;

    case DelayType::FleeAttempt:
        if (fleeThreshold_ <= 0) {
            delayType_ = DelayType::EndCharacterTurn;
            Session::GetHud()->SetActionText("This Fight Cannot Be Escaped...");
            if (highlightedCombatant_ != nullptr) {
                highlightedCombatant_->SetIsTurnTaken(true);
            }
        } else if (CalculateFleeAttempt()) {
            delayType_ = DelayType::FleeSuccessful;
            Session::GetHud()->SetActionText("Your Party Has Fled!");
        } else {
            delayType_ = DelayType::EndCharacterTurn;
            Session::GetHud()->SetActionText("Your Party Failed to Escape!");
            if (highlightedCombatant_ != nullptr) {
                highlightedCombatant_->SetIsTurnTaken(true);
            }
        }
        break;

    case DelayType::FleeSuccessful:
        EndCombat(CombatEndingState::Fled);
        delayType_ = DelayType::NoDelay;
        break;

    default:
        break;
    }
}

inline void CombatEngine::EndCombat(CombatEndingState combatEndingState) {
    // go back to the non-combat music
    AudioManager::PopMusic();

    switch (combatEndingState) {
    case CombatEndingState::Victory: {
        int experienceReward = 0;
        int goldReward = 0;
        std::vector<std::shared_ptr<RolePlayingGameData::Gear>> gearRewards;
        std::vector<std::string> gearRewardNames;
        // calculate the rewards from the monsters
        for (const std::shared_ptr<CombatantMonster>& combatantMonster : monsters_) {
            const std::shared_ptr<Monster>& monster = combatantMonster->GetMonster();
            Session::GetParty()->AddMonsterKill(*monster);
            experienceReward += monster->CalculateExperienceReward(Session::GetRandom());
            goldReward += monster->CalculateGoldReward(Session::GetRandom());
            std::vector<std::string> drops = monster->CalculateGearDrop(Session::GetRandom());
            gearRewardNames.insert(gearRewardNames.end(), drops.begin(), drops.end());
        }
        auto& content = Session::GetScreenManager()->getGameProperty().getContentProperty();
        for (const std::string& gearRewardName : gearRewardNames) {
            gearRewards.push_back(
                content.Load<std::shared_ptr<RolePlayingGameData::Gear>>("Gear/" + gearRewardName));
        }
        // remove the fixed combat entry, if this wasn't a random fight
        std::shared_ptr<MapEntry<FixedCombat>> entry = fixedCombatEntry_;
        ScreenManager* screenManager = Session::GetScreenManager();
        // retire the singleton before opening the reward screen, which may start a new combat
        RetireSingleton();
        // add the reward screen
        screenManager->AddScreen(std::make_shared<RewardsScreen>(
            RewardsScreen::RewardScreenMode::Combat, experienceReward, goldReward, gearRewards));
        if (entry != nullptr) {
            Session::RemoveFixedCombat(entry);
        }
        // the singleton owned this object; delete it now that nothing else refers to it
        return;
    }

    case CombatEndingState::Loss: { // game over
        ScreenManager* screenManager = Session::GetScreenManager();
        // end the session
        Session::EndSession();
        // add the game-over screen
        screenManager->AddScreen(std::make_shared<GameOverScreen>());
        break;
    }

    case CombatEndingState::Fled:
        break;
    }

    // clear the singleton
    RetireSingleton();
}

inline void CombatEngine::UpdateCombatEngine(const GameTime& gameTime) {
    // check for the end of combat
    if (ArePlayersDefeated()) {
        EndCombat(CombatEndingState::Loss);
        return;
    } else if (AreMonstersDefeated()) {
        EndCombat(CombatEndingState::Victory);
        return;
    }

    // update the target selections
    if (highlightedCombatant_ != nullptr && highlightedCombatant_->GetCombatAction() != nullptr) {
        SetTargets(highlightedCombatant_->GetCombatAction()->Target,
                   highlightedCombatant_->GetCombatAction()->AdjacentTargets());
    }

    // update the delay
    UpdateDelay((int)gameTime.getElapsedGameTimeProperty().getMillisecondsProperty());
    // UpdateDelay might cause combat to end due to a successful escape, which clears the
    // singleton.
    if (singleton_ == nullptr) {
        return;
    }

    // update the players
    for (const std::shared_ptr<CombatantPlayer>& player : players_) {
        player->Update(gameTime);
    }

    // update the monsters
    for (const std::shared_ptr<CombatantMonster>& monster : monsters_) {
        monster->Update(gameTime);
    }

    // check for completion of the highlighted combatant
    if (delayType_ == DelayType::NoDelay && highlightedCombatant_ != nullptr &&
        highlightedCombatant_->IsTurnTaken()) {
        delayType_ = DelayType::EndCharacterTurn;
    }

    // handle any player input
    HandleInput();
}

inline void CombatEngine::HandleInput() {
    // only accept input during the players' turn
    // -- exit game, etc. is handled by GameplayScreen
    if (!isPlayersTurn_ || IsPlayersTurnComplete() || highlightedCombatant_ == nullptr) {
        return;
    }

#ifndef NDEBUG
    // cheat key
    if (InputManager::IsGamePadRightShoulderTriggered() ||
        InputManager::IsKeyTriggered(Microsoft::Xna::Framework::Input::Keys::W)) {
        EndCombat(CombatEndingState::Victory);
        return;
    }
#endif

    // handle input while choosing an action
    if (highlightedCombatant_->GetCombatAction() != nullptr) {
        // skip if its turn is over or the action is already going
        if (highlightedCombatant_->IsTurnTaken() ||
            highlightedCombatant_->GetCombatAction()->Stage() !=
                CombatAction::CombatActionStage::NotStarted) {
            return;
        }

        // back out of the action
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            highlightedCombatant_->SetCombatAction(nullptr);
            SetTargets(nullptr, 0);
            return;
        }

        // start the action
        if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            highlightedCombatant_->GetCombatAction()->Start();
            return;
        }

        // go to the next target
        if (InputManager::IsActionTriggered(InputManager::Action::TargetUp)) {
            // cycle through monsters or party members
            if (highlightedCombatant_->GetCombatAction()->IsOffensive()) {
                int newIndex = IndexOfMonster(primaryTargetedCombatant_);
                do {
                    newIndex = (newIndex + 1) % (int)monsters_.size();
                } while (monsters_[(std::size_t)newIndex]->IsDeadOrDying());
                highlightedCombatant_->GetCombatAction()->Target =
                    monsters_[(std::size_t)newIndex].get();
            } else {
                int newIndex = IndexOfPlayer(primaryTargetedCombatant_);
                do {
                    newIndex = (newIndex + 1) % (int)players_.size();
                } while (players_[(std::size_t)newIndex]->IsDeadOrDying());
                highlightedCombatant_->GetCombatAction()->Target =
                    players_[(std::size_t)newIndex].get();
            }
            return;
        }
        // go to the previous target
        else if (InputManager::IsActionTriggered(InputManager::Action::TargetDown)) {
            // cycle through monsters or party members
            if (highlightedCombatant_->GetCombatAction()->IsOffensive()) {
                int newIndex = IndexOfMonster(primaryTargetedCombatant_);
                do {
                    newIndex--;
                    while (newIndex < 0) {
                        newIndex += (int)monsters_.size();
                    }
                } while (monsters_[(std::size_t)newIndex]->IsDeadOrDying());
                highlightedCombatant_->GetCombatAction()->Target =
                    monsters_[(std::size_t)newIndex].get();
            } else {
                int newIndex = IndexOfPlayer(primaryTargetedCombatant_);
                do {
                    newIndex--;
                    while (newIndex < 0) {
                        newIndex += (int)players_.size();
                    }
                } while (players_[(std::size_t)newIndex]->IsDeadOrDying());
                highlightedCombatant_->GetCombatAction()->Target =
                    players_[(std::size_t)newIndex].get();
            }
            return;
        }
    } else // choosing which character will act
    {
        // move to the previous living character
        if (InputManager::IsActionTriggered(InputManager::Action::ActiveCharacterLeft)) {
            int newHighlightedPlayer = highlightedPlayer_;
            do {
                newHighlightedPlayer--;
                while (newHighlightedPlayer < 0) {
                    newHighlightedPlayer += (int)players_.size();
                }
            } while (players_[(std::size_t)newHighlightedPlayer]->IsDeadOrDying() ||
                     players_[(std::size_t)newHighlightedPlayer]->IsTurnTaken());
            if (newHighlightedPlayer != highlightedPlayer_) {
                highlightedPlayer_ = newHighlightedPlayer;
                BeginPlayerTurn(players_[(std::size_t)highlightedPlayer_]);
            }
            return;
        }
        // move to the next living character
        else if (InputManager::IsActionTriggered(InputManager::Action::ActiveCharacterRight)) {
            int newHighlightedPlayer = highlightedPlayer_;
            do {
                newHighlightedPlayer = (newHighlightedPlayer + 1) % (int)players_.size();
            } while (players_[(std::size_t)newHighlightedPlayer]->IsDeadOrDying() ||
                     players_[(std::size_t)newHighlightedPlayer]->IsTurnTaken());
            if (newHighlightedPlayer != highlightedPlayer_) {
                highlightedPlayer_ = newHighlightedPlayer;
                BeginPlayerTurn(players_[(std::size_t)highlightedPlayer_]);
            }
            return;
        }
        Session::GetHud()->UpdateActionsMenu();
    }
}

// ---- Combatant methods that report to the combat engine ----

inline void Combatant::Heal(const StatisticsValue& healingStatistics, int duration) {
    (void)duration;
    CombatEngine::AddNewHealingEffects(OriginalPosition(), healingStatistics);
}

inline void Combatant::Damage(const StatisticsValue& damageStatistics, int duration) {
    (void)duration;
    SetState(Character::CharacterState::Hit);
    CombatEngine::AddNewDamageEffects(OriginalPosition(), damageStatistics);
}

inline void Combatant::Update(const GameTime& gameTime) {
    float elapsedSeconds = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

    // update the combat action
    if (combatAction_ != nullptr) {
        // update the combat action
        combatAction_->Update(gameTime);
        // remove the combat action if it is done and set the turn-taken flag
        if (combatAction_->Stage() == CombatAction::CombatActionStage::Complete) {
            combatAction_ = nullptr;
            isTurnTaken_ = true;
        }
    }

    // update the combat sprite animation
    GetCombatSprite().UpdateAnimation(elapsedSeconds);

    // check for death
    if (!IsDeadOrDying() && Statistics().HealthPoints <= 0) {
        AudioManager::PlayCue("Death");
        SetState(Character::CharacterState::Dying);
    }
    // check for waking up
    else if (IsDeadOrDying() && Statistics().HealthPoints > 0) {
        SetState(Character::CharacterState::Idle);
    } else if (GetCombatSprite().IsPlaybackComplete()) {
        if (GetState() == Character::CharacterState::Hit) {
            SetState(Character::CharacterState::Idle);
        } else if (GetState() == Character::CharacterState::Dying) {
            SetState(Character::CharacterState::Dead);
        }
    }
}

inline void Combatant::Draw(const GameTime& gameTime) {
    SpriteBatch& spriteBatch = Session::GetScreenManager()->getSpriteBatch();

    GetCombatSprite().Draw(spriteBatch, Position(), 1.0f - Position().Y / 720.0f);

    spriteBatch.Draw(*GetCharacter().ShadowTexture, Position(), std::nullopt,
                     Microsoft::Xna::Framework::Color::White, 0.0f,
                     Vector2((float)(GetCharacter().ShadowTexture->getWidthProperty() / 2),
                             (float)(GetCharacter().ShadowTexture->getHeightProperty() / 2)),
                     1.0f, Microsoft::Xna::Framework::Graphics::SpriteEffects::None, 1.0f);

    // draw the combat action
    if (combatAction_ != nullptr) {
        // update the combat action
        combatAction_->Draw(gameTime, spriteBatch);
    }
}

// ---- ArtificialIntelligence methods that read the engine's rosters ----

inline std::shared_ptr<CombatAction> ArtificialIntelligence::ChooseAction() {
    std::shared_ptr<CombatAction> combatAction;

    // determine if the monster will use a defensive action
    if (monster_->GetMonster()->DefendPercentage > 0 && !defensiveActions_.empty() &&
        Session::GetRandom().Next(0, 100) < monster_->GetMonster()->DefendPercentage) {
        combatAction = ChooseDefensiveAction();
    }

    // if we do not have an action yet, choose an offensive action
    if (combatAction == nullptr) {
        combatAction = ChooseOffensiveAction();
    }

    // reset the action to the initial state
    if (combatAction != nullptr) {
        combatAction->Reset();
    }

    return combatAction;
}

inline std::shared_ptr<CombatAction> ArtificialIntelligence::ChooseOffensiveAction() {
    std::vector<std::shared_ptr<CombatantPlayer>>& players = CombatEngine::Players();

    // be sure that there is a valid combat in progress
    if (players.empty()) {
        return nullptr;
    }

    // randomly choose a living target from the party
    int targetIndex;
    do {
        targetIndex = Session::GetRandom().Next((int)players.size());
    } while (players[(std::size_t)targetIndex]->IsDeadOrDying());
    CombatantPlayer* target = players[(std::size_t)targetIndex].get();

    // the action lists are sorted by descending potential, so find the first eligible action
    for (const std::shared_ptr<CombatAction>& action : offensiveActions_) {
        // check the restrictions on the action
        if (action->IsCharacterValidUser()) {
            action->Target = target;
            return action;
        }
    }

    // no eligible actions found
    return nullptr;
}

inline std::shared_ptr<CombatAction> ArtificialIntelligence::ChooseDefensiveAction() {
    std::vector<std::shared_ptr<CombatantMonster>>& monsters = CombatEngine::Monsters();

    // be sure that there is a valid combat in progress
    if (monsters.empty()) {
        return nullptr;
    }

    // find the monster with the least health
    CombatantMonster* target = nullptr;
    int leastHealthAmount = INT_MAX;
    for (const std::shared_ptr<CombatantMonster>& targetMonster : monsters) {
        // skip dead or dying targets
        if (targetMonster->IsDeadOrDying()) {
            continue;
        }
        // if the monster is damaged and it has the least health points, then it becomes the new
        // target
        StatisticsValue maxStatistics =
            targetMonster->GetMonster()->GetCharacterClass()->GetStatisticsForLevel(
                targetMonster->GetMonster()->CharacterLevel());
        int targetMonsterHealthPoints = targetMonster->Statistics().HealthPoints;
        if (targetMonsterHealthPoints < maxStatistics.HealthPoints &&
            targetMonsterHealthPoints < leastHealthAmount) {
            target = targetMonster.get();
            leastHealthAmount = targetMonsterHealthPoints;
        }
    }

    // if there is no target, then don't do anything
    if (target == nullptr) {
        return nullptr;
    }

    // the action lists are sorted by descending potential, so find the first eligible action
    for (const std::shared_ptr<CombatAction>& action : defensiveActions_) {
        // check the restrictions on the action
        if (action->IsCharacterValidUser()) {
            action->Target = target;
            return action;
        }
    }

    // no eligible actions found
    return nullptr;
}

// ---- combat actions whose Executing stage walks the engine's secondary targets ----

inline void SpellCombatAction::StartStage() {
    switch (stage_) {
    case CombatActionStage::Preparing: { // called from Start()
        // play the animations
        combatant_->GetCombatSprite().PlayAnimation("SpellCast");
        spellSpritePosition_ = GetCombatant().Position();
        spell_->SpellSprite->PlayAnimation("Creation");
        // remove the magic points
        GetCombatant().PayCostForSpell(spell_);
    } break;

    case CombatActionStage::Advancing: {
        // play the animations
        spell_->SpellSprite->PlayAnimation("Traveling");
        // calculate the projectile destination
        projectileDirection_ = Target->Position() - GetCombatant().OriginalPosition();
        totalProjectileDistance_ = projectileDirection_.Length();
        projectileDirection_.Normalize();
        projectileDistanceCovered_ = 0.0f;
        // determine if the projectile is flipped
        if (Target->Position().X > GetCombatant().Position().X) {
            projectileSpriteEffect_ = SpriteEffects::FlipHorizontally;
        } else {
            projectileSpriteEffect_ = SpriteEffects::None;
        }
        // get the projectile's cue and play it
        projectileCue_ = AudioManager::GetCue(spell_->TravelingCueName);
        if (projectileCue_ != nullptr) {
            projectileCue_->Play();
        }
    } break;

    case CombatActionStage::Executing: {
        // play the animation
        spell_->SpellSprite->PlayAnimation("Impact");
        // stop the projectile sound effect
        if (projectileCue_ != nullptr) {
            projectileCue_->Stop(AudioStopOptions::Immediate);
        }
        // apply the spell effect to the primary target
        bool damagedAnyone = ApplySpell(*Target);
        // apply the spell to the secondary targets
        for (Combatant* targetCombatant : CombatEngine::SecondaryTargetedCombatants()) {
            // skip dead or dying targets
            if (targetCombatant->IsDeadOrDying()) {
                continue;
            }
            // apply the spell
            damagedAnyone = ApplySpell(*targetCombatant) || damagedAnyone;
        }
        // play the impact sound effect
        if (damagedAnyone) {
            AudioManager::PlayCue(spell_->ImpactCueName);
            if (spell_->Overlay != nullptr) {
                spell_->Overlay->PlayAnimation(0);
                spell_->Overlay->ResetAnimation();
            }
        }
    } break;

    case CombatActionStage::Returning:
        // play the animation
        combatant_->GetCombatSprite().PlayAnimation("Idle");
        break;

    case CombatActionStage::Finishing:
        // play the animation
        combatant_->GetCombatSprite().PlayAnimation("Idle");
        break;

    case CombatActionStage::Complete:
        // play the animation
        combatant_->GetCombatSprite().PlayAnimation("Idle");
        // make sure that the overlay has stopped
        if (spell_->Overlay != nullptr) {
            spell_->Overlay->StopAnimation();
        }
        break;

    default:
        break;
    }
}

inline void ItemCombatAction::StartStage() {
    switch (stage_) {
    case CombatActionStage::Preparing: { // called from Start()
        // play the animations
        combatant_->GetCombatSprite().PlayAnimation("ItemCast");
        itemSpritePosition_ = GetCombatant().Position();
        item_->SpellSprite->PlayAnimation("Creation");
        Session::GetParty()->RemoveFromInventory(item_, 1);
    } break;

    case CombatActionStage::Advancing: {
        // play the animations
        item_->SpellSprite->PlayAnimation("Traveling");
        // calculate the projectile destination
        projectileDirection_ = Target->Position() - GetCombatant().OriginalPosition();
        totalProjectileDistance_ = projectileDirection_.Length();
        projectileDirection_.Normalize();
        projectileDistanceCovered_ = 0.0f;
        // determine if the projectile is flipped
        if (Target->Position().X > GetCombatant().Position().X) {
            projectileSpriteEffect_ = SpriteEffects::FlipHorizontally;
        } else {
            projectileSpriteEffect_ = SpriteEffects::None;
        }
        // get the projectile's cue and play it
        projectileCue_ = AudioManager::GetCue(item_->TravelingCueName);
        if (projectileCue_ != nullptr) {
            projectileCue_->Play();
        }
    } break;

    case CombatActionStage::Executing: {
        // play the animation
        item_->SpellSprite->PlayAnimation("Impact");
        // stop the projectile sound effect
        if (projectileCue_ != nullptr) {
            projectileCue_->Stop(AudioStopOptions::Immediate);
        }
        // apply the item effect to the primary target
        bool damagedAnyone = ApplyItem(*Target);
        // apply the item effect to the secondary targets
        for (Combatant* targetCombatant : CombatEngine::SecondaryTargetedCombatants()) {
            // skip any dead or dying combatants
            if (targetCombatant->IsDeadOrDying()) {
                continue;
            }
            // apply the effect
            damagedAnyone = ApplyItem(*targetCombatant) || damagedAnyone;
        }
        // play the impact sound effect
        if (damagedAnyone) {
            AudioManager::PlayCue(item_->ImpactCueName);
            if (item_->Overlay != nullptr) {
                item_->Overlay->PlayAnimation(0);
                item_->Overlay->ResetAnimation();
            }
        }
    } break;

    case CombatActionStage::Returning:
        // play the animation
        combatant_->GetCombatSprite().PlayAnimation("Idle");
        break;

    case CombatActionStage::Finishing:
        // play the animation
        combatant_->GetCombatSprite().PlayAnimation("Idle");
        break;

    case CombatActionStage::Complete:
        // play the animation
        combatant_->GetCombatSprite().PlayAnimation("Idle");
        break;

    default:
        break;
    }
}

// ---- Hud methods that depend on CombatEngine ----

inline void Hud::Draw() {
    SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();

    spriteBatch.Begin();

    startingInfoPosition_.X = 640.0f;

    startingInfoPosition_.X -= (float)(Session::GetParty()->Players.size() / 2) * 200.0f;
    if (Session::GetParty()->Players.size() % 2 != 0) {
        startingInfoPosition_.X -= 100.0f;
    }

    spriteBatch.Draw(backgroundHudTexture_, backgroundHudPosition_, Color::White);

    if (CombatEngine::IsActive()) {
        DrawForCombat();
    } else {
        DrawForNonCombat();
    }

    spriteBatch.End();
}

inline void Hud::DrawForCombat() {
    SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();
    Vector2 position = startingInfoPosition_;

    for (const std::shared_ptr<CombatantPlayer>& combatantPlayer : CombatEngine::Players()) {
        DrawCombatPlayerDetails(*combatantPlayer, position);
        position.X += (float)activeCharInfoTexture_.getWidthProperty() - 6.0f;
    }

    charSelLeftPosition_.X =
        startingInfoPosition_.X - 5.0f - (float)charSelArrowLeftTexture_.getWidthProperty();
    charSelRightPosition_.X = position.X + 5.0f;
    // Draw character Selection Arrows
    if (CombatEngine::IsPlayersTurn()) {
        spriteBatch.Draw(charSelArrowLeftTexture_, charSelLeftPosition_, Color::White);
        spriteBatch.Draw(charSelArrowRightTexture_, charSelRightPosition_, Color::White);
    } else {
        spriteBatch.Draw(charSelFadeLeftTexture_, charSelLeftPosition_, Color::White);
        spriteBatch.Draw(charSelFadeRightTexture_, charSelRightPosition_, Color::White);
    }

    if (!actionText_.empty()) {
        spriteBatch.Draw(topHudTexture_, topHudPosition_, Color::White);
        // Draw Action Text
        Fonts::DrawCenteredText(spriteBatch, Fonts::PlayerStatisticsFont(), actionText_,
                                actionTextPosition_, Color::Black);
    }
}

inline void Hud::DrawForNonCombat() {
    SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();

    Vector2 position = startingInfoPosition_;

    for (const std::shared_ptr<Player>& player : Session::GetParty()->Players) {
        DrawNonCombatPlayerDetails(*player, position);

        position.X += (float)inActiveCharInfoTexture_.getWidthProperty() - 6.0f;
    }

    yTextPosition_.X = position.X + 5.0f;
    yButtonPosition_.X = position.X + 9.0f;

    // Draw Select Button
    spriteBatch.Draw(statsTexture_, yTextPosition_, Color::White);
    spriteBatch.Draw(yButtonTexture_, yButtonPosition_, Color::White);

    startTextPosition_.X =
        startingInfoPosition_.X - (float)startButtonTexture_.getWidthProperty() - 25.0f;
    startButtonPosition_.X =
        startingInfoPosition_.X - (float)startButtonTexture_.getWidthProperty() - 10.0f;

    // Draw Back Button
    spriteBatch.Draw(menuTexture_, startTextPosition_, Color::White);
    spriteBatch.Draw(startButtonTexture_, startButtonPosition_, Color::White);
}

inline void Hud::DrawCombatPlayerDetails(CombatantPlayer& player, Vector2 position) {
    SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();

    PlankState plankState;
    bool isPortraitActive = false;
    bool isCharDead = false;
    Color color;

    portraitPosition_.X = position.X + 7.0f;
    portraitPosition_.Y = position.Y + 7.0f;

    namePosition_.X = position.X + 84.0f;
    namePosition_.Y = position.Y + 12.0f;

    levelPosition_.X = position.X + 84.0f;
    levelPosition_.Y = position.Y + 39.0f;

    detailPosition_.X = position.X + 25.0f;
    detailPosition_.Y = position.Y + 66.0f;

    position.X -= 2.0f;
    position.Y -= 4.0f;

    if (player.IsTurnTaken()) {
        plankState = PlankState::CantUse;

        isPortraitActive = false;
    } else {
        plankState = PlankState::InActive;

        isPortraitActive = true;
    }

    bool isSecondaryTarget = false;
    for (Combatant* combatant : CombatEngine::SecondaryTargetedCombatants()) {
        if (combatant == &player) {
            isSecondaryTarget = true;
            break;
        }
    }
    if ((CombatEngine::HighlightedCombatant() == &player && !player.IsTurnTaken()) ||
        CombatEngine::PrimaryTargetedCombatant() == &player || isSecondaryTarget) {
        plankState = PlankState::Active;
    }

    if (player.IsDeadOrDying()) {
        isCharDead = true;
        isPortraitActive = false;
        plankState = PlankState::CantUse;
    }

    // Draw Info Slab
    if (plankState == PlankState::Active) {
        color = activeNameColor_;

        spriteBatch.Draw(activeCharInfoTexture_, position, Color::White);

        // Draw Brackets
        if (CombatEngine::HighlightedCombatant() == &player && !player.IsTurnTaken()) {
            spriteBatch.Draw(selectionBracketTexture_, position, Color::White);
        }

        if (isPortraitActive && CombatEngine::HighlightedCombatant() == &player &&
            CombatEngine::HighlightedCombatant()->GetCombatAction() == nullptr &&
            !CombatEngine::IsDelaying()) {
            position.X += (float)(activeCharInfoTexture_.getWidthProperty() / 2);
            position.X -= (float)(combatPopupTexture_.getWidthProperty() / 2);
            position.Y -= (float)combatPopupTexture_.getHeightProperty();
            // Draw Action
            DrawActionsMenu(position);
        }
    } else if (plankState == PlankState::InActive) {
        color = inActiveNameColor_;
        spriteBatch.Draw(inActiveCharInfoTexture_, position, Color::White);
    } else {
        color = Color::Black;
        spriteBatch.Draw(cantUseCharInfoTexture_, position, Color::White);
    }

    if (isCharDead) {
        spriteBatch.Draw(deadPortraitTexture_, portraitPosition_, Color::White);
    } else {
        // Draw Player Portrait
        DrawPortrait(*player.GetPlayer(), portraitPosition_, plankState);
    }

    // Draw Player Name
    spriteBatch.DrawString(Fonts::PlayerStatisticsFont(), player.GetPlayer()->Name(),
                           namePosition_, color);

    color = Color::Black;
    // Draw Player Details
    spriteBatch.DrawString(
        Fonts::HudDetailFont(),
        "Lvl: " + System::Int32::ToString(player.GetPlayer()->CharacterLevel()), levelPosition_,
        color);

    spriteBatch.DrawString(
        Fonts::HudDetailFont(),
        "HP: " + System::Int32::ToString(player.Statistics().HealthPoints) + "/" +
            System::Int32::ToString(player.GetPlayer()->CharacterStatistics().HealthPoints),
        detailPosition_, color);

    detailPosition_.Y += 30.0f;
    spriteBatch.DrawString(
        Fonts::HudDetailFont(),
        "MP: " + System::Int32::ToString(player.Statistics().MagicPoints) + "/" +
            System::Int32::ToString(player.GetPlayer()->CharacterStatistics().MagicPoints),
        detailPosition_, color);
}

inline void Hud::UpdateActionsMenu() {
    // cursor up
    if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
        if (highlightedAction_ > 0) {
            highlightedAction_--;
        }
        return;
    }
    // cursor down
    if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
        if (highlightedAction_ < (int)actionList_.size() - 1) {
            highlightedAction_++;
        }
        return;
    }
    // select an action
    if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
        const std::string& action = actionList_[(std::size_t)highlightedAction_];
        if (action == "Attack") {
            SetActionText("Performing a Melee Attack");
            CombatEngine::HighlightedCombatant()->SetCombatAction(
                std::make_shared<MeleeCombatAction>(*CombatEngine::HighlightedCombatant()));
            CombatEngine::HighlightedCombatant()->GetCombatAction()->Target =
                CombatEngine::FirstEnemyTarget();
        } else if (action == "Spell") {
            auto spellbookScreen = std::make_shared<SpellbookScreen>(
                CombatEngine::HighlightedCombatant()->GetCharacterShared(),
                CombatEngine::HighlightedCombatant()->Statistics());
            spellbookScreen->SpellSelected = [this](const std::shared_ptr<Spell>& spell) {
                SpellbookScreenSpellSelected(spell);
            };
            Session::GetScreenManager()->AddScreen(spellbookScreen);
        } else if (action == "Item") {
            auto inventoryScreen = std::make_shared<InventoryScreen>(true);
            inventoryScreen->GearSelected = [this](const std::shared_ptr<Gear>& gear) {
                InventoryScreenGearSelected(gear);
            };
            Session::GetScreenManager()->AddScreen(inventoryScreen);
        } else if (action == "Defend") {
            SetActionText("Defending");
            CombatEngine::HighlightedCombatant()->SetCombatAction(
                std::make_shared<DefendCombatAction>(*CombatEngine::HighlightedCombatant()));
            CombatEngine::HighlightedCombatant()->GetCombatAction()->Start();
        } else if (action == "Flee") {
            CombatEngine::AttemptFlee();
        }
        return;
    }
}

inline void Hud::SpellbookScreenSpellSelected(const std::shared_ptr<Spell>& spell) {
    if (spell != nullptr) {
        SetActionText("Casting " + spell->Name);
        CombatEngine::HighlightedCombatant()->SetCombatAction(
            std::make_shared<SpellCombatAction>(*CombatEngine::HighlightedCombatant(), spell));
        if (spell->IsOffensive) {
            CombatEngine::HighlightedCombatant()->GetCombatAction()->Target =
                CombatEngine::FirstEnemyTarget();
        } else {
            CombatEngine::HighlightedCombatant()->GetCombatAction()->Target =
                CombatEngine::HighlightedCombatant();
        }
    }
}

inline void Hud::InventoryScreenGearSelected(const std::shared_ptr<Gear>& gear) {
    auto item = std::dynamic_pointer_cast<RolePlayingGameData::Item>(gear);
    if (item != nullptr) {
        SetActionText("Using " + item->Name);
        CombatEngine::HighlightedCombatant()->SetCombatAction(
            std::make_shared<ItemCombatAction>(*CombatEngine::HighlightedCombatant(), item));
        if (item->IsOffensive) {
            CombatEngine::HighlightedCombatant()->GetCombatAction()->Target =
                CombatEngine::FirstEnemyTarget();
        } else {
            CombatEngine::HighlightedCombatant()->GetCombatAction()->Target =
                CombatEngine::HighlightedCombatant();
        }
    }
}

// ---- TileEngine methods that depend on Session ----

inline void TileEngine::Update(const GameTime& gameTime) {
    Vector2 autoMovement = UpdatePartyLeaderAutoMovement();

    Vector2 userMovement = Vector2::Zero;
    if (autoMovement == Vector2::Zero) {
        userMovement = UpdateUserMovement();
        if (userMovement != Vector2::Zero) {
            Point desiredTilePosition = partyLeaderPosition_.TilePosition;
            Vector2 desiredTileOffset = partyLeaderPosition_.TileOffset;
            PlayerPosition::CalculateMovement(Vector2::Multiply(userMovement, 15.0f), desiredTilePosition,
                                              desiredTileOffset);
            if (partyLeaderPosition_.TilePosition != desiredTilePosition && !MoveIntoTile(desiredTilePosition)) {
                userMovement = Vector2::Zero;
            }
        }
    }

    Point oldTilePosition = partyLeaderPosition_.TilePosition;
    Vector2 combinedMovement = autoMovement + userMovement;
    partyLeaderPosition_.Move(combinedMovement);

    if (autoMovement == Vector2::Zero && partyLeaderPosition_.TilePosition != oldTilePosition) {
        Session::CheckForRandomCombat(map_->RandomCombatData);
    }

    auto* party = Session::GetParty();
    Vector2 leaderOffset =
        party && !party->Players.empty() && party->Players[0]->MapSprite ? party->Players[0]->MapSprite->SourceOffset : Vector2::Zero;
    mapOriginPosition_ = mapOriginPosition_ + (viewportCenter_ - (partyLeaderPosition_.ScreenPosition() + leaderOffset));

    mapOriginPosition_.X = Microsoft::Xna::Framework::MathHelper::Min(mapOriginPosition_.X, (float)viewport_.getXProperty());
    mapOriginPosition_.Y = Microsoft::Xna::Framework::MathHelper::Min(mapOriginPosition_.Y, (float)viewport_.getYProperty());
    mapOriginPosition_.X += Microsoft::Xna::Framework::MathHelper::Max(
        (float)(viewport_.getXProperty() + viewport_.getWidthProperty()) -
            (mapOriginPosition_.X + map_->MapDimensions.X * map_->TileSize.X),
        0.0f);
    mapOriginPosition_.Y += Microsoft::Xna::Framework::MathHelper::Max(
        (float)(viewport_.getYProperty() + viewport_.getHeightProperty() - Hud::HudHeight) -
            (mapOriginPosition_.Y + map_->MapDimensions.Y * map_->TileSize.Y),
        0.0f);
}

inline bool TileEngine::MoveIntoTile(Point mapPosition) {
    if (map_->IsBlocked(mapPosition)) return false;
    if (Session::EncounterTile(mapPosition)) return false;
    return true;
}

// ---- Party methods that depend on Session/LevelUpScreen ----

inline void Party::GiveExperience(int experience) {
    // check the parameters
    if (experience < 0) {
        throw System::ArgumentOutOfRangeException("experience");
    } else if (experience == 0) {
        return;
    }
    std::vector<std::shared_ptr<Player>> leveledUp;
    for (auto& player : Players) {
        int oldLevel = player->CharacterLevel();
        player->SetExperience(player->Experience() + experience);
        if (player->CharacterLevel() > oldLevel) leveledUp.push_back(player);
    }
    if (!leveledUp.empty()) Session::GetScreenManager()->AddScreen(std::make_shared<LevelUpScreen>(leveledUp));
}

} // namespace RolePlaying
