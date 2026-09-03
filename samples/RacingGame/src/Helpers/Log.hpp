// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

namespace RacingGame::Helpers
{
    /** @brief Writes the original desktop diagnostic log in isolated storage. */
    class Log final
    {
    public:
        /** @brief Opens or rotates Log.txt and writes a session header. */
        static void Initialize();

        /**
         * @brief Writes one timestamped message when the log is available.
         * @param message Diagnostic message to append.
         */
        static void Write(const std::string& message);

    private:
        Log() = delete;
    };
}
