// SPDX-License-Identifier: MS-PL

#pragma once

#include <filesystem>

namespace RacingGame::Platform
{
    /** @brief Identifies one deferred group in the browser content package. */
    enum class ContentGroup
    {
        Models,
        Landscape,
        Textures,
    };

    /** @brief Makes staged Web content visible before its normal load phase. */
    class ContentDelivery final
    {
    public:
        /**
         * @brief Creates a delivery gate for one deployed Content root.
         *
         * @param contentRoot Runtime Content directory used by the game.
         */
        explicit ContentDelivery(std::filesystem::path contentRoot);

        /**
         * @brief Starts a deferred package and reports when its files are ready.
         *
         * Native builds are already deployed and return true immediately.
         *
         * @param group Deferred load phase requested by the existing loading screen.
         * @return True when that phase may construct its normal XNA resources.
         */
        [[nodiscard]] bool EnsureReady(ContentGroup group) const;

    private:
        std::filesystem::path contentRoot;

        [[nodiscard]] static const char* GetName(ContentGroup group);
    };
}
