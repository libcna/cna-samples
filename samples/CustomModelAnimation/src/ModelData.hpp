// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelData.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ModelAnimationClip.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"

namespace CustomModelAnimation
{
    using Microsoft::Xna::Framework::Matrix;

    /** @brief The clips a model carries, keyed by name. */
    using ModelAnimationClipDictionary =
        std::unordered_map<std::string, std::shared_ptr<ModelAnimationClip>>;

    /**
     * @brief Combines all the data needed to render and animate a skinned object.
     *
     * This is typically stored in the `Tag` property of the model being animated.
     */
    class ModelData : public System::Object
    {
    public:
        /**
         * @brief Constructs a new skinning data object.
         *
         * @param modelAnimationClips The clips that operate on the model's own bones.
         * @param rootAnimationClips The clips that operate on the root of the object.
         * @param bindPose Bind-pose matrices for each bone in the skeleton, relative to the parent.
         * @param inverseBindPose Vertex-to-bone-space transforms for each bone in the skeleton.
         * @param skeletonHierarchy For each bone in the skeleton, the index of the parent bone.
         */
        ModelData(ModelAnimationClipDictionary modelAnimationClips,
                  ModelAnimationClipDictionary rootAnimationClips,
                  std::vector<Matrix> bindPose,
                  std::vector<Matrix> inverseBindPose,
                  std::vector<int> skeletonHierarchy);

        /**
         * @brief Constructs empty model data for the XNB deserializer to fill in.
         *
         * Public for the same reason @ref ModelKeyframe's is.
         */
        ModelData() = default;

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "CustomModelAnimation.ModelData".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets a collection of animation clips that operate on the root of the object.
         *
         * These are stored by name, so there could for instance be clips for "Walk", "Run",
         * "JumpReallyHigh", etc.
         *
         * @return The root animation clips.
         */
        [[nodiscard]] const ModelAnimationClipDictionary& getRootAnimationClipsProperty() const
        {
            return rootAnimationClips;
        }

        /**
         * @brief Gets a collection of model animation clips, stored by name.
         * @return The model animation clips.
         */
        [[nodiscard]] const ModelAnimationClipDictionary& getModelAnimationClipsProperty() const
        {
            return modelAnimationClips;
        }

        /**
         * @brief Gets the bind-pose matrices for each bone in the skeleton, relative to the parent.
         * @return The bind pose.
         */
        [[nodiscard]] const std::vector<Matrix>& getBindPoseProperty() const { return bindPose; }

        /**
         * @brief Gets the vertex-to-bone-space transforms for each bone in the skeleton.
         * @return The inverse bind pose.
         */
        [[nodiscard]] const std::vector<Matrix>& getInverseBindPoseProperty() const
        {
            return inverseBindPose;
        }

        /**
         * @brief Gets, for each bone in the skeleton, the index of the parent bone.
         * @return The skeleton hierarchy.
         */
        [[nodiscard]] const std::vector<int>& getSkeletonHierarchyProperty() const
        {
            return skeletonHierarchy;
        }

    private:
        friend struct ModelContentReaderRegistrationEXT;

        // Declared in the order IntermediateSerializer wrote them, which is the order the readers
        // consume them in -- the properties in declaration order, root clips first.
        ModelAnimationClipDictionary rootAnimationClips;
        ModelAnimationClipDictionary modelAnimationClips;
        std::vector<Matrix> bindPose;
        std::vector<Matrix> inverseBindPose;
        std::vector<int> skeletonHierarchy;
    };
}
