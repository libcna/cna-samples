// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "System/TimeSpan.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"

namespace NinjAcademy {

struct GameState {
    int Score = 0;
    int HitPoints = 0;
    int GamePhasesPassed = 0;
    System::TimeSpan ElapsedPhaseTime = System::TimeSpan::Zero;
};

inline constexpr const char* SaveFileName = "State.txt";
inline constexpr const char* GameStateKey = "GameState";

// Microsoft.Phone.Shell.PhoneApplicationService.State has no portable XNA
// counterpart. This one-entry holder preserves its transient hand-off role;
// persistence itself still uses IsolatedStorageFile exactly like the source.
inline std::optional<GameState>& CurrentGameState() {
    static std::optional<GameState> state;
    return state;
}

inline void CleanSavedGameState() {
    auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
    storage.DeleteFile(SaveFileName);
}

} // namespace NinjAcademy
