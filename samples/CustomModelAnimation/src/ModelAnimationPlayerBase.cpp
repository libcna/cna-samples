// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelAnimationPlayerBase.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ModelAnimationPlayerBase.hpp"

#include <vector>

#include "System/ArgumentNullException.hpp"

namespace CustomModelAnimation
{
    void ModelAnimationPlayerBase::setCurrentKeyFrameProperty(int value)
    {
        const std::vector<std::shared_ptr<ModelKeyframe>>& keyframes =
            currentClipValue->getKeyframesProperty();
        const System::TimeSpan time = keyframes[static_cast<std::size_t>(value)]->getTimeProperty();
        setCurrentTimeValueProperty(time);
    }

    void ModelAnimationPlayerBase::setCurrentTimeValueProperty(System::TimeSpan value)
    {
        System::TimeSpan time = value;

        // If the position moved backwards, reset the keyframe index.
        if (time < currentTimeValue)
        {
            currentKeyframe = 0;
            InitClip();
        }

        currentTimeValue = time;

        // Read keyframe matrices.
        const std::vector<std::shared_ptr<ModelKeyframe>>& keyframes =
            currentClipValue->getKeyframesProperty();

        while (static_cast<std::size_t>(currentKeyframe) < keyframes.size())
        {
            const ModelKeyframe& keyframe = *keyframes[static_cast<std::size_t>(currentKeyframe)];

            // Stop when we've read up to the current time position.
            if (keyframe.getTimeProperty() > currentTimeValue)
                break;

            // Use this keyframe
            SetKeyframe(keyframe);

            currentKeyframe++;
        }
    }

    void ModelAnimationPlayerBase::StartClip(const std::shared_ptr<ModelAnimationClip>& clip)
    {
        StartClip(clip, 1.0f, System::TimeSpan::MaxValue);
    }

    void ModelAnimationPlayerBase::StartClip(const std::shared_ptr<ModelAnimationClip>& clip,
                                             float playbackRate, System::TimeSpan duration)
    {
        if (clip == nullptr)
            throw System::ArgumentNullException("Clip required");

        // Store the clip and reset playing data
        currentClipValue = clip;
        currentKeyframe = 0;
        setCurrentTimeValueProperty(System::TimeSpan::Zero);
        elapsedPlaybackTime = System::TimeSpan::Zero;
        paused = false;

        // Store the data about how we want to playback
        this->playbackRate = playbackRate;
        this->duration = duration;

        // Call the virtual to allow initialization of the clip
        InitClip();
    }

    void ModelAnimationPlayerBase::PauseClip()
    {
        paused = true;
    }

    void ModelAnimationPlayerBase::ResumeClip()
    {
        paused = false;
    }

    void ModelAnimationPlayerBase::Update(const GameTime& gameTime)
    {
        if (currentClipValue == nullptr)
            return;

        if (paused)
            return;

        System::TimeSpan time = gameTime.getElapsedGameTimeProperty();

        // Adjust for the rate
        if (playbackRate != 1.0f)
            time = System::TimeSpan::FromMilliseconds(
                time.getTotalMillisecondsProperty() * static_cast<double>(playbackRate));

        elapsedPlaybackTime += time;

        // See if we should terminate
        if ((elapsedPlaybackTime > duration && duration != System::TimeSpan::Zero) ||
            (elapsedPlaybackTime > currentClipValue->getDurationProperty() &&
             duration == System::TimeSpan::Zero))
        {
            Completed.Raise(this, System::EventArgs::Empty);

            currentClipValue = nullptr;

            return;
        }

        // Update the animation position.
        time += currentTimeValue;

        // If we reached the end, loop back to the start.
        while (time >= currentClipValue->getDurationProperty())
            time -= currentClipValue->getDurationProperty();

        setCurrentTimeValueProperty(time);

        OnUpdate();
    }
}
