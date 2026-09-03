// SPDX-License-Identifier: MS-PL

#pragma once

namespace RacingGame::Platform
{
    /** @brief Connects Racing save paths to platform-persistent storage. */
    class PersistentStorage final
    {
    public:
        /** @brief Prepares the platform storage root before game settings load. */
        static void Prepare();
    };
}
