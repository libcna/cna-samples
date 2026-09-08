#pragma once

// HumanPlayer.hpp -- C++ port of Yacht/Objects/HumanPlayer.cs.

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

#include "../Misc/Button.hpp"
#include "../ScreenManager/InputState.hpp"
#include "../YachtServices/DataModel.hpp"
#include "DiceHandler.hpp"
#include "YachtPlayer.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Content::ContentManager;

/**
 * @brief The person holding the phone.
 *
 * Two buttons and the dice: roll, keep what looks useful, roll again up to three times, then
 * pick a line to score. Shaking the phone rolls as well, which is why this player is the only
 * one that listens to the accelerometer.
 */
class HumanPlayer : public YachtPlayer {
public:
    /**
     * @brief Creates a human player.
     *
     * @param name         The player's name.
     * @param diceHandler  The dice this player rolls.
     * @param gameType     Whether the game is offline or on the server.
     * @param input        Where this player reads gestures from.
     * @param screenBounds The screen the buttons are laid out against.
     */
    HumanPlayer(std::string name, DiceHandler* diceHandler, YachtServices::GameTypes gameType,
                InputState* input, const Rectangle& screenBounds)
        : YachtPlayer(std::move(name), diceHandler),
          input_(input),
          gameType_(gameType),
          screenBounds_(screenBounds)
    {
    }

    /**
     * @brief Loads the roll and score buttons and places them.
     *
     * @param contentManager The content manager to load through.
     */
    void LoadAssets(ContentManager& contentManager);

    /**
     * @brief Draws the buttons, the rolls left, and the score the held dice would earn.
     *
     * @param spriteBatch The batch to draw into; must already be begun.
     */
    void Draw(SpriteBatch& spriteBatch) override;

    /** @brief Reads this frame's gestures and the accelerometer, and acts on them. */
    void PerformPlayerLogic() override;

private:
    void DrawSelectedScore(SpriteBatch& spriteBatch);
    void DrawRollCounter(SpriteBatch& spriteBatch);
    void HandleShakeInput();
    void HandleSelectScoreInput(const GestureSample& sample);
    void HandleDiceHandlerInput(const GestureSample& sample);
    void ScoreClick();
    void RollClick();

    InputState* input_ = nullptr;
    std::optional<Button> roll_;
    std::optional<Button> score_;
    std::optional<Texture2D> rollTexture_;
    std::optional<Texture2D> scoreTexture_;
    Rectangle screenBounds_;
    YachtServices::GameTypes gameType_ = YachtServices::GameTypes::Offline;
    bool registeredForShakeDetection_ = false;
    bool shakeDetect_ = false;
};

} // namespace Yacht
