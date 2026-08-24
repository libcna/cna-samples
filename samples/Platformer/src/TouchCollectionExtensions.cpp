// SPDX-License-Identifier: MS-PL

#include "TouchCollectionExtensions.hpp"

#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"

namespace Platformer
{
    bool TouchCollectionExtensions::AnyTouch(
        const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState)
    {
        using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
        for (const auto& location : touchState)
        {
            if (location.getStateProperty() == TouchLocationState::Pressed ||
                location.getStateProperty() == TouchLocationState::Moved)
                return true;
        }
        return false;
    }
}
