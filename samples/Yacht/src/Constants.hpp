#pragma once

// Constants.hpp -- C++ port of Yacht/Constants.cs.

#include <string>

namespace Yacht {

// The names the game persists itself under.
//
// The original is a `static class` with `internal` visibility, so this is a namespace of
// constants rather than a type.
namespace Constants {

    // The key the live game state is kept under in PhoneApplicationService::State.
    inline const std::string YachtStateKey = "YachtState";

    // The isolated-storage file an offline game is written to.
    inline const std::string YachtStateFileNameOffline = "Offline.sav";

    // The isolated-storage file an online game is written to.
    //
    // Two files rather than one, because the two kinds of game are resumed differently: an
    // offline game is simply reopened, while an online one has to reconnect to the server
    // first, and the game has to know which it is looking at before it reads either.
    inline const std::string YachtStateFileNameOnline = "Online.sav";

} // namespace Constants

} // namespace Yacht
