#pragma once

// CombatAction.hpp -- C++ port of Combat/Actions/CombatAction.cs.

#include <memory>

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "System/ArgumentNullException.hpp"

#include "../Combatant.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;

// An action taken by an individual character in combat.
//
// Note that party actions, like Flee, are not represented here. These are only the actions that
// individual characters, on either side, can perform in combat.
class CombatAction {
public:
    // Stages of the action as it is executed.
    enum class CombatActionStage {
        // The initial state, with no action taken yet.
        NotStarted,

        // The action is getting ready to start. Spell actions stay in this stage while the
        // casting animation plays.
        Preparing,

        // The effect is traveling to the target, if needed. The character walks to melee targets
        // while in this stage; spell effects also travel to the target while in this state.
        Advancing,

        // The action is being applied to the target(s).
        Executing,

        // The effect is returning from the target, if needed. The character walks back from the
        // melee target while in this stage.
        Returning,

        // The action is performing any final actions.
        Finishing,

        // The action is complete.
        Complete,
    };

    virtual ~CombatAction() = default;

    // Returns true if the action is offensive, targeting the opponents.
    virtual bool IsOffensive() const = 0;

    // Returns true if this action requires a target.
    virtual bool IsTargetNeeded() const = 0;

    // The current state of the action.
    CombatActionStage Stage() const { return stage_; }

    // The character performing this action.
    Combatant& GetCombatant() const { return *combatant_; }

    // Returns true if the character can use this action.
    virtual bool IsCharacterValidUser() const { return true; }

    // The target of the action.
    Combatant* Target = nullptr;

    // The number of adjacent targets in each direction that are affected.
    int AdjacentTargets() const { return adjacentTargets_; }

    // The heuristic used to compare actions of this type to similar ones.
    virtual int Heuristic() const = 0;

    // Compares the combat actions by their heuristic, in descending order.
    static bool CompareCombatActionsByHeuristic(const std::shared_ptr<CombatAction>& a,
                                                const std::shared_ptr<CombatAction>& b) {
        return a->Heuristic() > b->Heuristic();
    }

    // Reset the action so that it may be started again.
    virtual void Reset() {
        // set the state to not-started
        stage_ = CombatActionStage::NotStarted;
    }

    // Start executing the combat action.
    virtual void Start() {
        // set the state to the first step
        stage_ = CombatActionStage::Preparing;
        StartStage();
    }

    // Updates the action over time.
    virtual void Update(const GameTime& gameTime) {
        // update the current stage
        UpdateCurrentStage(gameTime);

        // if the action is ready for the next stage, then advance
        if (stage_ != CombatActionStage::NotStarted && stage_ != CombatActionStage::Complete &&
            IsReadyForNextStage()) {
            switch (stage_) {
            case CombatActionStage::Preparing:
                stage_ = CombatActionStage::Advancing;
                break;

            case CombatActionStage::Advancing:
                stage_ = CombatActionStage::Executing;
                break;

            case CombatActionStage::Executing:
                stage_ = CombatActionStage::Returning;
                break;

            case CombatActionStage::Returning:
                stage_ = CombatActionStage::Finishing;
                break;

            case CombatActionStage::Finishing:
                stage_ = CombatActionStage::Complete;
                break;

            default:
                break;
            }
            StartStage();
        }
    }

    // Draw any elements of the action that are independent of the character.
    virtual void Draw(const GameTime& gameTime, SpriteBatch& spriteBatch) {
        (void)gameTime;
        (void)spriteBatch;
    }

protected:
    // Constructs a new CombatAction object.
    explicit CombatAction(Combatant& combatant) : combatant_(&combatant) { CombatAction::Reset(); }

    // Starts a new combat stage. Called right after the stage changes. The stage never changes
    // into NotStarted.
    virtual void StartStage() {}

    // Update the action for the current stage. This function is guaranteed to be called at least
    // once per stage.
    virtual void UpdateCurrentStage(const GameTime& gameTime) { (void)gameTime; }

    // Returns true if the combat action is ready to proceed to the next stage.
    virtual bool IsReadyForNextStage() const {
        // the action doesn't care about the state, so move on
        return true;
    }

    // The current state of the action.
    CombatActionStage stage_ = CombatActionStage::NotStarted;

    // The character performing this action.
    Combatant* combatant_;

    // The number of adjacent targets in each direction that are affected.
    int adjacentTargets_ = 0;
};

} // namespace RolePlaying
