// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelAnimationPlayerBase.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "ModelAnimationClip.hpp"
#include "ModelKeyframe.hpp"

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace CustomModelAnimation
{
    using Microsoft::Xna::Framework::GameTime;

    /**
     * @brief Base class for the animation players.
     *
     * It contains the common functionality to deal with a clip, playing it back at a speed,
     * notifying clients of completion, etc.
     */
    class ModelAnimationPlayerBase : public System::Object
    {
    public:
        /** @brief Destroys the player. */
        virtual ~ModelAnimationPlayerBase() = default;

        /**
         * @brief Gets the clip currently being decoded.
         * @return The current clip, or null when nothing is playing.
         */
        [[nodiscard]] const std::shared_ptr<ModelAnimationClip>& getCurrentClipProperty() const
        {
            return currentClipValue;
        }

        /**
         * @brief Gets the current key frame index.
         * @return The index of the next keyframe to be applied.
         */
        [[nodiscard]] int getCurrentKeyFrameProperty() const { return currentKeyframe; }

        /**
         * @brief Sets the current key frame index, which moves the play position to its time.
         * @param value The keyframe index to move to.
         */
        void setCurrentKeyFrameProperty(int value);

        /**
         * @brief Gets the current play position.
         * @return The play position.
         */
        [[nodiscard]] System::TimeSpan getCurrentTimeValueProperty() const
        {
            return currentTimeValue;
        }

        /**
         * @brief Sets the current play position, applying every keyframe up to it.
         * @param value The new play position.
         */
        void setCurrentTimeValueProperty(System::TimeSpan value);

        /** @brief Invoked when playback has completed. */
        System::EventHandler<System::EventArgs> Completed;

        /**
         * @brief Starts decoding the specified animation clip, looping forever at normal speed.
         * @param clip The animation clip to play.
         */
        void StartClip(const std::shared_ptr<ModelAnimationClip>& clip);

        /**
         * @brief Starts playing a clip.
         *
         * @param clip Animation clip to play.
         * @param playbackRate Speed to playback.
         * @param duration Length of time to play (max is looping, 0 is once).
         */
        void StartClip(const std::shared_ptr<ModelAnimationClip>& clip, float playbackRate,
                       System::TimeSpan duration);

        /** @brief Will pause the playback of the current clip. */
        void PauseClip();

        /** @brief Will resume playback of the current clip. */
        void ResumeClip();

        /**
         * @brief Called during the update loop to move the animation forward.
         * @param gameTime Provides a snapshot of timing values.
         */
        virtual void Update(const GameTime& gameTime);

    protected:
        /**
         * @brief Allows subclasses to do any initialization of data when the clip is initialized.
         */
        virtual void InitClip() {}

        /**
         * @brief Allows subclasses to set any data associated with a particular keyframe.
         * @param keyframe Keyframe being set.
         */
        virtual void SetKeyframe(const ModelKeyframe& keyframe) { (void)keyframe; }

        /**
         * @brief Allows subclasses to perform work needed after the animation has been updated
         *        for a new time index.
         */
        virtual void OnUpdate() {}

    private:
        // Clip currently being played
        std::shared_ptr<ModelAnimationClip> currentClipValue;

        // Current timeindex and keyframe in the clip
        System::TimeSpan currentTimeValue;
        int currentKeyframe = 0;

        // Speed of playback
        float playbackRate = 1.0f;

        // The amount of time for which the animation will play.
        // TimeSpan::MaxValue will loop forever. TimeSpan::Zero will play once.
        System::TimeSpan duration = System::TimeSpan::MaxValue;

        // Amount of time elapsed while playing
        System::TimeSpan elapsedPlaybackTime = System::TimeSpan::Zero;

        // Whether or not playback is paused
        bool paused = false;
    };
}
