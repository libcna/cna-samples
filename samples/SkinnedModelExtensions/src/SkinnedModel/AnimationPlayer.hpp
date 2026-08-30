// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "AnimationClip.hpp"
#include "SkinningData.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace SkinnedModel
{
    using Microsoft::Xna::Framework::Matrix;

    /** @brief Decodes bone-position matrices from an animation clip. */
    class AnimationPlayer : public System::Object
    {
    public:
        /**
         * @brief Constructs an animation player.
         * @param skinningData Bind pose, hierarchy and clips to decode.
         */
        explicit AnimationPlayer(const SkinningData& skinningData);

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `SkinnedModel.AnimationPlayer`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Starts decoding a clip from its beginning.
         * @param clip Clip to decode.
         */
        void StartClip(const AnimationClip& clip);

        /**
         * @brief Advances animation and refreshes all transform arrays.
         * @param time Relative delta or absolute animation time.
         * @param relativeToCurrentTime Whether @p time is relative to the current position.
         * @param rootTransform Transform applied above the skeleton root.
         */
        void Update(System::TimeSpan time, bool relativeToCurrentTime,
                    const Matrix& rootTransform);

        /**
         * @brief Refreshes local bone transforms from the current clip.
         * @param time Relative delta or absolute animation time.
         * @param relativeToCurrentTime Whether @p time is relative to the current position.
         */
        void UpdateBoneTransforms(System::TimeSpan time, bool relativeToCurrentTime);

        /**
         * @brief Refreshes absolute bone transforms.
         * @param rootTransform Transform applied above the skeleton root.
         * @param transforms Local bone transforms to compose.
         */
        void UpdateWorldTransforms(const Matrix& rootTransform,
                                   const std::vector<Matrix>& transforms);

        /** @brief Refreshes final skinning transforms from inverse bind pose and world transforms. */
        void UpdateSkinTransforms();

        /**
         * @brief Gets bone transforms relative to parent bones.
         * @return Current local bone transforms.
         */
        [[nodiscard]] const std::vector<Matrix>& GetBoneTransforms() const
        {
            return boneTransforms;
        }

        /**
         * @brief Gets absolute bone transforms.
         * @return Current world-space bone transforms.
         */
        [[nodiscard]] const std::vector<Matrix>& GetWorldTransforms() const
        {
            return worldTransforms;
        }

        /**
         * @brief Gets transforms relative to the skinning bind pose.
         * @return Current GPU-ready skin transforms.
         */
        [[nodiscard]] const std::vector<Matrix>& GetSkinTransforms() const
        {
            return skinTransforms;
        }

        /**
         * @brief Gets the clip currently being decoded.
         * @return Active clip, or null before StartClip.
         */
        [[nodiscard]] const AnimationClip* getCurrentClipProperty() const { return currentClip; }

        /**
         * @brief Gets the current play position.
         * @return Current time within the active clip.
         */
        [[nodiscard]] System::TimeSpan getCurrentTimeProperty() const { return currentTime; }

    private:
        const AnimationClip* currentClip = nullptr;
        System::TimeSpan currentTime;
        int currentKeyframe = 0;

        std::vector<Matrix> boneTransforms;
        std::vector<Matrix> worldTransforms;
        std::vector<Matrix> skinTransforms;

        const SkinningData* skinningData;
    };
}
