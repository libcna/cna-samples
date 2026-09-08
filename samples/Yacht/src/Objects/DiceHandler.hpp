#pragma once

// DiceHandler.hpp -- C++ port of Yacht/Objects/DiceHandler.cs.

#include <memory>
#include <stdexcept>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Xml/Serialization/IXmlSerializable.hpp"

#include "../Misc/AudioManager.hpp"
#include "DiceState.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Graphics::GraphicsDevice;

// The game class, for the fonts it owns. Draw() is defined out of line in YachtGame.hpp: the
// game header reaches this one through the screens, so the include cannot go both ways -- the
// same cross-reference C# resolves for itself and C++ does not.
class YachtGame;

/**
 * @brief Owns the five dice: rolls them, moves them to and from the holding tray, draws them.
 *
 * The handler holds no dice of its own -- they live in the DiceState it is given, which is what
 * lets a game be restored mid-turn: hand the handler the state that was read back and the same
 * dice are in the same trays with the same rolls spent.
 */
class DiceHandler : public System::Xml::Serialization::IXmlSerializable {
public:
    /** @brief How many dice are in play. */
    static constexpr int DiceAmount = 5;

    /**
     * @brief The dice this handler is working on.
     *
     * @return The state.
     */
    [[nodiscard]] Yacht::DiceState& getDiceStateProperty() { return *diceState_; }

    /**
     * @brief The dice this handler is working on.
     *
     * @return The state.
     */
    [[nodiscard]] const Yacht::DiceState& getDiceStateProperty() const { return *diceState_; }

    /**
     * @brief The dice this handler is working on, as the object itself.
     *
     * The original's property hands back the DiceState, and a C# object is a reference -- the
     * saved game keeps the same one the handler is using, so a save written mid-turn has the
     * dice that are on screen. This is that reference.
     *
     * @return The state.
     */
    [[nodiscard]] const std::shared_ptr<Yacht::DiceState>& getDiceStatePointerEXT() const
    {
        return diceState_;
    }

    /**
     * @brief How many times the dice have been rolled this turn.
     *
     * @return The roll count, at most three.
     */
    [[nodiscard]] int getRollsProperty() const { return diceState_->Rolls; }

    /**
     * @brief Creates a handler.
     *
     * @param graphicsDevice The device whose viewport bounds the dice are laid out against.
     * @param state          The dice to work on, or null to start a fresh set.
     */
    DiceHandler(GraphicsDevice& graphicsDevice, const std::shared_ptr<Yacht::DiceState>& state)
    {
        screenBounds_ = graphicsDevice.getViewportProperty().getBoundsProperty();

        if (state == nullptr) {
            diceState_ = std::make_shared<Yacht::DiceState>();
            Reset(false);
        } else {
            diceState_ = state;
        }
    }

    /**
     * @brief Loads the tray and roll-border art and lays out their positions.
     *
     * @param contentManager The content manager to load through.
     */
    void LoadAssets(ContentManager& contentManager)
    {
        diceRollBorder_.emplace(contentManager.Load<Texture2D>("Images/diceRollBorder"));
        holdingTray_.emplace(contentManager.Load<Texture2D>("Images/holdingTray"));

        // Initialize positions
        holdingTrayPosition_ =
            Vector2(static_cast<float>(screenBounds_.getLeftProperty()),
                    static_cast<float>(screenBounds_.getBottomProperty() -
                                       holdingTray_->getBoundsProperty().Height));
        holdTextPosition_ = holdingTrayPosition_ + Vector2(20, 30);
        rollBorderPosition_ =
            holdingTrayPosition_ -
            Vector2(0, static_cast<float>(diceRollBorder_->getHeightProperty() + 10));
    }

    /** @brief Advances every rolling die and moves settled dice to the tray after three rolls. */
    void Update()
    {
        // Update all rolling dice
        for (std::size_t i = 0; i < diceState_->RollingDice.size(); i++) {
            if (diceState_->RollingDice[i] != nullptr) {
                diceState_->RollingDice[i]->Update();
            }
        }

        // Place all dice on the holding tray after 3 rolls
        if (getRollsProperty() == 3 && !DiceRolling()) {
            for (int i = 0; i < DiceAmount; i++) {
                if (diceState_->RollingDice[static_cast<std::size_t>(i)] != nullptr) {
                    MoveDice(i);
                }
            }
        }

        if (DiceRolling()) {
            AudioManager::PlaySoundRandom("Roll", 4);
        }
    }

    /**
     * @brief Draws the tray, the roll border and the dice.
     *
     * @param spriteBatch The batch to draw into; must already be begun.
     */
    void Draw(SpriteBatch& spriteBatch) const;

    /**
     * @brief The dice on the holding tray.
     *
     * @return The holding tray, or null when nothing is held.
     */
    [[nodiscard]] std::array<std::shared_ptr<Dice>, DiceState::DieAmount>* GetHoldingDice()
    {
        for (std::size_t i = 0; i < diceState_->HoldingDice.size(); i++) {
            if (diceState_->HoldingDice[i] != nullptr) {
                return &diceState_->HoldingDice;
            }
        }
        return nullptr;
    }

    /**
     * @brief The dice still in the roll border.
     *
     * @return The rolling dice, or null when none are there.
     */
    [[nodiscard]] std::array<std::shared_ptr<Dice>, DiceState::DieAmount>* GetRollingDice()
    {
        for (std::size_t i = 0; i < diceState_->RollingDice.size(); i++) {
            if (diceState_->RollingDice[i] != nullptr) {
                return &diceState_->RollingDice;
            }
        }
        return nullptr;
    }

    /**
     * @brief Starts a fresh set of dice.
     *
     * @param gameOver True to clear the dice away instead of laying out a new set.
     */
    void Reset(bool gameOver) { Reset(gameOver, true); }

    /**
     * @brief Starts a fresh set of dice.
     *
     * @param gameOver   True to clear the dice away instead of laying out a new set.
     * @param resetRolls True to put the roll count back to zero.
     */
    void Reset(bool gameOver, bool resetRolls)
    {
        if (resetRolls) {
            diceState_->Rolls = 0;
        }

        for (int i = 0; i < DiceAmount; i++) {
            const auto index = static_cast<std::size_t>(i);
            // Initialize new dice and arrange them inside the rolling border
            if (gameOver) {
                diceState_->RollingDice[index] = nullptr;
            } else {
                diceState_->RollingDice[index] = std::make_shared<Dice>();
                diceState_->RollingDice[index]->setPositionProperty(
                    Vector2(rollBorderPosition_.X + 89 * static_cast<float>(i) + 30,
                            rollBorderPosition_.Y + 20));
            }
            diceState_->HoldingDice[index] = nullptr;
        }
    }

    /** @brief Puts every die back where its tray says it belongs. */
    void PositionDice()
    {
        for (int i = 0; i < DiceAmount; i++) {
            const auto index = static_cast<std::size_t>(i);
            if (diceState_->RollingDice[index] != nullptr) {
                diceState_->RollingDice[index]->setPositionProperty(
                    Vector2(rollBorderPosition_.X + 89 * static_cast<float>(i) + 30,
                            rollBorderPosition_.Y + 20));
            } else if (diceState_->HoldingDice[index] != nullptr) {
                diceState_->HoldingDice[index]->setPositionProperty(
                    Vector2(rollBorderPosition_.X + 89 * static_cast<float>(i) + 30,
                            rollBorderPosition_.Y + 30 +
                                static_cast<float>(holdingTray_->getHeightProperty())));
            }
        }
    }

    /** @brief Rolls every die still in the roll border, if a roll is left. */
    void Roll()
    {
        if (getRollsProperty() < 3 && !DiceRolling()) {
            for (int i = 0; i < DiceAmount; i++) {
                if (diceState_->RollingDice[static_cast<std::size_t>(i)] != nullptr) {
                    diceState_->RollingDice[static_cast<std::size_t>(i)]->Roll();
                }
            }
            diceState_->Rolls++;
        }
    }

    /**
     * @brief Moves one die between the roll border and the holding tray.
     *
     * @param diceIndex Which of the five to move.
     */
    void MoveDice(int diceIndex)
    {
        // Check if the dice can be moved
        if (getRollsProperty() > 0 && diceIndex >= 0 && diceIndex < 5) {
            const auto index = static_cast<std::size_t>(diceIndex);
            const auto trayHeight = static_cast<float>(holdingTray_->getHeightProperty());

            // Check if the dice is not in the hold tray and not currently rolling
            if (diceState_->RollingDice[index] != nullptr &&
                !diceState_->RollingDice[index]->getIsRollingProperty()) {
                diceState_->RollingDice[index]->setPositionProperty(
                    diceState_->RollingDice[index]->getPositionProperty() +
                    Vector2(0, trayHeight + 10));
                diceState_->HoldingDice[index] = diceState_->RollingDice[index];
                diceState_->RollingDice[index] = nullptr;
                AudioManager::PlaySoundRandom("DieSelect", 2);
            } else if (diceState_->HoldingDice[index] != nullptr) {
                diceState_->HoldingDice[index]->setPositionProperty(
                    diceState_->HoldingDice[index]->getPositionProperty() -
                    Vector2(0, trayHeight + 10));
                diceState_->RollingDice[index] = diceState_->HoldingDice[index];
                diceState_->HoldingDice[index] = nullptr;
                AudioManager::PlaySoundRandom("DieSelect", 2);
            }
        }
    }

    /**
     * @brief Whether any die is still tumbling.
     *
     * @return True while at least one die is rolling.
     */
    [[nodiscard]] bool DiceRolling() const
    {
        bool isRolling = false;
        for (std::size_t i = 0; i < diceState_->RollingDice.size(); i++) {
            if (diceState_->RollingDice[i] != nullptr) {
                isRolling |= diceState_->RollingDice[i]->getIsRollingProperty();
            }
        }
        return isRolling;
    }

    /**
     * @brief Takes over a restored set of dice.
     *
     * @param state The dice to work on from now on.
     */
    void LoadState(const std::shared_ptr<Yacht::DiceState>& state)
    {
        diceState_ = state;
        PositionDice();
    }

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("DiceHandler::GetSchema is not implemented.");
    }

    /**
     * @brief Reads the dice state.
     *
     * @param reader The xml reader from which to read.
     */
    void ReadXml(System::Xml::XmlReader& reader) override { diceState_->ReadXml(reader); }

    /**
     * @brief Writes the dice state.
     *
     * @param writer The xml writer to use when writing.
     */
    void WriteXml(System::Xml::XmlWriter& writer) const override { diceState_->WriteXml(writer); }

private:
    void DrawDice(SpriteBatch& spriteBatch) const
    {
        for (int i = 0; i < DiceAmount; i++) {
            const auto index = static_cast<std::size_t>(i);
            if (diceState_->RollingDice[index] != nullptr) {
                diceState_->RollingDice[index]->Draw(spriteBatch);
            } else if (diceState_->HoldingDice[index] != nullptr) {
                diceState_->HoldingDice[index]->Draw(spriteBatch);
            }
        }
    }

    std::shared_ptr<Yacht::DiceState> diceState_;

    std::optional<Texture2D> diceRollBorder_;
    std::optional<Texture2D> holdingTray_;
    Vector2 holdingTrayPosition_;
    Vector2 holdTextPosition_;
    Vector2 rollBorderPosition_;
    Rectangle screenBounds_;
};

} // namespace Yacht
