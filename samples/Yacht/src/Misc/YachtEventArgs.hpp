#pragma once

// YachtEventArgs.hpp -- C++ port of Yacht/Misc/YachtEventArgs.cs.
//
// The arguments the network manager raises its events with. They are one file in the original
// because they exist for one reason: the service proxy answers asynchronously, so every reply
// comes back as an event carrying whatever the call asked for.

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/EventArgs.hpp"

#include "../YachtServices/DataModel.hpp"

namespace Yacht {

/** @brief Carries a game state sent by the server. */
class YachtGameStateEventArgs : public System::EventArgs {
public:
    /** @brief The state of the game as the server sees it. */
    std::shared_ptr<YachtServices::GameState> GameState;
};

/** @brief Carries the list of games a player may join. */
class YachtAvailableGamesEventArgs : public System::EventArgs {
public:
    /** @brief The games that are available. */
    std::shared_ptr<YachtServices::AvailableGames> AvailableGames;
};

/** @brief Carries a player's score card. */
class YachtScoreCardEventArgs : public System::EventArgs {
public:
    /** @brief The twelve scores on the card. */
    std::vector<SharpRuntime::bytecs> ScoreCard;
};

/** @brief Carries the result of a finished game. */
class YachtGameOverEventArgs : public System::EventArgs {
public:
    /** @brief The winning player and their final card. */
    std::shared_ptr<YachtServices::EndGameInformation> EndGameState;
};

/** @brief Carries a yes-or-no answer from the server. */
class BooleanEventArgs : public System::EventArgs {
public:
    /** @brief The answer. */
    bool Answer = false;
};

/** @brief Carries a single string value. */
class StringEventArgs : public System::EventArgs {
public:
    /** @brief The value. */
    std::string Value;
};

/** @brief Carries an error that occurred while contacting the server. */
class ExceptionEventArgs : public System::EventArgs {
public:
    /** @brief The error. */
    std::exception_ptr Error;

    /**
     * @brief The error's message, or an empty string when there is no error.
     *
     * The original hands the caller a live Exception object and every use is
     * `e.Error.Message`. A `std::exception_ptr` cannot be read without rethrowing, so the
     * message is offered directly rather than making each call site write the same
     * try/catch.
     *
     * @return The message.
     */
    [[nodiscard]] std::string getMessageProperty() const
    {
        if (!Error) {
            return {};
        }
        try {
            std::rethrow_exception(Error);
        } catch (const std::exception& e) {
            return e.what();
        } catch (...) {
            return "Unknown error";
        }
    }
};

} // namespace Yacht
