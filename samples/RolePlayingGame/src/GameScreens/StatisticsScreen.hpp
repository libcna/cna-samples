#pragma once

// StatisticsScreen.hpp -- C++ port of GameScreens/StatisticsScreen.cs.

#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Int32.hpp"

#include "../Data/AnimatingSprite.hpp"
#include "../Data/Animation.hpp"
#include "../Data/Characters/Player.hpp"
#include "../Data/Gear/Armor.hpp"
#include "../Data/Int32Range.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/Session.hpp"
#include "EquipmentScreen.hpp"
#include "InventoryScreen.hpp"
#include "PlayerNpcScreen.hpp"
#include "QuestLogScreen.hpp"
#include "SpellbookScreen.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::AnimatingSprite;
using RolePlayingGameData::Animation;
using RolePlayingGameData::Armor;
using RolePlayingGameData::Int32Range;
using RolePlayingGameData::Player;
using RolePlayingGameData::Weapon;

// Draws the statistics for a particular Player.
class StatisticsScreen : public GameScreen {
public:
    // Creates a new StatisticsScreen object for the given player.
    explicit StatisticsScreen(const std::shared_ptr<Player>& player) : player_(player) {
        SetIsPopup(true);

        ResetScreenAnimation();
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        statisticsScreen_ = content.Load<Texture2D>("Textures/GameScreens/GameScreenBkgd");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");
        scoreBoardTexture_ = content.Load<Texture2D>("Textures/GameScreens/CountShieldWithArrow");
        leftTriggerButton_ = content.Load<Texture2D>("Textures/Buttons/LeftTriggerButton");
        rightTriggerButton_ = content.Load<Texture2D>("Textures/Buttons/RightTriggerButton");
        backButton_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        selectButton_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        dropButton_ = content.Load<Texture2D>("Textures/Buttons/YButton");
        statisticsBorder_ = content.Load<Texture2D>("Textures/GameScreens/StatsBorderTable");
        borderLine_ = content.Load<Texture2D>("Textures/GameScreens/LineBorder");
        goldIcon_ = content.Load<Texture2D>("Textures/GameScreens/GoldIcon");
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        screenRectangle_ = Rectangle(viewport.getXProperty(), viewport.getYProperty(),
                                     viewport.getWidthProperty(), viewport.getHeightProperty());

        statisticsNamePosition_.X = ((float)viewport.getWidthProperty() -
                                     Fonts::HeaderFont().MeasureString("Statistics").X) / 2.0f;
        statisticsNamePosition_.Y = 90.0f;

        plankPosition_.X =
            (float)((viewport.getWidthProperty() - plankTexture_.getWidthProperty()) / 2);
        plankPosition_.Y = 67.0f;
    }

    void HandleInput() override {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }
        // shows the spells for this player
        else if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            GetScreenManager()->AddScreen(
                std::make_shared<SpellbookScreen>(player_, player_->CharacterStatistics()));
            return;
        }
        // show the equipment for this player, allowing the user to unequip
        else if (InputManager::IsActionTriggered(InputManager::Action::TakeView)) {
            GetScreenManager()->AddScreen(std::make_shared<EquipmentScreen>(player_));
            return;
        } else if (IsPlayerInParty()) { // player is in the party
            // move to the previous screen
            if (InputManager::IsActionTriggered(InputManager::Action::PageLeft)) {
                ExitScreen();
                GetScreenManager()->AddScreen(std::make_shared<QuestLogScreen>(nullptr));
                return;
            }
            // move to the next screen
            else if (InputManager::IsActionTriggered(InputManager::Action::PageRight)) {
                ExitScreen();
                GetScreenManager()->AddScreen(std::make_shared<InventoryScreen>(true));
                return;
            }
            // move to the previous party member
            else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
                playerIndex_--;
                if (playerIndex_ < 0) {
                    playerIndex_ = (int)Session::GetParty()->Players.size() - 1;
                }
                const std::shared_ptr<Player>& newPlayer =
                    Session::GetParty()->Players[(std::size_t)playerIndex_];
                if (newPlayer != player_) {
                    player_ = newPlayer;
                    ResetScreenAnimation();
                }
            }
            // move to the next party member
            else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
                playerIndex_++;
                if (playerIndex_ >= (int)Session::GetParty()->Players.size()) {
                    playerIndex_ = 0;
                }
                const std::shared_ptr<Player>& newPlayer =
                    Session::GetParty()->Players[(std::size_t)playerIndex_];
                if (newPlayer != player_) {
                    player_ = newPlayer;
                    ResetScreenAnimation();
                }
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        screenAnimation_->UpdateAnimation(
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        GetScreenManager()->getSpriteBatch().Begin();
        DrawStatistics();
        GetScreenManager()->getSpriteBatch().End();
    }

private:
    bool IsPlayerInParty() const {
        for (const auto& p : Session::GetParty()->Players) {
            if (p == player_) return true;
        }
        return false;
    }

    // Build the idle animation the screen shows for the current player.
    void ResetScreenAnimation() {
        screenAnimation_ = std::make_shared<AnimatingSprite>();
        screenAnimation_->SetFrameDimensions(player_->MapSprite->FrameDimensions());
        screenAnimation_->FramesPerRow = player_->MapSprite->FramesPerRow;
        screenAnimation_->SourceOffset = player_->MapSprite->SourceOffset;
        screenAnimation_->Texture = player_->MapSprite->Texture;
        screenAnimation_->AddAnimation(std::make_shared<Animation>("Idle", 43, 48, 150, true));
        screenAnimation_->PlayAnimation(0);
    }

    // Draws the player statistics.
    void DrawStatistics() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // Draw faded screen
        spriteBatch.Draw(fadeTexture_, screenRectangle_, Color::White);

        // Draw the Statistics Screen
        spriteBatch.Draw(statisticsScreen_, screenRectangle_, Color::White);
        spriteBatch.Draw(plankTexture_, plankPosition_, Color::White);
        spriteBatch.Draw(statisticsBorder_, statisticsBorderPosition_, Color::White);

        spriteBatch.Draw(borderLine_, borderLinePosition_, Color::White);

        spriteBatch.Draw(*screenAnimation_->Texture, idlePortraitPosition_,
                         screenAnimation_->SourceRectangle(), Color::White, 0.0f,
                         Vector2(screenAnimation_->SourceOffset.X, screenAnimation_->SourceOffset.Y),
                         1.0f, SpriteEffects::None, 0.0f);

        spriteBatch.DrawString(Fonts::HeaderFont(), "Statistics", statisticsNamePosition_,
                               Fonts::TitleColor);
        DrawPlayerDetails();
        DrawButtons();
    }

    void DrawButtons() {
        if (!IsActive()) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position;

        if (IsPlayerInParty()) {
            // Left Trigger
            position = leftTriggerPosition_;
            spriteBatch.Draw(leftTriggerButton_, position, Color::White);

            // Draw Left Trigger Information
            position.Y += (float)leftTriggerButton_.getHeightProperty();
            placeTextMid_ = Fonts::PlayerStatisticsFont().MeasureString("Quest");
            position.X += (float)(leftTriggerButton_.getWidthProperty() / 2) - placeTextMid_.X / 2.0f;
            spriteBatch.DrawString(Fonts::PlayerStatisticsFont(), "Quest", position, Color::Black);

            // Right Trigger
            position = rightTriggerPosition_;
            spriteBatch.Draw(rightTriggerButton_, position, Color::White);

            // Draw Right Trigger Information
            position.Y += (float)rightTriggerButton_.getHeightProperty();
            placeTextMid_ = Fonts::PlayerStatisticsFont().MeasureString("Items");
            position.X += (float)(leftTriggerButton_.getWidthProperty() / 2) - placeTextMid_.X / 2.0f;
            spriteBatch.DrawString(Fonts::PlayerStatisticsFont(), "Items", position, Color::Black);
        }

        // Back Button
        spriteBatch.Draw(backButton_, backButtonPosition_, Color::White);

        spriteBatch.Draw(selectButton_, selectButtonPosition_, Color::White);
        position = selectButtonPosition_;
        position.X -= Fonts::ButtonNamesFont().MeasureString("Spellbook").X + 10.0f;
        position.Y += 5.0f;
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Spellbook", position, Color::White);

        // Draw Back
        position = backButtonPosition_;
        position.X += (float)backButton_.getWidthProperty() + 10.0f;
        position.Y += 5.0f;
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back", position, Color::White);

        // Draw drop Button
        spriteBatch.Draw(dropButton_, dropButtonPosition_, Color::White);
        position = dropButtonPosition_;
        position.X -= Fonts::ButtonNamesFont().MeasureString("Equipment").X + 10.0f;
        position.Y += 5.0f;
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Equipment", position, Color::White);

        // Draw Gold Icon
        spriteBatch.Draw(goldIcon_, goldIconPosition_, Color::White);
    }

    // Draws player information.
    void DrawPlayerDetails() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        if (player_ != nullptr) {
            currentTextPosition_.X = playerTextPosition_.X;
            currentTextPosition_.Y = playerTextPosition_.Y;

            // Current Level
            spriteBatch.DrawString(
                Fonts::DescriptionFont(),
                "Level: " + System::Int32::ToString(player_->CharacterLevel()),
                currentTextPosition_, Color::Black);

            // Health Points
            currentTextPosition_.Y += (float)IntervalBetweenEachInfo;
            spriteBatch.DrawString(
                Fonts::DescriptionFont(),
                "Health Points: " +
                    System::Int32::ToString(player_->CurrentStatistics().HealthPoints) + "/" +
                    System::Int32::ToString(player_->CharacterStatistics().HealthPoints),
                currentTextPosition_, Color::Black);

            // Magic Points
            currentTextPosition_.Y += (float)IntervalBetweenEachInfo;
            spriteBatch.DrawString(
                Fonts::DescriptionFont(),
                "Magic Points: " +
                    System::Int32::ToString(player_->CurrentStatistics().MagicPoints) + "/" +
                    System::Int32::ToString(player_->CharacterStatistics().MagicPoints),
                currentTextPosition_, Color::Black);

            // Experience Details
            currentTextPosition_.Y += (float)IntervalBetweenEachInfo;
            if (player_->IsMaximumCharacterLevel()) {
                spriteBatch.DrawString(Fonts::DescriptionFont(), "Experience: Maximum",
                                       currentTextPosition_, Color::Black);
            } else {
                spriteBatch.DrawString(
                    Fonts::DescriptionFont(),
                    "Experience: " + System::Int32::ToString(player_->Experience()) + "/" +
                        System::Int32::ToString(player_->ExperienceForNextLevel()),
                    currentTextPosition_, Color::Black);
            }

            DrawEquipmentInfo(*player_);

            DrawModifiers(*player_);

            DrawEquipmentStatistics(*player_);
        }
        // Draw Gold
        spriteBatch.DrawString(Fonts::ButtonNamesFont(),
                               Fonts::GetGoldString(Session::GetParty()->PartyGold()),
                               goldPosition_, Color::White);
    }

    // Draw Equipment Info of the player selected.
    void DrawEquipmentInfo(Player& selectedPlayer) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        // Character name
        currentTextPosition_ = characterNamePosition_;

        currentTextPosition_.X -= Fonts::HeaderFont().MeasureString(selectedPlayer.Name()).X / 2.0f;
        spriteBatch.DrawString(Fonts::HeaderFont(), selectedPlayer.Name(), currentTextPosition_,
                               Fonts::TitleColor);

        // Class name
        currentTextPosition_ = classNamePosition_;
        currentTextPosition_.X -=
            Fonts::GearInfoFont().MeasureString(selectedPlayer.GetCharacterClass()->Name).X / 2.0f;
        spriteBatch.DrawString(Fonts::GearInfoFont(), selectedPlayer.GetCharacterClass()->Name,
                               currentTextPosition_, Color::Black);
    }

    // Draw Base Amount Plus any Modifiers.
    void DrawModifiers(Player& selectedPlayer) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        currentTextPosition_.X = playerTextPosition_.X;
        currentTextPosition_.Y = playerTextPosition_.Y + 150.0f;

        // PO + Modifiers
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "Physical Offense: " +
                System::Int32::ToString(selectedPlayer.CurrentStatistics().PhysicalOffense),
            currentTextPosition_, Color::Black);

        // PD + Modifiers
        currentTextPosition_.Y += (float)IntervalBetweenEachInfo;
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "Physical Defense: " +
                System::Int32::ToString(selectedPlayer.CurrentStatistics().PhysicalDefense),
            currentTextPosition_, Color::Black);

        // MO + Modifiers
        currentTextPosition_.Y += (float)IntervalBetweenEachInfo;
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "Magical Offense: " +
                System::Int32::ToString(selectedPlayer.CurrentStatistics().MagicalOffense),
            currentTextPosition_, Color::Black);

        // MD + Modifiers
        currentTextPosition_.Y += (float)IntervalBetweenEachInfo;
        spriteBatch.DrawString(
            Fonts::DescriptionFont(),
            "Magical Defense: " +
                System::Int32::ToString(selectedPlayer.CurrentStatistics().MagicalDefense),
            currentTextPosition_, Color::Black);
    }

    // Draw the equipment statistics.
    void DrawEquipmentStatistics(Player& selectedPlayer) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        Vector2 position = weaponPosition_;
        Int32Range healthDamageRange;
        healthDamageRange.Minimum = healthDamageRange.Maximum =
            selectedPlayer.CurrentStatistics().PhysicalOffense;
        std::shared_ptr<Weapon> weapon = selectedPlayer.GetEquippedWeapon();
        if (weapon != nullptr) {
            weapon->DrawIcon(spriteBatch, position);
            healthDamageRange += weapon->TargetDamageRange;
        }

        position = armorPosition_;
        Int32Range healthDefenseRange;
        healthDefenseRange.Minimum = healthDefenseRange.Maximum =
            selectedPlayer.CurrentStatistics().PhysicalDefense;
        Int32Range magicDefenseRange;
        magicDefenseRange.Minimum = magicDefenseRange.Maximum =
            selectedPlayer.CurrentStatistics().MagicalDefense;
        for (int i = 0; i < 4; i++) {
            std::shared_ptr<Armor> armor =
                selectedPlayer.GetEquippedArmor((Armor::ArmorSlot)i);
            if (armor != nullptr) {
                armor->DrawIcon(spriteBatch, position);
                healthDefenseRange += armor->OwnerHealthDefenseRange;
                magicDefenseRange += armor->OwnerMagicDefenseRange;
            }
            position.X += 68.0f;
        }

        position = weaponTextPosition_;
        spriteBatch.DrawString(Fonts::DescriptionFont(),
                               "Weapon Attack: (" +
                                   System::Int32::ToString(healthDamageRange.Minimum) + "," +
                                   System::Int32::ToString(healthDamageRange.Maximum) + ")",
                               position, Color::Black);

        position.Y += 130.0f;
        spriteBatch.DrawString(Fonts::DescriptionFont(),
                               "Weapon Defense: (" +
                                   System::Int32::ToString(healthDefenseRange.Minimum) + "," +
                                   System::Int32::ToString(healthDefenseRange.Maximum) + ")",
                               position, Color::Black);

        position.Y += 30.0f;
        spriteBatch.DrawString(Fonts::DescriptionFont(),
                               "Spell Defense: (" +
                                   System::Int32::ToString(magicDefenseRange.Minimum) + "," +
                                   System::Int32::ToString(magicDefenseRange.Maximum) + ")",
                               position, Color::Black);
    }

    // This is used as the Player NPC to display statistics of the player.
    std::shared_ptr<Player> player_;

    Texture2D statisticsScreen_;
    Texture2D selectButton_;
    Texture2D backButton_;
    Texture2D dropButton_;
    Texture2D statisticsBorder_;
    Texture2D plankTexture_;
    Texture2D fadeTexture_;
    Texture2D goldIcon_;
    Texture2D scoreBoardTexture_;
    Texture2D rightTriggerButton_;
    Texture2D leftTriggerButton_;
    Texture2D borderLine_;

    Rectangle screenRectangle_;
    static constexpr int IntervalBetweenEachInfo = 30;
    int playerIndex_ = 0;
    std::shared_ptr<AnimatingSprite> screenAnimation_;

    const Vector2 playerTextPosition_{515.0f, 200.0f};
    Vector2 currentTextPosition_;
    const Vector2 scoreBoardPosition_{1160.0f, 354.0f};
    Vector2 placeTextMid_;
    Vector2 statisticsNamePosition_;
    const Vector2 shieldPosition_{1124.0f, 253.0f};
    const Vector2 idlePortraitPosition_{300.0f, 380.0f};
    const Vector2 dropButtonPosition_{900.0f, 640.0f};
    const Vector2 statisticsBorderPosition_{180.0f, 147.0f};
    const Vector2 borderLinePosition_{184.0f, 523.0f};
    const Vector2 characterNamePosition_{330.0f, 180.0f};
    const Vector2 classNamePosition_{330.0f, 465.0f};
    Vector2 plankPosition_;
    const Vector2 goldIconPosition_{490.0f, 640.0f};
    const Vector2 weaponPosition_{790.0f, 220.0f};
    const Vector2 armorPosition_{790.0f, 346.0f};
    const Vector2 weaponTextPosition_{790.0f, 285.0f};
    const Vector2 leftTriggerPosition_{340.0f, 50.0f};
    const Vector2 rightTriggerPosition_{900.0f, 50.0f};
    const Vector2 iconPosition_{100.0f, 200.0f};
    const Vector2 selectButtonPosition_{1150.0f, 640.0f};
    const Vector2 backButtonPosition_{80.0f, 640.0f};
    const Vector2 goldPosition_{565.0f, 648.0f};
};

// ---- screens that page into StatisticsScreen (defined here) ----

inline void PlayerNpcScreen::HandleInput() {
    // view the player's statistics
    if (InputManager::IsActionTriggered(InputManager::Action::TakeView)) {
        GetScreenManager()->AddScreen(
            std::make_shared<StatisticsScreen>(std::dynamic_pointer_cast<Player>(character_)));
        return;
    }

    if (isIntroduction_) {
        // accept the invitation
        if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            isIntroduction_ = false;
            std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(character_);
            Session::GetParty()->JoinParty(player);
            Session::RemovePlayerNpc(mapEntry_);
            SetDialogueText(player->JoinAcceptedDialogue);
            SetBackText("Back");
            SetSelectText("Back");
        }
        // reject the invitation
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            isIntroduction_ = false;
            std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(character_);
            SetDialogueText(player->JoinRejectedDialogue);
            SetBackText("Back");
            SetSelectText("Back");
        }
    } else {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Ok) ||
            InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }
    }
}

inline void InventoryScreen::PageScreenLeft() {
    if (CombatEngine::IsActive()) {
        return;
    }

    if (isItems_) {
        ExitScreen();
        GetScreenManager()->AddScreen(
            std::make_shared<StatisticsScreen>(Session::GetParty()->Players[0]));
    } else {
        isItems_ = !isItems_;
        ResetTriggerText();
    }
}

inline void InventoryScreen::PageScreenRight() {
    if (CombatEngine::IsActive()) {
        return;
    }

    if (isItems_) {
        isItems_ = !isItems_;
        ResetTriggerText();
    } else {
        ExitScreen();
        GetScreenManager()->AddScreen(std::make_shared<QuestLogScreen>(nullptr));
    }
}

inline void QuestLogScreen::PageScreenLeft() {
    ExitScreen();
    GetScreenManager()->AddScreen(std::make_shared<InventoryScreen>(false));
}

inline void QuestLogScreen::PageScreenRight() {
    ExitScreen();
    GetScreenManager()->AddScreen(
        std::make_shared<StatisticsScreen>(Session::GetParty()->Players[0]));
}

} // namespace RolePlaying
