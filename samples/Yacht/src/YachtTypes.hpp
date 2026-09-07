#pragma once

// YachtTypes.hpp -- the types the game shares with the service, and the score-card enum.
//
// Migration note: PlayerInformation and GameState used to be declared here as plain-data
// subsets with serialization and the online fields removed. They are now the real
// YachtServices types (src/YachtServices/DataModel.hpp), which both products share and which
// the offline game is built on too. This header re-exports them into the game's namespace
// while the remaining units move to the original's own file layout.

#include "YachtServices/DataModel.hpp"
#include "YachtServices/ServiceConstants.hpp"

namespace Yacht {

using YachtServices::AvailableGames;
using YachtServices::EndGameInformation;
using YachtServices::GameInformation;
using YachtServices::GameState;
using YachtServices::GameTypes;
using YachtServices::Message;
using YachtServices::MessageContentType;
using YachtServices::PlayerInformation;
using YachtServices::SimpleType;
using YachtServices::YachtStep;

// The score-card value meaning "this line has not been scored yet".
inline constexpr SharpRuntime::bytecs NullScore = YachtServices::ServiceConstants::NullScore;

// Possible score types on the score card (Objects/GameStateHandler.cs's YachtCombination).
enum class YachtCombination {
    Ones = 1,
    Twos = 2,
    Threes = 3,
    Fours = 4,
    Fives = 5,
    Sixes = 6,
    Choise = 7,  // [sic] matches the original's own spelling
    FullHouse = 8,
    FourOfAKind = 9,
    SmallStraight = 10,
    LargeStraight = 11,
    Yacht = 12,
};

} // namespace Yacht
