// SPDX-License-Identifier: MS-PL

#include "Platform/ContentDelivery.hpp"

#include <stdexcept>
#include <string>
#include <utility>

#if defined(__EMSCRIPTEN__) && defined(RACING_WEB_PROGRESSIVE_CONTENT)
#include <emscripten.h>

EM_JS(int, BeginRacingContentPackage, (const char* groupPointer), {
    const group = UTF8ToString(groupPointer);
    if (!Module.racingContentPackages)
        Module.racingContentPackages = Object.create(null);
    const state = Module.racingContentPackages[group];
    if (state === "failed") return -1;
    if (state) return 0;

    Module.racingContentPackages[group] = "loading";
    const script = document.createElement("script");
    script.async = true;
    script.src = `RacingGame-content-${group}.js`;
    script.onload = () => {
        Module.racingContentPackages[group] = "loader-ready";
    };
    script.onerror = () => {
        Module.racingContentPackages[group] = "failed";
    };
    document.head.appendChild(script);
    return 0;
});

EM_JS(void, CompleteRacingContentPackage, (const char* groupPointer), {
    const group = UTF8ToString(groupPointer);
    Module.racingContentPackages[group] = "ready";
    if (group === "textures" && Module.setStatus)
        Module.setStatus("");
});
#endif

namespace RacingGame::Platform
{
    ContentDelivery::ContentDelivery(std::filesystem::path setContentRoot)
        : contentRoot(std::move(setContentRoot))
    {
    }

    bool ContentDelivery::EnsureReady(const ContentGroup group) const
    {
#if defined(__EMSCRIPTEN__) && defined(RACING_WEB_PROGRESSIVE_CONTENT)
        const char* name = GetName(group);
        if (std::filesystem::is_regular_file(
                contentRoot / (std::string(".racing-") + name + "-ready")))
        {
            CompleteRacingContentPackage(name);
            return true;
        }
        if (BeginRacingContentPackage(name) < 0)
            throw std::runtime_error(
                std::string("Unable to load Racing Web content group: ") + name);
        return false;
#else
        (void)group;
        return true;
#endif
    }

    const char* ContentDelivery::GetName(const ContentGroup group)
    {
        switch (group)
        {
        case ContentGroup::Models: return "models";
        case ContentGroup::Landscape: return "landscape";
        case ContentGroup::Textures: return "textures";
        }
        throw std::invalid_argument("Unknown Racing content group");
    }
}
