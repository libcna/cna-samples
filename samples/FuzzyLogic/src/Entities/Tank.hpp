// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "Entity.hpp"
#include "Mouse.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/TimeSpan.hpp"
#include "System/Collections/Generic/List.hpp"

namespace FuzzyLogic
{
    using System::TimeSpan;
    using System::Collections::Generic::List;

    /**
     * @brief The Tank class, unsurprisingly enough, represents the tank, which chases
     *        after the mice.
     *
     * This class contains most of the interesting logic in this sample, including the
     * fuzzy decision making process.
     */
    class Tank : public Entity
    {
    public:
        /**
         * @brief How fast can the tank move?
         * @return The maximum speed.
         */
        [[nodiscard]] float getMaxSpeedProperty() const override { return 2.0f; }

        /**
         * @brief How fast can he turn?
         * @return The turn speed, in radians per update.
         */
        [[nodiscard]] float getTurnSpeedProperty() const override { return .075f; }

        /**
         * @brief What texture should Entity use for the tank?
         * @return The asset name of the tank's texture.
         */
        [[nodiscard]] String getTextureFileProperty() const override { return "Tank"; }

        /** @brief At what distance will the tank have "caught" the mouse? */
        static constexpr float CaughtDistance = 30.0f;

        /**
         * @brief Gets the weight that makes the tank prefer to chase after mice which
         *        are nearby.
         *
         * If this value is 0, the distance will not be a factor.
         *
         * @return The distance weight, from 0 to 1.
         */
        [[nodiscard]] float getFuzzyDistanceWeightProperty() const
        {
            return fuzzyDistanceWeight;
        }

        /**
         * @brief Sets the weight that makes the tank prefer to chase after mice which
         *        are nearby.
         * @param value The new weight; clamped to the range 0 to 1.
         */
        void setFuzzyDistanceWeightProperty(float value)
        {
            fuzzyDistanceWeight =
                Microsoft::Xna::Framework::MathHelper::Clamp(value, 0.0f, 1.0f);
        }

        /**
         * @brief Gets the weight that makes the tank prefer to chase after the mice
         *        that it is already chasing after.
         *
         * If this value is 1, the tank never "change its mind" about which mouse to
         * chase: once it picks a target it will stay with it until it either catches
         * it, or it gets away. If this value is 0, the tank will appear to be very
         * indecisive, and will constantly change targets.
         *
         * @return The time weight, from 0 to 1.
         */
        [[nodiscard]] float getFuzzyTimeWeightProperty() const { return fuzzyTimeWeight; }

        /**
         * @brief Sets the weight that makes the tank prefer to chase after the mice
         *        that it is already chasing after.
         * @param value The new weight; clamped to the range 0 to 1.
         */
        void setFuzzyTimeWeightProperty(float value)
        {
            fuzzyTimeWeight = Microsoft::Xna::Framework::MathHelper::Clamp(value, 0.0f, 1.0f);
        }

        /**
         * @brief Gets the weight that makes the tank prefer to chase after the mice
         *        that are directly in front of it.
         *
         * If this value is 1, the tank will select its target based only on how much
         * it will have to turn to catch it. If this value is 0, the angle to the
         * target will not be a factor.
         *
         * @return The angle weight, from 0 to 1.
         */
        [[nodiscard]] float getFuzzyAngleWeightProperty() const
        {
            return fuzzyAngleWeight;
        }

        /**
         * @brief Sets the weight that makes the tank prefer to chase after the mice
         *        that are directly in front of it.
         * @param value The new weight; clamped to the range 0 to 1.
         */
        void setFuzzyAngleWeightProperty(float value)
        {
            fuzzyAngleWeight = Microsoft::Xna::Framework::MathHelper::Clamp(value, 0.0f, 1.0f);
        }

        /**
         * @brief Constructs the tank in the middle of the level.
         * @param levelBoundary The rectangle the tank is clamped to.
         * @param mice The list of mice the tank may chase.
         */
        Tank(Rectangle levelBoundary, List<std::shared_ptr<Mouse>>& mice);

    protected:
        /**
         * @brief ChooseBehavior is overriden from entity and will use fuzzy logic to
         *        determine how the tank should act.
         *
         * Which mouse should it chase? or should it wander around?
         *
         * @param gameTime Provides a snapshot of timing values.
         */
        void ChooseBehavior(const GameTime& gameTime) override;

    private:
        /**
         * ChooseMouse contains the logic at the heart of this sample: the fuzzy logic
         * calcuations that will make the tank choose its next target mouse.
         */
        std::shared_ptr<Mouse> ChooseMouse();

        /** Calculates a value from 0 to 1 that represents how "close" a mouse is. */
        float CalculateFuzzyDistance(float distance) const;

        /**
         * Returns a value from 0 to 1 that is based on the tank's orientation, and the
         * angle to the mouse.
         */
        float CalculateFuzzyAngle(int i) const;

        /**
         * Returns a value from 0 to 1 that represents how long the tank has been
         * chasing a mouse.
         */
        float CalculateFuzzyTime(int i) const;

        // As discussed in the accompanying doc, these next three pairs of constants
        // are the minimum and maximum values for the fuzzy logic calculations.

        static constexpr float MinDistance = 0.0f;
        static constexpr float MaxDistance = 175.0f;

        static constexpr float MinAngle = 0.0f;
        static constexpr float MaxAngle = Microsoft::Xna::Framework::MathHelper::PiOver2;

        static const TimeSpan MinTime;
        static const TimeSpan MaxTime;

        float fuzzyDistanceWeight = .5f;
        float fuzzyTimeWeight = .5f;
        float fuzzyAngleWeight = .5f;

        // We'll need to keep track of the list of mice that we can chase,...
        List<std::shared_ptr<Mouse>>& mice;

        // ... who we're currently chasing after...
        std::shared_ptr<Mouse> currentlyChasingMouse;

        // ... and how long we've been chasing him.
        TimeSpan timeChasingThisMouse;
    };
}
