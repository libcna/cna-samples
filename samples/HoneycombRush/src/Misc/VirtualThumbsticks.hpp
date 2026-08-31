#pragma once

// VirtualThumbsticks.hpp — C++ port of Misc/VirtualThumbsticks.cs (XNA 4.0
// HoneycombRush sample). Touching the left half of the screen places the
// center of the left thumbstick, the right half the right thumbstick;
// dragging away from that center simulates thumbstick input.

#include <optional>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

#include "../ScreenManager/InputState.hpp"

namespace HoneycombRush {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchLocation;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

class VirtualThumbsticks {
public:
    VirtualThumbsticks() = delete;

    static std::optional<Vector2> getLeftThumbstickCenter() { return leftThumbstickCenter_; }
    static std::optional<Vector2> getRightThumbstickCenter() { return rightThumbstickCenter_; }

    static Vector2 getLeftThumbstick() {
        if (!leftThumbstickCenter_.has_value()) return Vector2::Zero;

        Vector2 l = (leftPosition_ - *leftThumbstickCenter_) / MaxThumbstickDistance;
        if (l.LengthSquared() > 1.0f) l.Normalize();
        return l;
    }

    static Vector2 getRightThumbstick() {
        if (!rightThumbstickCenter_.has_value()) return Vector2::Zero;

        Vector2 r = (rightPosition_ - *rightThumbstickCenter_) / MaxThumbstickDistance;
        if (r.LengthSquared() > 1.0f) r.Normalize();
        return r;
    }

    // Updates the virtual thumbsticks based on current touch state. Must be
    // called every frame.
    static void Update(InputState& input) {
        std::optional<TouchLocation> leftTouch, rightTouch;
        const TouchCollection& touches = input.TouchState;

        for (const TouchLocation& touch : touches) {
            if (touch.getIdProperty() == leftId_) {
                leftTouch = touch;
                continue;
            }

            if (touch.getIdProperty() == rightId_) {
                rightTouch = touch;
                continue;
            }

            TouchLocation earliestTouch = touch;
            TouchLocation previous;
            if (touch.TryGetPreviousLocation(previous)) {
                earliestTouch = previous;
            }

            if (leftId_ == -1) {
                if (earliestTouch.getPositionProperty().X < (float)TouchPanel::getDisplayWidthProperty() / 2.0f) {
                    leftTouch = earliestTouch;
                    continue;
                }
            }

            if (rightId_ == -1) {
                if (earliestTouch.getPositionProperty().X >= (float)TouchPanel::getDisplayWidthProperty() / 2.0f) {
                    rightTouch = earliestTouch;
                    continue;
                }
            }
        }

        if (leftTouch.has_value()) {
            if (!leftThumbstickCenter_.has_value()) {
                leftThumbstickCenter_ = leftTouch->getPositionProperty();
            }
            leftPosition_ = leftTouch->getPositionProperty();
            leftId_ = leftTouch->getIdProperty();
        } else {
            leftThumbstickCenter_.reset();
            leftId_ = -1;
        }

        if (rightTouch.has_value()) {
            if (!rightThumbstickCenter_.has_value()) {
                rightThumbstickCenter_ = rightTouch->getPositionProperty();
            }
            rightPosition_ = rightTouch->getPositionProperty();
            rightId_ = rightTouch->getIdProperty();
        } else {
            rightThumbstickCenter_.reset();
            rightId_ = -1;
        }
    }

private:
    static constexpr float MaxThumbstickDistance = 60.0f;

    static inline Vector2 leftPosition_;
    static inline Vector2 rightPosition_;

    static inline int leftId_ = -1;
    static inline int rightId_ = -1;

    static inline std::optional<Vector2> leftThumbstickCenter_;
    static inline std::optional<Vector2> rightThumbstickCenter_;

};

} // namespace HoneycombRush
