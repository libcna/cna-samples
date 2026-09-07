#pragma once

// Hud.hpp -- C++ port of GameScreens/Hud.cs.
//
// UpdateActionsMenu and its two selection callbacks open the spellbook and inventory screens and
// build combat actions, all of which need CombatEngine; they are defined in RolePlayingGame.hpp
// with the rest of this port's cross-references.

#include <array>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../Data/Characters/Player.hpp"
#include "../Data/Gear/Gear.hpp"
#include "../Data/Spell.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::Gear;
using RolePlayingGameData::Player;
using RolePlayingGameData::Spell;

class CombatantPlayer; // fwd decl -- see Combat/CombatantPlayer.hpp

// Displays each player's basic statistics and the combat action menu.
class Hud {
public:
    static constexpr int HudHeight = 183;

    // Creates a new Hud object using the given ScreenManager.
    explicit Hud(ScreenManager& screenManager) : screenManager_(&screenManager) {}

    // The text that is shown in the action bar at the top of the combat screen.
    const std::string& ActionText() const { return actionText_; }
    void SetActionText(const std::string& value) { actionText_ = value; }

    // Load the graphics content from the content manager.
    void LoadContent() {
        auto& content = screenManager_->getGameProperty().getContentProperty();

        backgroundHudTexture_ = content.Load<Texture2D>("Textures/HUD/HudBkgd");
        topHudTexture_ = content.Load<Texture2D>("Textures/HUD/CombatStateInfoStrip");
        activeCharInfoTexture_ = content.Load<Texture2D>("Textures/HUD/PlankActive");
        inActiveCharInfoTexture_ = content.Load<Texture2D>("Textures/HUD/PlankInActive");
        cantUseCharInfoTexture_ = content.Load<Texture2D>("Textures/HUD/PlankCantUse");
        selectionBracketTexture_ = content.Load<Texture2D>("Textures/HUD/SelectionBrackets");
        deadPortraitTexture_ =
            content.Load<Texture2D>("Textures/Characters/Portraits/Tombstone");
        combatPopupTexture_ = content.Load<Texture2D>("Textures/HUD/CombatPopup");
        charSelFadeLeftTexture_ = content.Load<Texture2D>("Textures/Buttons/CharSelectFadeLeft");
        charSelFadeRightTexture_ =
            content.Load<Texture2D>("Textures/Buttons/CharSelectFadeRight");
        charSelArrowLeftTexture_ = content.Load<Texture2D>("Textures/Buttons/CharSelectHlLeft");
        charSelArrowRightTexture_ = content.Load<Texture2D>("Textures/Buttons/CharSelectHlRight");
        actionTexture_ = content.Load<Texture2D>("Textures/HUD/HudSelectButton");
        yButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/YButton");
        startButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/StartButton");
        menuTexture_ = content.Load<Texture2D>("Textures/HUD/Menu");
        statsTexture_ = content.Load<Texture2D>("Textures/HUD/Stats");
    }

    // Draw the screen.
    // Defined in RolePlayingGame.hpp -- it asks CombatEngine whether combat is running.
    void Draw();

    // Handle user input to the actions menu.
    // Defined in RolePlayingGame.hpp -- it builds combat actions and opens screens.
    void UpdateActionsMenu();

private:
    enum class PlankState {
        Active,
        InActive,
        CantUse,
    };

    // Draws HUD for Combat Mode.
    // Defined in RolePlayingGame.hpp -- it walks CombatEngine's roster.
    void DrawForCombat();

    // Draws HUD for non Combat Mode.
    // Defined in RolePlayingGame.hpp -- it walks the party through Session.
    void DrawForNonCombat();

    // Draws one combatant player's details.
    // Defined in RolePlayingGame.hpp -- it reads CombatEngine's highlight and targets.
    void DrawCombatPlayerDetails(CombatantPlayer& player, Vector2 position);

    // Draws one out-of-combat player's details.
    void DrawNonCombatPlayerDetails(Player& player, Vector2 position) {
        SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();

        PlankState plankState;
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

        plankState = PlankState::Active;

        // Draw Info Slab
        if (plankState == PlankState::Active) {
            color = activeNameColor_;

            spriteBatch.Draw(activeCharInfoTexture_, position, Color::White);
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
            DrawPortrait(player, portraitPosition_, plankState);
        }

        // Draw Player Name
        spriteBatch.DrawString(Fonts::PlayerStatisticsFont(), player.Name(), namePosition_, color);

        color = Color::Black;
        // Draw Player Details
        spriteBatch.DrawString(Fonts::HudDetailFont(),
                               "Lvl: " + System::Int32::ToString(player.CharacterLevel()),
                               levelPosition_, color);

        spriteBatch.DrawString(
            Fonts::HudDetailFont(),
            "HP: " + System::Int32::ToString(player.CurrentStatistics().HealthPoints) + "/" +
                System::Int32::ToString(player.CharacterStatistics().HealthPoints),
            detailPosition_, color);

        detailPosition_.Y += 30.0f;
        spriteBatch.DrawString(
            Fonts::HudDetailFont(),
            "MP: " + System::Int32::ToString(player.CurrentStatistics().MagicPoints) + "/" +
                System::Int32::ToString(player.CharacterStatistics().MagicPoints),
            detailPosition_, color);
    }

    // Draw the portrait of the given player at the given position.
    void DrawPortrait(Player& player, const Vector2& position, PlankState plankState) {
        switch (plankState) {
        case PlankState::Active:
            screenManager_->getSpriteBatch().Draw(*player.ActivePortraitTexture, position,
                                                  Color::White);
            break;
        case PlankState::InActive:
            screenManager_->getSpriteBatch().Draw(*player.InactivePortraitTexture, position,
                                                  Color::White);
            break;
        case PlankState::CantUse:
            screenManager_->getSpriteBatch().Draw(*player.UnselectablePortraitTexture, position,
                                                  Color::White);
            break;
        }
    }

    // Draws the combat action menu.
    void DrawActionsMenu(Vector2 position) {
        SetActionText("Choose an Action");

        SpriteBatch& spriteBatch = screenManager_->getSpriteBatch();

        Vector2 arrowPosition;
        float height = 25.0f;

        spriteBatch.Draw(combatPopupTexture_, position, Color::White);

        position.Y += 21.0f;
        arrowPosition = position;

        arrowPosition.X += 10.0f;
        arrowPosition.Y += 2.0f;
        arrowPosition.Y += height * (float)highlightedAction_;
        spriteBatch.Draw(actionTexture_, arrowPosition, Color::White);

        position.Y += 4.0f;
        position.X += 50.0f;

        // Draw Action Text
        for (int i = 0; i < (int)actionList_.size(); i++) {
            spriteBatch.DrawString(Fonts::GearInfoFont(), actionList_[(std::size_t)i], position,
                                   i == highlightedAction_ ? selColor_ : nonSelColor_);
            position.Y += height;
        }
    }

    // Receives the spell from the Spellbook screen and casts it.
    // Defined in RolePlayingGame.hpp -- it builds a SpellCombatAction.
    void SpellbookScreenSpellSelected(const std::shared_ptr<Spell>& spell);

    // Receives the item back from the Inventory screen and uses it.
    // Defined in RolePlayingGame.hpp -- it builds an ItemCombatAction.
    void InventoryScreenGearSelected(const std::shared_ptr<Gear>& gear);

    ScreenManager* screenManager_;

    Texture2D backgroundHudTexture_;
    Texture2D topHudTexture_;
    Texture2D combatPopupTexture_;
    Texture2D activeCharInfoTexture_;
    Texture2D inActiveCharInfoTexture_;
    Texture2D cantUseCharInfoTexture_;
    Texture2D selectionBracketTexture_;
    Texture2D menuTexture_;
    Texture2D statsTexture_;
    Texture2D deadPortraitTexture_;
    Texture2D charSelFadeLeftTexture_;
    Texture2D charSelFadeRightTexture_;
    Texture2D charSelArrowLeftTexture_;
    Texture2D charSelArrowRightTexture_;
    Texture2D actionTexture_;
    Texture2D yButtonTexture_;
    Texture2D startButtonTexture_;

    Vector2 topHudPosition_{353.0f, 30.0f};
    Vector2 charSelLeftPosition_{70.0f, 600.0f};
    Vector2 charSelRightPosition_{1170.0f, 600.0f};
    Vector2 yButtonPosition_{0.0f, 560.0f + 20.0f};
    Vector2 startButtonPosition_{0.0f, 560.0f + 35.0f};
    Vector2 yTextPosition_{0.0f, 560.0f + 70.0f};
    Vector2 startTextPosition_{0.0f, 560.0f + 70.0f};
    Vector2 actionTextPosition_{640.0f, 55.0f};
    Vector2 backgroundHudPosition_{0.0f, 525.0f};
    Vector2 portraitPosition_{640.0f, 55.0f};
    Vector2 startingInfoPosition_{0.0f, 550.0f};
    Vector2 namePosition_;
    Vector2 levelPosition_;
    Vector2 detailPosition_;

    const Color activeNameColor_{200, 200, 200};
    const Color inActiveNameColor_{100, 100, 100};
    const Color nonSelColor_{86, 26, 5};
    const Color selColor_{229, 206, 144};

    // The text that is shown in the action bar at the top of the combat screen.
    std::string actionText_;

    // The list of entries in the combat action menu.
    std::array<std::string, 5> actionList_{"Attack", "Spell", "Item", "Defend", "Flee"};

    // The currently highlighted item.
    int highlightedAction_ = 0;
};

} // namespace RolePlaying
