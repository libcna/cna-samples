// SPDX-License-Identifier: MS-PL

#include "Platform/PersistentStorage.hpp"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

#include <stdexcept>

#include "System/Environment.hpp"

EM_JS(int, IsRacingPersistentStorageReady, (), {
    return Module.racingStorage === "ready" ? 1 : 0;
});
#endif

namespace RacingGame::Platform
{
    void PersistentStorage::Prepare()
    {
#if defined(__EMSCRIPTEN__)
        if (!IsRacingPersistentStorageReady())
            throw std::runtime_error(
                "Racing browser storage did not finish its initial synchronization");
        System::Environment::SetEnvironmentVariable(
            "XDG_DATA_HOME", "/save");
#endif
    }
}
