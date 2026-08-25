// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Tank.hpp"

#include "../Behaviors/ChaseBehavior.hpp"
#include "../Behaviors/WanderBehavior.hpp"

#include "System/Math.hpp"

namespace FuzzyLogic
{
    using Microsoft::Xna::Framework::MathHelper;

    const TimeSpan Tank::MinTime = TimeSpan::Zero;
    const TimeSpan Tank::MaxTime = TimeSpan::FromSeconds(4.0f);

    Tank::Tank(Rectangle levelBoundary, List<std::shared_ptr<Mouse>>& mice)
        : Entity(levelBoundary), mice(mice)
    {
        setPositionProperty(Vector2((float)(levelBoundary.Width / 2),
                                    (float)(levelBoundary.Height / 2)));
    }

    void Tank::ChooseBehavior(const GameTime& gameTime)
    {
        // Use fuzzy logic to choose which mouse to chase.
        std::shared_ptr<Mouse> nextMouse = ChooseMouse();

        // If ChooseMouse returned null, that means that there werent any suitable
        // mice in range. we should wander around.
        if (nextMouse == nullptr)
        {
            // if we're wandering, the tank shouldn't be highlighted.
            setIsHighlightedProperty(false);

            if (currentlyChasingMouse != nullptr)
            {
                currentlyChasingMouse->setIsHighlightedProperty(false);
                currentlyChasingMouse = nullptr;
            }

            if (dynamic_cast<WanderBehavior*>(getCurrentBehaviorProperty()) == nullptr)
            {
                setCurrentBehaviorProperty(std::make_unique<WanderBehavior>(this));
            }
        }
        // Ok, nextMouse isn't null. Is it a new mouse?
        else if (nextMouse != currentlyChasingMouse)
        {
            // We're going to start chasing after someone, highlight the tank and
            // reset the chasing timer.
            setIsHighlightedProperty(true);
            timeChasingThisMouse = TimeSpan::Zero;

            // Unhighlight the old prey...
            if (currentlyChasingMouse != nullptr)
            {
                currentlyChasingMouse->setIsHighlightedProperty(false);
            }

            // ...change to the new one ....
            currentlyChasingMouse = nextMouse;

            // .... and highlight him.
            nextMouse->setIsHighlightedProperty(true);

            // Finally, set our new behavior to chase after our new prey.
            setCurrentBehaviorProperty(
                std::make_unique<ChaseBehavior>(this, currentlyChasingMouse.get()));
        }
        // If we hit this, we're still chasing the same mouse. All we have to do is
        // update the "timeChasingThisMouse" timer.
        else
        {
            timeChasingThisMouse += gameTime.getElapsedGameTimeProperty();
        }
    }

    std::shared_ptr<Mouse> Tank::ChooseMouse()
    {
        // In order to decide which mouse to chase, we'll loop over all of them and
        // see which is the best choice.
        std::shared_ptr<Mouse> bestMouse = nullptr;
        float bestFuzzyValue = 0.0f;

        for (int i = 0; i < mice.getCountProperty(); i++)
        {
            // Calculate the distance to the mouse. if it's greater than the max
            // distance, the tank can't "see" this mouse and we should move on to
            // the next.
            // List<T>'s indexer hands back a mutation-tracking proxy whose operator->
            // stops at the element, so the shared_ptr is bound before it is followed.
            const std::shared_ptr<Mouse>& mouse = mice[i];
            float distance =
                Vector2::Distance(getPositionProperty(), mouse->getPositionProperty());
            if (distance > MaxDistance)
            {
                continue;
            }

            // This variable will store the fuzzy value for this mouse: in other
            // words, "how good of a choice" this mouse is. the tank will prefer
            // mice with a fuzzy value that is close to 1.0. The fuzzy value is
            // based on three factors: distance, angle, and time.
            float fuzzy = 0.0f;

            // First, we'll use the distance to the mouse that we computed earlier
            // to add in the fuzzy distance value. This is a value that ranges from
            // 0 to 1, and increases as the tank gets closer to the mouse.
            fuzzy += CalculateFuzzyDistance(distance) * getFuzzyDistanceWeightProperty();


            // Next, we'll calculate the angle to the mouse, and use that to get
            // a value that starts at 0 and increases towards 1 as the angle to the
            // mouse diminishes. This will make the tank prefer mice that are
            // already in front of him.
            fuzzy += CalculateFuzzyAngle(i) * getFuzzyAngleWeightProperty();


            // The final value that we'll include is time. We want the tank to
            // prefer the mouse he is already changing. At a minimum, this can be
            // used to prevent hysterisis, but it can also be used to make the tank
            // behave more tenaciously; focusing on one mouse even if a "better"
            // choice appears.
            fuzzy += CalculateFuzzyTime(i) * getFuzzyTimeWeightProperty();


            // Now that we know how good of a choice this mouse is, is it better
            // than the best one we've found so far?
            if (fuzzy > bestFuzzyValue)
            {
                bestMouse = mouse;
                bestFuzzyValue = fuzzy;
            }
        }
        return bestMouse;
    }

    float Tank::CalculateFuzzyDistance(float distance) const
    {
        return (1 - ((distance - MinDistance) / (MaxDistance - MinDistance)));
    }

    float Tank::CalculateFuzzyAngle(int i) const
    {
        // to calculate this value, first we need to find the angle to the
        // mouse...
        const std::shared_ptr<Mouse>& mouse = mice[i];
        Vector2 toMouse = mouse->getPositionProperty() - getPositionProperty();
        float angleToMouse = (float)System::Math::Atan2(toMouse.Y, toMouse.X);

        // and then find the difference between that angle and the tank's
        // current orientation.
        float angleDifference = System::Math::Abs(
            Behavior::WrapAngle(getOrientationProperty() - angleToMouse));

        // Calculate fuzzyAngle, which should range from 0 to 1 based on
        // angleDifference. We'll need to clamp it explicitly to that range.
        float fuzzyAngle =
            (1 - ((angleDifference - MinAngle) / (MaxAngle - MinAngle)));
        return MathHelper::Clamp(fuzzyAngle, 0.0f, 1.0f);
    }

    float Tank::CalculateFuzzyTime(int i) const
    {
        // To calcuate the fuzzy time value, first we figure out how long we've
        // been chasing this mouse. For most mice, this will of course be no
        // time at all.
        TimeSpan time = TimeSpan::Zero;
        if (mice[i] == currentlyChasingMouse)
        {
            time = timeChasingThisMouse;
        }

        // Next we calculate fuzzyTime, which is a value ranging from 0 to 1. It
        // will increase as the amount of time we have spent chasing this mouse
        // increases. The value must be clamped to enforce the 0 to 1 rule.
        float fuzzyTime = (float)((time - MinTime).getTotalSecondsProperty()
            / (MaxTime - MinTime).getTotalSecondsProperty());
        return MathHelper::Clamp(fuzzyTime, 0.0f, 1.0f);
    }
}
