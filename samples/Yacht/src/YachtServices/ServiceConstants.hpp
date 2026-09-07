#pragma once

// ServiceConstants.hpp -- C++ port of YachtServices/ServiceConstants.cs.

#include <string>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace YachtServices {

// Constants shared by the game client and the game server.
//
// The original is a `static class` with `internal` visibility, so this is a namespace of
// constants rather than a type: nothing outside the two products ever names it.
namespace ServiceConstants {

    // The score-card value meaning "this line has not been scored yet".
    //
    // A score of zero is a legal, common result -- a player who rolls no sixes scores zero on
    // Sixes -- so the empty marker has to be a value no combination can produce, and 255 is the
    // one the original picked.
    inline constexpr SharpRuntime::bytecs NullScore = 255;

    inline const std::string LeftMessageString = "Left Game";
    inline const std::string JoinMessageString = "Join Game";
    inline const std::string GameStateMessageString = "Game State";
    inline const std::string NewGameMessageString = "New Game";
    inline const std::string AvailableGamesMessageString = "Available Games";
    inline const std::string BannedMessageString = "Player Banned";
    inline const std::string GameOverMessageString = "Game Over";
    inline const std::string AIMessageString = "AI";

} // namespace ServiceConstants

} // namespace YachtServices
