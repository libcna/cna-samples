#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"

#include "YachtTypes.hpp"
#include "Objects/YachtPlayer.hpp"
#include "Objects/Dice.hpp"
#include "Objects/DiceHandler.hpp"
#include "Objects/HumanPlayer.hpp"
#include "Objects/AIPlayer.hpp"
#include "Objects/NetworkPlayer.hpp"
#include "Misc/AudioManager.hpp"
#include "Accelerometer.hpp"
#include "Misc/NetworkManager.hpp"
#include "ScreenManager/InputState.hpp"
#include "System/Int32.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;

// Bundle of the score-related fonts GameStateHandler needs to draw the
// score card and leader board. Threaded in explicitly by GameplayScreen
// (which owns the actual SpriteFont instances loaded from its own
// ContentManager calls) rather than reached for via a YachtGame-wide
// static, to avoid a header-only circular include between YachtGame.hpp and
// the gameplay object headers it composes (see missing.md).
// Manages the local (offline) turn state machine and draws the score card /
// leader board. Ported from Objects/GameStateHandler.cs (whose own file
// header still says "ScoreCard.cs" -- a leftover from an earlier refactor
// in the original itself). Per the approved plan, every GameTypes.Online-
// gated code path (IsWaitingForPlayer, InitializeOnlinePlayers, SetState,
// UpdateScoreCard, the server-driven half of FinishTurn, ShowGameOver, and
// the "message"/"waiting for other players" HUD text, which was always
// null/false on the local turn state machine and dead in offline play
// anyway) is dropped; only the local turn state machine remains: Roll ->
// hold dice -> SelectScore -> FinishTurn, 12 rounds per player, highest
// total score wins.
class GameStateHandler {
public:
    static const std::array<std::string, 12> ScoreTypesNames;

    /**
     * @brief Creates a handler, either for a fresh offline game or for a state that already
     *        exists -- one restored from storage, or one the server sent.
     *
     * @param diceHandler    The dice every local player rolls.
     * @param input          Where the human player reads gestures from.
     * @param name           The human player's name, for a fresh game.
     * @param state          The state to adopt, or null to start a new offline game.
     * @param screenBounds   The screen the board is laid out against.
     * @param contentManager The content manager to load through.
     */
    GameStateHandler(DiceHandler& diceHandler, InputState& input, const std::string& name,
                     const std::shared_ptr<YachtServices::GameState>& state,
                     Rectangle screenBounds, ContentManager& contentManager)
        : diceHandler_(&diceHandler),
          input_(&input),
          type_(state == nullptr ? YachtServices::GameTypes::Offline : state->GameType),
          screenBounds_(screenBounds),
          contentManager_(&contentManager)
    {
        if (state == nullptr) {
            LoadNewOfflinePlayers(name);
        } else {
            *state_ = *state;

            auto human = std::make_unique<HumanPlayer>(state_->Players[0].Name, diceHandler_, type_,
                                                       input_, screenBounds_);
            human->LoadAssets(*contentManager_);
            human->setGameStateHandlerProperty(this);
            players_.clear();
            players_.push_back(std::move(human));

            if (type_ == YachtServices::GameTypes::Offline) {
                InitializeOfflinePlayers();
            } else {
                isWaitingForPlayer_ = !state_->IsStarted;
                InitializeOnlinePlayers();
            }

            Initialize(false);
        }
    }

    /**
     * @brief Whether the game is still waiting for other people to join.
     *
     * @return True while the table is not full.
     */
    [[nodiscard]] bool IsWaitingForPlayer() const { return isWaitingForPlayer_; }

    /**
     * @brief The state of the game, as this client understands it.
     *
     * @return The state.
     */
    [[nodiscard]] YachtServices::GameState& State() { return *state_; }

    /**
     * @brief The state of the game, as the object the saved game shares.
     *
     * The original's State property hands back the GameState, and a C# object is a reference:
     * the saved game keeps the same one the handler is playing on, so a save written mid-turn
     * has the board that is on screen.
     *
     * @return The state.
     */
    [[nodiscard]] const std::shared_ptr<YachtServices::GameState>& getStatePointerEXT() const
    {
        return state_;
    }

    bool IsInitialized() const { return isInitialized_; }

    bool IsScoreSelect() const { return selectedScore_.has_value(); }
    std::optional<YachtCombination> SelectedScore() const { return selectedScore_; }

    YachtPlayer* CurrentPlayer() const { return players_[state_->CurrentPlayer].get(); }
    YachtPlayer* WinnerPlayer() const { return winnerPlayer_; }
    bool IsGameOver() const { return isGameOver_; }

    void HandleInput(const GestureSample& sample) {
        if (isWaitingForPlayer_ && startWithAI_.has_value()) {
            startWithAI_->HandleInput(sample);
        }

        if (sample.getGestureTypeProperty() == GestureType::VerticalDrag) {
            ScrollBy(sample.getPositionProperty(), sample.getDeltaProperty().Y);
        }
    }

    void Draw(SpriteBatch& spriteBatch) {
        DrawScore(spriteBatch);
        DrawLeaderBoard(spriteBatch);
        DrawMessage(spriteBatch);
    }

    /**
     * @brief Adopts a state the server sent.
     *
     * Only the parts the server owns are taken: how far the game has got, whose turn it is,
     * and what everyone is called. The score cards arrive separately.
     *
     * @param state The server's view of the game.
     */
    void SetState(const YachtServices::GameState& state)
    {
        state_->StepsMade = state.StepsMade;
        state_->IsStarted = state.IsStarted;
        state_->CurrentPlayer = state.CurrentPlayer;

        for (std::size_t i = 0; i < players_.size() && i < state.Players.size(); i++) {
            players_[i]->setNameProperty(state.Players[i].Name);
            state_->Players[i].Name = state.Players[i].Name;
            state_->Players[i].TotalScore = state.Players[i].TotalScore;

            if (i == 0 && dynamic_cast<HumanPlayer*>(players_[i].get()) != nullptr) {
                isWaitingForPlayer_ = !state_->IsStarted;
            }
        }
    }

    /**
     * @brief Takes the score card the server sent for the current player.
     *
     * @param scoreCard The twelve scores.
     */
    void UpdateScoreCard(const std::vector<SharpRuntime::bytecs>& scoreCard)
    {
        players_[static_cast<std::size_t>(state_->CurrentPlayer)]->setGameStateHandlerProperty(this);
        state_->Players[static_cast<std::size_t>(state_->CurrentPlayer)].ScoreCard = scoreCard;
    }

    /**
     * @brief Ends the game on the server's word, with the winner it names.
     *
     * @param endGameState The winning player and their final card.
     */
    void ShowGameOver(const YachtServices::EndGameInformation& endGameState)
    {
        for (std::size_t i = 0; i < state_->Players.size(); i++) {
            if (state_->Players[i].PlayerID == endGameState.PlayerID) {
                state_->CurrentPlayer = static_cast<int>(i);
                state_->Players[i].ScoreCard = endGameState.ScoreCard;
                winnerPlayer_ = players_[i].get();
                isGameOver_ = true;
                return;
            }
        }
    }

    // Write the selected score to the score table and move to the next
    // player. Checks if the game is over.
    void FinishTurn() {
        if (!selectedScore_.has_value())
            return;

        if (type_ != YachtServices::GameTypes::Offline) {
            // The server manages the game state, so simply send the move over.
            const YachtServices::YachtStep currentMove(
                static_cast<int>(selectedScore_.value()) - 1,
                static_cast<SharpRuntime::bytecs>(
                    CombinationScore(selectedScore_.value(), currentDice_.value())),
                state_->CurrentPlayer, state_->StepsMade);

            state_->CurrentPlayer =
                (state_->CurrentPlayer + 1) % static_cast<int>(state_->Players.size());

            if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
                network->GameStep(currentMove);
            }

            selectedScore_.reset();
            message_.clear();
            return;
        }

        PlayerInformation& current = state_->Players[state_->CurrentPlayer];
        int scoreIndex = (int)selectedScore_.value() - 1;
        current.ScoreCard[scoreIndex] = CombinationScore(selectedScore_.value(), currentDice_.value());
        current.TotalScore += current.ScoreCard[scoreIndex];

        state_->CurrentPlayer = (state_->CurrentPlayer + 1) % (int)players_.size();
        state_->StepsMade++;

        if (state_->StepsMade == 12 * (int)players_.size()) {
            state_->CurrentPlayer = HighestPlayerScore();
            winnerPlayer_ = players_[state_->CurrentPlayer].get();
            isGameOver_ = true;

            if (dynamic_cast<HumanPlayer*>(winnerPlayer_) != nullptr)
                AudioManager::PlaySound("Winner");
            else
                AudioManager::PlaySound("Loss");
        } else {
            AudioManager::PlaySoundRandom("TurnChange", 2);
        }

        selectedScore_.reset();
        message_.clear();
    }

    // Sets the dice used to calculate the possible scores.
    void setScoreDice(std::optional<std::array<Dice*, DiceHandler::DiceAmount>> dice) {
        currentDice_ = dice;
    }

    // Select a score line to serve as the user's score for the current turn.
    bool SelectScore(std::optional<YachtCombination> selectedScore) {
        if (selectedScore.has_value() &&
            state_->Players[state_->CurrentPlayer].ScoreCard[(int)selectedScore.value() - 1] == NullScore &&
            currentDice_.has_value()) {
            selectedScore_ = selectedScore;
            AudioManager::PlaySound("ScoreSelect");
            return true;
        } else if (!selectedScore.has_value()) {
            selectedScore_.reset();
            return true;
        }
        return false;
    }

    // Checks whether a specified rectangle intersects a specified score line.
    bool IntersectLine(Rectangle rectangle, int index) const {
        rectangle.Y -= (int)scoreOffset_.Y;
        return scoreLine_[index].Intersects(rectangle);
    }

    // Calculate the score of the supplied dice according to a specified
    // combination. `dice` holds 5 entries, some of which may be nullptr.
    // The trays own their dice; the scoring helpers only read them, so they take raw pointers.
    static std::array<Dice*, DiceHandler::DiceAmount> ToRawDice(
        const std::array<std::shared_ptr<Dice>, DiceHandler::DiceAmount>& dice) {
        std::array<Dice*, DiceHandler::DiceAmount> raw{};
        for (std::size_t i = 0; i < dice.size(); i++) raw[i] = dice[i].get();
        return raw;
    }

    static int CombinationScore(YachtCombination combination,
                                std::array<Dice*, DiceHandler::DiceAmount> dice) {
        // Array.Sort puts nulls first and orders the rest by IComparable, which is Dice::CompareTo.
        std::sort(dice.begin(), dice.end(), [](const Dice* a, const Dice* b) {
            if (a == nullptr) return b != nullptr;
            if (b == nullptr) return false;
            return a->CompareTo(*b) < 0;
        });

        Dice* first = First(dice);
        Dice* last = Last(dice);

        switch (combination) {
            case YachtCombination::Yacht:
                if (first != nullptr && last != nullptr && Times(dice, first->getValueProperty()) == 5)
                    return 50;
                return 0;
            case YachtCombination::LargeStraight:
                if (first != nullptr && last != nullptr &&
                    CheckConsecutiveDice(dice) && last->getValueProperty() == DiceValue::Six)
                    return 30;
                return 0;
            case YachtCombination::SmallStraight:
                if (first != nullptr && last != nullptr &&
                    CheckConsecutiveDice(dice) && last->getValueProperty() == DiceValue::Five)
                    return 30;
                return 0;
            case YachtCombination::FourOfAKind:
                if (first != nullptr && last != nullptr &&
                    (Times(dice, first->getValueProperty()) >= 4 || Times(dice, last->getValueProperty()) >= 4))
                    return Sum(dice, std::nullopt);
                return 0;
            case YachtCombination::FullHouse:
                if (first != nullptr && last != nullptr &&
                    ((Times(dice, first->getValueProperty()) == 3 && Times(dice, last->getValueProperty()) == 2) ||
                     (Times(dice, first->getValueProperty()) == 2 && Times(dice, last->getValueProperty()) == 3)))
                    return Sum(dice, std::nullopt);
                return 0;
            case YachtCombination::Choise:
                return Sum(dice, std::nullopt);
            case YachtCombination::Sixes:
            case YachtCombination::Fives:
            case YachtCombination::Fours:
            case YachtCombination::Threes:
            case YachtCombination::Twos:
            case YachtCombination::Ones:
                return Sum(dice, (DiceValue)(int)combination);
            default:
                return 0;
        }
    }

private:
    // Load and initialize the offline players (the original's
    // LoadNewOfflinePlayers -- there is no "load saved game" path left,
    // since tombstoning/save-load is dropped).
    // A fresh score card: twelve lines, none of them scored yet.
    static std::vector<SharpRuntime::bytecs> NewScoreCard()
    {
        return std::vector<SharpRuntime::bytecs>(
            12, YachtServices::ServiceConstants::NullScore);
    }

    // The three computer opponents that fill an offline table.
    void InitializeOfflinePlayers()
    {
        for (std::size_t i = 1; i < state_->Players.size(); i++) {
            players_.push_back(
                std::make_unique<AIPlayer>(state_->Players[i].Name, diceHandler_));
            players_.back()->setGameStateHandlerProperty(this);
        }
    }

    // The other people at an online table: whoever shares this client's player ID is the
    // human holding the phone, and everyone else is played from the server.
    void InitializeOnlinePlayers()
    {
        // Wait for other players if the game has not started
        isWaitingForPlayer_ = !state_->IsStarted;

        auto* network = NetworkManager::getInstanceProperty();
        const int localPlayerID = network != nullptr ? network->playerID : -1;

        players_.clear();
        for (std::size_t i = 0; i < state_->Players.size(); i++) {
            if (state_->Players[i].PlayerID == localPlayerID) {
                auto human = std::make_unique<HumanPlayer>(state_->Players[i].Name, diceHandler_,
                                                           type_, input_, screenBounds_);
                human->LoadAssets(*contentManager_);
                players_.push_back(std::move(human));

                if (i != 0) {
                    isWaitingForPlayer_ = false;
                }
            } else {
                players_.push_back(
                    std::make_unique<NetworkPlayer>(state_->Players[i].Name, screenBounds_));
            }
            players_.back()->setGameStateHandlerProperty(this);
        }
    }

    void DrawMessage(SpriteBatch& spriteBatch);

    void StartWithAIClick()
    {
        isWaitingForPlayer_ = false;

        // Reset timeout on the server.
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->ResetTimeout();
        }
    }

    // Load and initialize players.
    void LoadNewOfflinePlayers(const std::string& name) {
        auto human = std::make_unique<HumanPlayer>(name, diceHandler_, state_->GameType, input_,
                                                   screenBounds_);
        human->LoadAssets(*contentManager_);

        players_.clear();
        players_.push_back(std::move(human));
        players_.push_back(std::make_unique<AIPlayer>("Josh", diceHandler_));
        players_.push_back(std::make_unique<AIPlayer>("Charles", diceHandler_));
        players_.push_back(std::make_unique<AIPlayer>("Alex", diceHandler_));

        Initialize(true);
    }

    // Lays out the board: the score positions, the leaderboard slots and the hit lines, and
    // -- when the game is new rather than adopted -- a blank score card per player.
    void Initialize(bool initializeScoreTable)
    {
        LoadAssets();

        if (initializeScoreTable) {
            // Initialize the score card
            state_->Players.clear();
            for (auto& player : players_) {
                player->setGameStateHandlerProperty(this);
                YachtServices::PlayerInformation information;
                information.Name = player->getNameProperty();
                information.ScoreCard = NewScoreCard();
                state_->Players.push_back(information);
            }
        }

        // Initialize the position of the score on the card
        for (std::size_t i = 0; i < scorePosition_.size(); i++) {
            scorePosition_[i] = Vector2(20, 50.0f + 42.0f * static_cast<float>(i));
        }

        // Initialize the score line rectangle
        for (std::size_t i = 0; i < scoreLine_.size(); i++) {
            scoreLine_[i] = Rectangle(static_cast<int>(scorePosition_[i].X),
                                      static_cast<int>(scorePosition_[i].Y), 200, 42);
        }

        // Initialize the player leader board position
        for (std::size_t i = 0; i < players_.size() && i < playerPositions_.size(); i++) {
            playerPositions_[i] = Vector2(
                static_cast<float>(screenBounds_.getRightProperty() -
                                   leaderBoardTexture_->getWidthProperty()),
                static_cast<float>(screenBounds_.getTopProperty() + 10 +
                                   (leaderBoardTexture_->getHeightProperty() + 20) *
                                       static_cast<int>(i)));
        }

        isInitialized_ = true;
    }

    void LoadAssets() {
        scoreCardTexture_.emplace(contentManager_->Load<Texture2D>("Images/NameAndTotal"));
        scoreLinesTexture_.emplace(contentManager_->Load<Texture2D>("Images/Score"));
        leaderBoardTexture_.emplace(contentManager_->Load<Texture2D>("Images/leaderboardBg"));
        activeLeaderBoardTexture_.emplace(contentManager_->Load<Texture2D>("Images/leaderboardBg_active"));
        scrollThumbTexture_.emplace(contentManager_->Load<Texture2D>("Images/ScrollThumb"));
        starTexture_.emplace(contentManager_->Load<Texture2D>("Images/Dot"));
        startWithAITexture_.emplace(contentManager_->Load<Texture2D>("Images/startBtn"));

        startWithAI_.emplace(
            &*startWithAITexture_,
            Vector2(static_cast<float>(screenBounds_.Width / 2 -
                                       startWithAITexture_->getWidthProperty() / 2),
                    680.0f),
            nullptr, "");
        startWithAI_->Click += [this](System::Object*, const System::EventArgs&) {
            StartWithAIClick();
        };
    }

    void ScrollBy(Vector2 position, float deltaY) {
        Rectangle touchRect((int)position.X - 5, (int)position.Y - 5, 10, 10);
        Rectangle scrollLineBounds = scoreLinesTexture_->getBoundsProperty();
        scrollLineBounds.Y += 10;

        if (scrollLineBounds.Intersects(touchRect))
            scoreOffset_.Y += deltaY;

        scoreOffset_.Y = MathHelper::Clamp(scoreOffset_.Y,
                                          (float)(scrollLineRectDestination_.Height - scrollLineBounds.Height), 0.0f);
    }

    void DrawScore(SpriteBatch& spriteBatch);

    void DrawLeaderBoard(SpriteBatch& spriteBatch);

    int HighestPlayerScore() const {
        int playerIndex = 0;
        for (size_t i = 0; i < players_.size(); i++) {
            if (AccumulateScore((int)i) > AccumulateScore(playerIndex))
                playerIndex = (int)i;
        }
        return playerIndex;
    }

    int AccumulateScore(int playerIndex) const {
        int total = 0;
        for (int i = 0; i < 12; i++)
            if (state_->Players[playerIndex].ScoreCard[i] != NullScore)
                total += state_->Players[playerIndex].ScoreCard[i];
        return total;
    }

    static Dice* First(const std::array<Dice*, DiceHandler::DiceAmount>& dice) {
        for (auto* d : dice) if (d != nullptr) return d;
        return nullptr;
    }

    static Dice* Last(const std::array<Dice*, DiceHandler::DiceAmount>& dice) {
        for (auto it = dice.rbegin(); it != dice.rend(); ++it) if (*it != nullptr) return *it;
        return nullptr;
    }

    static int Sum(const std::array<Dice*, DiceHandler::DiceAmount>& dice, std::optional<DiceValue> value) {
        int sum = 0;
        for (auto* d : dice)
            if (d != nullptr && (!value.has_value() || d->getValueProperty() == value.value()))
                sum += (int)d->getValueProperty();
        return sum;
    }

    static int Times(const std::array<Dice*, DiceHandler::DiceAmount>& dice, std::optional<DiceValue> value) {
        int count = 0;
        for (auto* d : dice)
            if (d != nullptr && (!value.has_value() || d->getValueProperty() == value.value()))
                count++;
        return count;
    }

    static bool CheckConsecutiveDice(const std::array<Dice*, DiceHandler::DiceAmount>& dice) {
        int count = 0;
        for (size_t i = 0; i + 1 < dice.size(); i++)
            if (dice[i] != nullptr && dice[i + 1] != nullptr &&
                (int)dice[i]->getValueProperty() + 1 == (int)dice[i + 1]->getValueProperty())
                count++;
        return count == (int)dice.size() - 1;
    }

    DiceHandler* diceHandler_;
    InputState* input_;
    YachtServices::GameTypes type_ = YachtServices::GameTypes::Offline;
    bool isWaitingForPlayer_ = false;
    std::string message_;
    std::optional<Button> startWithAI_;
    std::optional<Texture2D> startWithAITexture_;
    Rectangle screenBounds_;
    ContentManager* contentManager_;
    SpriteFont* font_;

    std::vector<std::unique_ptr<YachtPlayer>> players_;
    std::shared_ptr<YachtServices::GameState> state_ =
        std::make_shared<YachtServices::GameState>();
    bool isInitialized_ = false;
    bool isGameOver_ = false;
    YachtPlayer* winnerPlayer_ = nullptr;
    std::optional<YachtCombination> selectedScore_;
    std::optional<std::array<Dice*, DiceHandler::DiceAmount>> currentDice_;

    std::optional<Texture2D> scoreCardTexture_, scoreLinesTexture_, leaderBoardTexture_,
                             activeLeaderBoardTexture_, scrollThumbTexture_, starTexture_;

    Vector2 scoreOffset_ = Vector2::Zero;
    std::array<Vector2, 12> scorePosition_{};
    std::array<Vector2, 4> playerPositions_{};
    Vector2 totalScore_ = Vector2(20, 445);

    std::array<Rectangle, 12> scoreLine_{};
    Rectangle scrollLineRectDestination_ = Rectangle(0, 42, 232, 405);
};

inline const std::array<std::string, 12> GameStateHandler::ScoreTypesNames = {
    "Ones", "Twos", "Threes", "Fours", "Fives", "Sixes", "Choice",
    "Full House", "4 of a kind", "Small 1-5", "Large 2-6", "Yacht"
};

// ---- HumanPlayer methods deferred from Objects/HumanPlayer.hpp (need GameStateHandler) ----
//
// The player draws with the game's fonts and asks the game state handler what a line is worth,
// and the handler owns the players. C# closes that circle for itself.

inline void HumanPlayer::LoadAssets(ContentManager& contentManager)
{
    rollTexture_.emplace(contentManager.template Load<Texture2D>("Images/rollBtn"));
    scoreTexture_.emplace(contentManager.template Load<Texture2D>("Images/scoreBtn"));

    // Initialize the buttons
    Vector2 position(
        static_cast<float>(screenBounds_.getRightProperty() - rollTexture_->getWidthProperty() - 10),
        static_cast<float>(screenBounds_.getCenterProperty().Y -
                           rollTexture_->getBoundsProperty().Height));

    roll_.emplace(&*rollTexture_, position, nullptr, "");

    position.X -= static_cast<float>(scoreTexture_->getWidthProperty() + 20);

    score_.emplace(&*scoreTexture_, position, nullptr, "");

    roll_->Click += [this](System::Object*, const System::EventArgs&) { RollClick(); };
    score_->Click += [this](System::Object*, const System::EventArgs&) { ScoreClick(); };
}

inline void HumanPlayer::Draw(SpriteBatch& spriteBatch)
{
    roll_->Draw(spriteBatch);
    score_->Draw(spriteBatch);
    DrawRollCounter(spriteBatch);
    DrawSelectedScore(spriteBatch);
}



inline void HumanPlayer::PerformPlayerLogic()
{
    // Enable or disable buttons
    roll_->Enabled = diceHandler_->getRollsProperty() != 3 && !diceHandler_->DiceRolling();
    score_->Enabled = gameStateHandler_ != nullptr && gameStateHandler_->IsScoreSelect();

    for (const GestureSample& gesture : input_->Gestures) {
        roll_->HandleInput(gesture);
        score_->HandleInput(gesture);
        HandleDiceHandlerInput(gesture);
        HandleSelectScoreInput(gesture);
    }

    HandleShakeInput();
}

inline void HumanPlayer::HandleShakeInput()
{
    // Register for shake detection
    if (!registeredForShakeDetection_) {
        Accelerometer::ShakeDetected += [this](System::Object*, const System::EventArgs&) {
            shakeDetect_ = true;
        };
        registeredForShakeDetection_ = true;
    }

    if (shakeDetect_) {
        diceHandler_->Roll();

        if (gameType_ == YachtServices::GameTypes::Online) {
            if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
                network->ResetTimeout();
            }
        }

        shakeDetect_ = false;
    }
}

inline void HumanPlayer::HandleSelectScoreInput(const GestureSample& sample)
{
    if (sample.getGestureTypeProperty() != GestureType::Tap) {
        return;
    }

    // Create the touch rectangle
    const Rectangle touchRect(static_cast<int>(sample.getPositionProperty().X) - 5,
                              static_cast<int>(sample.getPositionProperty().Y) - 5, 10, 10);

    for (int i = 0; i < 12; i++) {
        if (gameStateHandler_->IntersectLine(touchRect, i)) {
            gameStateHandler_->SelectScore(static_cast<YachtCombination>(i + 1));
        }
    }
}

inline void HumanPlayer::HandleDiceHandlerInput(const GestureSample& sample)
{
    if (diceHandler_->getRollsProperty() >= 3) {
        return;
    }

    auto* rollingDice = diceHandler_->GetRollingDice();
    auto* holdingDice = diceHandler_->GetHoldingDice();

    if (sample.getGestureTypeProperty() != GestureType::Tap) {
        return;
    }

    // Create the touch rectangle
    const Rectangle touchRect(static_cast<int>(sample.getPositionProperty().X) - 5,
                              static_cast<int>(sample.getPositionProperty().Y) - 5, 10, 10);

    for (int i = 0; i < DiceHandler::DiceAmount; i++) {
        const auto index = static_cast<std::size_t>(i);
        // Check for intersection between the touch rectangle and any of the dice
        const bool rollHit = rollingDice != nullptr && (*rollingDice)[index] != nullptr &&
                             !(*rollingDice)[index]->getIsRollingProperty() &&
                             (*rollingDice)[index]->Intersects(touchRect);
        const bool holdHit = holdingDice != nullptr && (*holdingDice)[index] != nullptr &&
                             (*holdingDice)[index]->Intersects(touchRect);

        if (rollHit || holdHit) {
            diceHandler_->MoveDice(i);

            if (diceHandler_->GetHoldingDice() == nullptr) {
                gameStateHandler_->SelectScore(std::nullopt);
            }
        }
    }
}

inline void HumanPlayer::ScoreClick()
{
    if (gameStateHandler_ != nullptr && gameStateHandler_->IsScoreSelect()) {
        gameStateHandler_->FinishTurn();
        AudioManager::PlaySoundRandom("Pencil", 3);
        diceHandler_->Reset(gameStateHandler_->IsGameOver());
    }
}

inline void HumanPlayer::RollClick()
{
    diceHandler_->Roll();

    if (gameType_ == YachtServices::GameTypes::Online) {
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->ResetTimeout();
        }
    }
}
// ---- AIPlayer method deferred from AIPlayer.hpp (needs GameStateHandler) ----

inline void AIPlayer::PerformPlayerLogic() {
    switch (state_) {
        case AIState::Roll:
            diceHandler_->Roll();
            state_ = AIState::Rolling;
            break;
        case AIState::Rolling:
            if (!diceHandler_->DiceRolling())
                state_ = AIState::ChooseDice;
            break;
        case AIState::ChooseDice:
            diceHandler_->MoveDice(random_.Next(0, 5));
            if (diceHandler_->GetHoldingDice() != nullptr && random_.Next(0, 5) == 1)
                state_ = AIState::SelectScore;
            break;
        case AIState::SelectScore:
            if (gameStateHandler_->SelectScore((YachtCombination)random_.Next(1, 13)))
                state_ = AIState::WriteScore;
            break;
        case AIState::WriteScore:
            if (gameStateHandler_ != nullptr && gameStateHandler_->IsScoreSelect()) {
                gameStateHandler_->FinishTurn();
                diceHandler_->Reset(gameStateHandler_->IsGameOver());
                state_ = AIState::Roll;
            }
            break;
        default:
            break;
    }
}

} // namespace Yacht
