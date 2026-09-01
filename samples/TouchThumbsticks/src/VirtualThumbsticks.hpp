// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// VirtualThumbsticks.cs
//
// Microsoft Advanced Technology Group
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace TouchThumbsticks
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocation;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    /** @brief Converts simultaneous raw touch contacts into two virtual thumbsticks. */
    class VirtualThumbsticks
    {
    private:
        static constexpr float maxThumbstickDistance = 60.0f;

        inline static Vector2 leftPosition;
        inline static Vector2 rightPosition;
        inline static int leftId = -1;
        inline static int rightId = -1;
        inline static std::optional<Vector2> leftThumbstickCenter;
        inline static std::optional<Vector2> rightThumbstickCenter;

    public:
        /** @brief Static utility type; instances cannot be created. */
        VirtualThumbsticks() = delete;

        /**
         * @brief Gets the center position of the left thumbstick.
         *
         * @return Center position while a left touch is tracked; otherwise no value.
         */
        [[nodiscard]] static std::optional<Vector2> getLeftThumbstickCenterProperty()
        {
            return leftThumbstickCenter;
        }

        /**
         * @brief Gets the center position of the right thumbstick.
         *
         * @return Center position while a right touch is tracked; otherwise no value.
         */
        [[nodiscard]] static std::optional<Vector2> getRightThumbstickCenterProperty()
        {
            return rightThumbstickCenter;
        }

        /**
         * @brief Gets the current normalized left-thumbstick value.
         *
         * @return Thumbstick displacement clamped to unit length, or zero without a touch.
         */
        [[nodiscard]] static Vector2 getLeftThumbstickProperty()
        {
            if (!leftThumbstickCenter.has_value())
                return Vector2::Zero;

            Vector2 result =
                (leftPosition - *leftThumbstickCenter) / maxThumbstickDistance;
            if (result.LengthSquared() > 1.0f)
                result.Normalize();
            return result;
        }

        /**
         * @brief Gets the current normalized right-thumbstick value.
         *
         * @return Thumbstick displacement clamped to unit length, or zero without a touch.
         */
        [[nodiscard]] static Vector2 getRightThumbstickProperty()
        {
            if (!rightThumbstickCenter.has_value())
                return Vector2::Zero;

            Vector2 result =
                (rightPosition - *rightThumbstickCenter) / maxThumbstickDistance;
            if (result.LengthSquared() > 1.0f)
                result.Normalize();
            return result;
        }

        /** @brief Updates both virtual thumbsticks from the current raw touch snapshot. */
        static void Update()
        {
            std::optional<TouchLocation> leftTouch;
            std::optional<TouchLocation> rightTouch;
            const TouchCollection touches = TouchPanel::GetState();

            for (const TouchLocation& touch : touches)
            {
                if (touch.getIdProperty() == leftId)
                {
                    leftTouch = touch;
                    continue;
                }

                if (touch.getIdProperty() == rightId)
                {
                    rightTouch = touch;
                    continue;
                }

                TouchLocation earliestTouch;
                if (!touch.TryGetPreviousLocation(earliestTouch))
                    earliestTouch = touch;

                if (leftId == -1 &&
                    earliestTouch.getPositionProperty().X <
                        TouchPanel::getDisplayWidthProperty() / 2)
                {
                    leftTouch = earliestTouch;
                    continue;
                }

                if (rightId == -1 &&
                    earliestTouch.getPositionProperty().X >=
                        TouchPanel::getDisplayWidthProperty() / 2)
                {
                    rightTouch = earliestTouch;
                    continue;
                }
            }

            if (leftTouch.has_value())
            {
                if (!leftThumbstickCenter.has_value())
                    leftThumbstickCenter = leftTouch->getPositionProperty();
                leftPosition = leftTouch->getPositionProperty();
                leftId = leftTouch->getIdProperty();
            }
            else
            {
                leftThumbstickCenter.reset();
                leftId = -1;
            }

            if (rightTouch.has_value())
            {
                if (!rightThumbstickCenter.has_value())
                    rightThumbstickCenter = rightTouch->getPositionProperty();
                rightPosition = rightTouch->getPositionProperty();
                rightId = rightTouch->getIdProperty();
            }
            else
            {
                rightThumbstickCenter.reset();
                rightId = -1;
            }
        }
    };
}
