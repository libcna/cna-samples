// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"

namespace Platformer
{
    class TouchCollectionExtensions final
    {
    public:
        TouchCollectionExtensions() = delete;

        [[nodiscard]] static bool AnyTouch(
            const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState);
    };
}
