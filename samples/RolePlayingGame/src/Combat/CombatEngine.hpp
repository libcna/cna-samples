#pragma once

// CombatEngine.hpp -- C++ port of Combat/CombatEngine.cs.
//
// StartNewCombat/EndCombat/HandleInput need Session (for the party, the HUD and the reward and
// game-over screens), and Session's encounter handlers need CombatEngine, so the members that
// close that cycle are defined in RolePlayingGame.hpp, the same way every other cross-reference
// in this port is resolved.

#include <algorithm>
#include <climits>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentException.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"
#include "System/InvalidOperationException.hpp"

#include "../AudioManager.hpp"
#include "../Data/AnimatingSprite.hpp"
#include "../Data/Animation.hpp"
#include "../Data/Characters/Monster.hpp"
#include "../Data/Map/FixedCombat.hpp"
#include "../Data/Map/RandomCombat.hpp"
#include "../Data/MapEntry.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "Actions/CombatAction.hpp"
#include "Actions/ItemCombatAction.hpp"
#include "Actions/SpellCombatAction.hpp"
#include "ArtificialIntelligence.hpp"
#include "CombatEndingState.hpp"
#include "CombatantMonster.hpp"
#include "CombatantPlayer.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::Animation;
using RolePlayingGameData::FixedCombat;
using RolePlayingGameData::MapEntry;
using RolePlayingGameData::Monster;
using RolePlayingGameData::RandomCombat;

// The runtime execution engine for the combat system.
class CombatEngine {
public:
    // Declared, not defaulted, because the singleton below is a unique_ptr<CombatEngine> held
    // inside CombatEngine itself: its deleter needs the complete type, which only exists after
    // the class. Defined at the bottom of this header.
    ~CombatEngine();

    // If true, the combat engine is active and the user is in combat.
    static bool IsActive() { return singleton_ != nullptr; }

    // If true, it is currently the players' turn.
    static bool IsPlayersTurn() {
        CheckSingleton();
        return singleton_->isPlayersTurn_;
    }

    // The fixed combat used to generate this fight, if any. Used for rewards; null means it was
    // a random fight with no special rewards.
    static std::shared_ptr<MapEntry<FixedCombat>> FixedCombatEntry() {
        return singleton_ == nullptr ? nullptr : singleton_->fixedCombatEntry_;
    }

    // The players involved in the current combat.
    static std::vector<std::shared_ptr<CombatantPlayer>>& Players() {
        CheckSingleton();
        return singleton_->players_;
    }

    // The monsters involved in the current combat.
    static std::vector<std::shared_ptr<CombatantMonster>>& Monsters() {
        CheckSingleton();
        return singleton_->monsters_;
    }

    // The currently highlighted combatant, if any.
    static Combatant* HighlightedCombatant() {
        CheckSingleton();
        return singleton_->highlightedCombatant_;
    }

    // The current primary target, if any.
    static Combatant* PrimaryTargetedCombatant() {
        CheckSingleton();
        return singleton_->primaryTargetedCombatant_;
    }

    // The current secondary targets, if any.
    static std::vector<Combatant*>& SecondaryTargetedCombatants() {
        CheckSingleton();
        return singleton_->secondaryTargetedCombatants_;
    }

    // Retrieves the first living enemy, if any.
    static Combatant* FirstEnemyTarget() {
        CheckSingleton();

        if (IsPlayersTurn()) {
            return singleton_->FirstMonsterTarget();
        } else {
            return singleton_->FirstPlayerTarget();
        }
    }

    // Retrieves the first living ally, if any.
    static Combatant* FirstAllyTarget() {
        CheckSingleton();

        if (IsPlayersTurn()) {
            return singleton_->FirstPlayerTarget();
        } else {
            return singleton_->FirstMonsterTarget();
        }
    }

    // Adds a new damage combat effect to the scene.
    static void AddNewDamageEffects(const Vector2& position, const StatisticsValue& damage) {
        CheckSingleton();
        AddNewEffects(singleton_->damageCombatEffects_, position, damage);
    }

    // Adds a new healing combat effect to the scene.
    static void AddNewHealingEffects(const Vector2& position, const StatisticsValue& healing) {
        CheckSingleton();
        AddNewEffects(singleton_->healingCombatEffects_, position, healing);
    }

    // Returns true if the combat engine is delaying for any reason.
    static bool IsDelaying() {
        return singleton_ == nullptr ? false : singleton_->delayType_ != DelayType::NoDelay;
    }

    // Start a new combat from the given FixedCombat object.
    // Defined in RolePlayingGame.hpp -- it reads the party through Session.
    static void StartNewCombat(const std::shared_ptr<MapEntry<FixedCombat>>& fixedCombatEntry);

    // Start a new combat from the given RandomCombat object.
    // Defined in RolePlayingGame.hpp -- it reads the party through Session.
    static void StartNewCombat(const std::shared_ptr<RandomCombat>& randomCombat);

    // Begin an attempt to flee the combat.
    // Defined in RolePlayingGame.hpp -- it writes the HUD action text through Session.
    static void AttemptFlee();

    // Ensure that there is no combat happening right now.
    static void ClearCombat() {
        // clear the singleton
        if (singleton_ != nullptr) {
            RetireSingleton();
        }
        retiredSingleton_.reset();
    }

    // Update the combat engine for this frame.
    static void Update(const GameTime& gameTime) {
        // if there is no active combat, then there's nothing to update
        // -- this will be called every frame, so there should be no exception for calling this
        //    method outside of combat
        if (singleton_ == nullptr) {
            return;
        }

        // update the singleton
        singleton_->UpdateCombatEngine(gameTime);
    }

    // Draw the combat for this frame.
    static void Draw(const GameTime& gameTime) {
        // if there is no active combat, then there's nothing to draw
        if (singleton_ == nullptr) {
            return;
        }

        // update the singleton
        singleton_->DrawCombatEngine(gameTime);
    }

private:
    // A combat effect sprite, typically used for damage or healing numbers.
    class CombatEffect {
    public:
        // The starting position of the effect on the screen.
        Vector2 OriginalPosition;

        // The current position of the effect on the screen.
        Vector2 Position() const { return position_; }

        // The text that appears on top of the effect.
        const std::string& Text() const { return text_; }
        void SetText(const std::string& value) {
            text_ = value;
            // recalculate the origin
            if (text_.empty()) {
                textOrigin_ = Vector2::Zero;
            } else {
                Vector2 textSize = Fonts::DamageFont().MeasureString(text_);
                textOrigin_ = Vector2(std::ceil(textSize.X / 2.0f), std::ceil(textSize.Y / 2.0f));
            }
        }

        // The amount which the effect has already risen on the screen.
        float Rise = 0.0f;

        // If true, the effect has finished rising.
        bool IsRiseComplete() const { return isRiseComplete_; }

        // Updates the combat effect.
        void Update(float elapsedSeconds) {
            if (!isRiseComplete_) {
                Rise += (float)RisePerSecond * elapsedSeconds;
                if (Rise > (float)RiseMaximum) {
                    Rise = (float)RiseMaximum;
                    isRiseComplete_ = true;
                }
                position_ = Vector2(OriginalPosition.X, OriginalPosition.Y - Rise);
            }
        }

        // Draw the combat effect.
        void Draw(SpriteBatch& spriteBatch, Texture2D& texture) {
            // draw the texture
            spriteBatch.Draw(texture, position_, std::nullopt, Color::White, 0.0f,
                             Vector2((float)(texture.getWidthProperty() / 2),
                                     (float)(texture.getHeightProperty() / 2)),
                             1.0f, SpriteEffects::None, 0.3f * Rise / 200.0f);
            // draw the text
            if (!text_.empty()) {
                spriteBatch.DrawString(Fonts::DamageFont(), text_, position_, Color::White, 0.0f,
                                       Vector2(textOrigin_.X, textOrigin_.Y), 1.0f,
                                       SpriteEffects::None, 0.2f * Rise / 200.0f);
            }
        }

    private:
        // The speed at which the effect rises on the screen.
        static constexpr int RisePerSecond = 100;

        // The amount which the effect rises on the screen.
        static constexpr int RiseMaximum = 80;

        Vector2 position_;
        std::string text_;
        Vector2 textOrigin_;
        bool isRiseComplete_ = false;
    };

    // Varieties of delays that are interspersed throughout the combat flow.
    enum class DelayType {
        // No delay at this time.
        NoDelay,
        // Delay at the start of combat.
        StartCombat,
        // Delay when one side turn's ends before the other side begins.
        EndRound,
        // Delay at the end of a character's turn before the next one begins.
        EndCharacterTurn,
        // Delay before a flee is attempted.
        FleeAttempt,
        // Delay when the party has fled from combat before combat ends.
        FleeSuccessful,
    };

    friend class Combatant;
    friend class ArtificialIntelligence;
    friend class SpellCombatAction;
    friend class ItemCombatAction;

    // Construct a new CombatEngine object.
    // Defined in RolePlayingGame.hpp -- it reads the map's combat music through TileEngine.
    CombatEngine(std::vector<std::shared_ptr<CombatantPlayer>> players,
                 std::vector<std::shared_ptr<CombatantMonster>> monsters, int fleeThreshold);

    // Check to see if there is a combat going on, and throw an exception if not.
    static void CheckSingleton() {
        if (singleton_ == nullptr) {
            throw System::InvalidOperationException("There is no active combat at this time.");
        }
    }

    // The positions of the players on screen.
    static const Vector2* PlayerPositions() {
        static const Vector2 positions[PlayerPositionCount] = {
            Vector2(850.0f, 345.0f), Vector2(980.0f, 260.0f), Vector2(940.0f, 440.0f),
            Vector2(1100.0f, 200.0f), Vector2(1100.0f, 490.0f)};
        return positions;
    }

    // The positions of the monsters on the screen.
    static const Vector2* MonsterPositions() {
        static const Vector2 positions[MonsterPositionCount] = {
            Vector2(480.0f, 345.0f), Vector2(345.0f, 260.0f), Vector2(370.0f, 440.0f),
            Vector2(225.0f, 200.0f), Vector2(225.0f, 490.0f)};
        return positions;
    }

    static constexpr int PlayerPositionCount = 5;
    static constexpr int MonsterPositionCount = 5;

    // Start the given player's combat turn.
    // Defined in RolePlayingGame.hpp -- it writes the HUD action text through Session.
    void BeginPlayerTurn(const std::shared_ptr<CombatantPlayer>& player);

    // Begin the players' turn in this combat round.
    // Defined in RolePlayingGame.hpp -- it writes the HUD action text through Session.
    void BeginPlayersTurn();

    // Check for whether all players have taken their turn.
    bool IsPlayersTurnComplete() const {
        return std::all_of(players_.begin(), players_.end(),
                           [](const std::shared_ptr<CombatantPlayer>& player) {
                               return player->IsTurnTaken() || player->IsDeadOrDying();
                           });
    }

    // Check for whether the players have been wiped out and defeated.
    bool ArePlayersDefeated() const {
        return std::all_of(players_.begin(), players_.end(),
                           [](const std::shared_ptr<CombatantPlayer>& player) {
                               return player->GetState() == Character::CharacterState::Dead;
                           });
    }

    // Retrieves the first living player, if any.
    CombatantPlayer* FirstPlayerTarget() const {
        // if there are no living players, then this is moot
        if (ArePlayersDefeated()) {
            return nullptr;
        }

        std::size_t playerIndex = 0;
        while (playerIndex < players_.size() && players_[playerIndex]->IsDeadOrDying()) {
            playerIndex++;
        }
        return players_[playerIndex].get();
    }

    // Start the given monster's combat turn.
    void BeginMonsterTurn(const std::shared_ptr<CombatantMonster>& chosen) {
        std::shared_ptr<CombatantMonster> monster = chosen;

        // if it's null, find a random living monster who has yet to take their turn
        if (monster == nullptr) {
            // don't bother if all monsters have finished
            if (IsMonstersTurnComplete()) {
                return;
            }
            // pick random living monsters who haven't taken their turn
            do {
                monster = monsters_[(std::size_t)Session::GetRandom().Next(
                    (int)monsters_.size())];
            } while (monster->IsTurnTaken() || monster->IsDeadOrDying());
        }

        // set the highlight sprite
        highlightedCombatant_ = monster.get();
        primaryTargetedCombatant_ = nullptr;
        secondaryTargetedCombatants_.clear();

        // choose the action immediately
        monster->SetCombatAction(monster->GetArtificialIntelligence().ChooseAction());
    }

    // Begin the monsters' turn in this combat round.
    // Defined in RolePlayingGame.hpp -- it writes the HUD action text through Session.
    void BeginMonstersTurn();

    // Check for whether all monsters have taken their turn.
    bool IsMonstersTurnComplete() const {
        return std::all_of(monsters_.begin(), monsters_.end(),
                           [](const std::shared_ptr<CombatantMonster>& monster) {
                               return monster->IsTurnTaken() || monster->IsDeadOrDying();
                           });
    }

    // Check for whether the monsters have been wiped out and defeated.
    bool AreMonstersDefeated() const {
        return std::all_of(monsters_.begin(), monsters_.end(),
                           [](const std::shared_ptr<CombatantMonster>& monster) {
                               return monster->GetState() == Character::CharacterState::Dead;
                           });
    }

    // Retrieves the first living monster, if any.
    CombatantMonster* FirstMonsterTarget() const {
        // if there are no living monsters, then this is moot
        if (AreMonstersDefeated()) {
            return nullptr;
        }

        std::size_t monsterIndex = 0;
        while (monsterIndex < monsters_.size() && monsters_[monsterIndex]->IsDeadOrDying()) {
            monsterIndex++;
        }
        return monsters_[monsterIndex].get();
    }

    // Index of a combatant in the player roster, or -1.
    int IndexOfPlayer(const Combatant* combatant) const {
        for (int i = 0; i < (int)players_.size(); i++) {
            if (players_[(std::size_t)i].get() == combatant) return i;
        }
        return -1;
    }

    // Index of a combatant in the monster roster, or -1.
    int IndexOfMonster(const Combatant* combatant) const {
        for (int i = 0; i < (int)monsters_.size(); i++) {
            if (monsters_[(std::size_t)i].get() == combatant) return i;
        }
        return -1;
    }

    // Set the primary and any secondary targets.
    void SetTargets(Combatant* primaryTarget, int adjacentTargets) {
        // set the primary target
        primaryTargetedCombatant_ = primaryTarget;

        // set any secondary targets
        secondaryTargetedCombatants_.clear();
        if (primaryTarget != nullptr && adjacentTargets > 0) {
            // find out which side is targeted
            bool isPlayerTarget = dynamic_cast<CombatantPlayer*>(primaryTarget) != nullptr;
            // find the index
            int primaryTargetIndex = -1;
            if (isPlayerTarget) {
                for (int i = 0; i < (int)players_.size(); i++) {
                    if (players_[(std::size_t)i].get() == primaryTarget) {
                        primaryTargetIndex = i;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < (int)monsters_.size(); i++) {
                    if (monsters_[(std::size_t)i].get() == primaryTarget) {
                        primaryTargetIndex = i;
                        break;
                    }
                }
            }
            if (primaryTargetIndex < 0) {
                return;
            }
            // add the surrounding indices
            for (int i = 1; i <= adjacentTargets; i++) {
                int leftIndex = primaryTargetIndex - i;
                if (leftIndex >= 0) {
                    secondaryTargetedCombatants_.push_back(
                        isPlayerTarget ? (Combatant*)players_[(std::size_t)leftIndex].get()
                                       : (Combatant*)monsters_[(std::size_t)leftIndex].get());
                }
                int rightIndex = primaryTargetIndex + i;
                if (rightIndex < (int)(isPlayerTarget ? players_.size() : monsters_.size())) {
                    secondaryTargetedCombatants_.push_back(
                        isPlayerTarget ? (Combatant*)players_[(std::size_t)rightIndex].get()
                                       : (Combatant*)monsters_[(std::size_t)rightIndex].get());
                }
            }
        }
    }

    // The shared body behind AddNewDamageEffects and AddNewHealingEffects: one rising number per
    // non-zero statistic, in the original's order.
    static void AddNewEffects(std::vector<CombatEffect>& effects, const Vector2& position,
                              const StatisticsValue& statistics) {
        int startingRise = 0;

        auto add = [&](const char* label, int value) {
            if (value == 0) return;
            CombatEffect combatEffect;
            combatEffect.OriginalPosition = position;
            combatEffect.SetText(std::string(label) + "\n" + System::Int32::ToString(value));
            combatEffect.Rise = (float)startingRise;
            startingRise -= 5;
            effects.push_back(combatEffect);
        };

        add("HP", statistics.HealthPoints);
        add("MP", statistics.MagicPoints);
        add("PO", statistics.PhysicalOffense);
        add("PD", statistics.PhysicalDefense);
        add("MO", statistics.MagicalOffense);
        add("MD", statistics.MagicalDefense);
    }

    // Load the graphics data for the combat effect sprites.
    // Defined in RolePlayingGame.hpp -- it loads through Session's ScreenManager.
    void CreateCombatEffectSprites();

    // Draw all combat effect sprites.
    // Defined in RolePlayingGame.hpp -- it draws through Session's ScreenManager.
    void DrawCombatEffects(const GameTime& gameTime);

    // Create the selection sprite objects.
    // Defined in RolePlayingGame.hpp -- it loads through Session's ScreenManager.
    void CreateSelectionSprites();

    // Draw the highlight sprites.
    // Defined in RolePlayingGame.hpp -- it draws through Session's ScreenManager.
    void DrawSelectionSprites(const GameTime& gameTime);

    // Update any delays in the combat system. This function may cause combat to end, clearing
    // the singleton.
    // Defined in RolePlayingGame.hpp -- the flee arms write the HUD action text.
    void UpdateDelay(int elapsedMilliseconds);

    // Calculate an attempted escape from the combat. If true, the escape succeeds.
    bool CalculateFleeAttempt() const {
        return Session::GetRandom().Next(100) < fleeThreshold_;
    }

    // End the combat.
    // Defined in RolePlayingGame.hpp -- it opens the reward and game-over screens.
    void EndCombat(CombatEndingState combatEndingState);

    // Update the combat engine for this frame.
    // Defined in RolePlayingGame.hpp -- HandleInput talks to the HUD.
    void UpdateCombatEngine(const GameTime& gameTime);

    // Handle player input that affects the combat engine.
    // Defined in RolePlayingGame.hpp -- it drives the HUD's action menu.
    void HandleInput();

    // Draw the combat for this frame.
    void DrawCombatEngine(const GameTime& gameTime) {
        // draw the players
        for (const std::shared_ptr<CombatantPlayer>& player : players_) {
            player->Draw(gameTime);
        }

        // draw the monsters
        for (const std::shared_ptr<CombatantMonster>& monster : monsters_) {
            monster->Draw(gameTime);
        }

        // draw the selection animations
        DrawSelectionSprites(gameTime);

        // draw the combat effects
        DrawCombatEffects(gameTime);
    }

    // Ends the current combat without destroying the object the caller may be executing inside.
    // C# just drops the reference and lets the collector take it later; the nearest C++ shape is
    // to hold the retired engine until the next combat starts or ClearCombat runs.
    static void RetireSingleton() { retiredSingleton_ = std::move(singleton_); }

    // The singleton of the combat engine. Defined after the class: a static inline
    // unique_ptr<Self> declared inside the class instantiates its deleter where Self is still
    // incomplete.
    static std::unique_ptr<CombatEngine> singleton_;

    // The previous singleton, kept alive until it is provably no longer executing.
    static std::unique_ptr<CombatEngine> retiredSingleton_;

    // If true, it is currently the players' turn.
    bool isPlayersTurn_ = false;

    // The fixed combat used to generate this fight, if any.
    std::shared_ptr<MapEntry<FixedCombat>> fixedCombatEntry_;

    // The players involved in the current combat.
    std::vector<std::shared_ptr<CombatantPlayer>> players_;

    int highlightedPlayer_ = 0;

    // The monsters involved in the current combat.
    std::vector<std::shared_ptr<CombatantMonster>> monsters_;

    // The currently highlighted combatant, if any.
    Combatant* highlightedCombatant_ = nullptr;

    // The current primary target, if any.
    Combatant* primaryTargetedCombatant_ = nullptr;

    // The current secondary targets, if any.
    std::vector<Combatant*> secondaryTargetedCombatants_;

    // The sprite texture for all damage combat effects.
    Texture2D damageCombatEffectTexture_;

    // All current damage combat effects.
    std::vector<CombatEffect> damageCombatEffects_;

    // The sprite texture for all healing combat effects.
    Texture2D healingCombatEffectTexture_;

    // All current healing combat effects.
    std::vector<CombatEffect> healingCombatEffects_;

    // The animating sprite that draws over the highlighted character.
    AnimatingSprite highlightForegroundSprite_;

    // The animating sprite that draws behind the highlighted character.
    AnimatingSprite highlightBackgroundSprite_;

    // The animating sprite that draws behind the primary target character.
    AnimatingSprite primaryTargetSprite_;

    // The animating sprite that draws behind any secondary target characters.
    AnimatingSprite secondaryTargetSprite_;

    // The current delay, if any (otherwise NoDelay).
    DelayType delayType_ = DelayType::NoDelay;

    // The duration for all kinds of delays, in milliseconds.
    static constexpr int TotalDelay = 1000;

    // The duration of the delay so far.
    int currentDelay_ = 0;

    // The odds of being able to flee this combat, from 0 to 100.
    int fleeThreshold_ = 0;
};

inline CombatEngine::~CombatEngine() = default;

inline std::unique_ptr<CombatEngine> CombatEngine::singleton_;
inline std::unique_ptr<CombatEngine> CombatEngine::retiredSingleton_;

} // namespace RolePlaying
