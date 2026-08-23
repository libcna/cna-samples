#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace SkinnedModel {

using Microsoft::Xna::Framework::Matrix;

class Keyframe : public System::Object {
public:
    Keyframe(int bone, System::TimeSpan time, Matrix transform)
        : Bone(bone), Time(time), Transform(transform) {}

    const std::string& GetTypeName() const override {
        static const std::string name = "SkinnedModel.Keyframe";
        return name;
    }

    int Bone;
    System::TimeSpan Time;
    Matrix Transform;
};

class AnimationClip : public System::Object {
public:
    AnimationClip(System::TimeSpan duration,
                  std::vector<std::shared_ptr<Keyframe>> keyframes)
        : Duration(duration), Keyframes(std::move(keyframes)) {}

    const std::string& GetTypeName() const override {
        static const std::string name = "SkinnedModel.AnimationClip";
        return name;
    }

    System::TimeSpan Duration;
    std::vector<std::shared_ptr<Keyframe>> Keyframes;
};

class SkinningData : public System::Object {
public:
    using AnimationClipDictionary =
        std::unordered_map<std::string, std::shared_ptr<AnimationClip>>;

    SkinningData(AnimationClipDictionary animationClips,
                 std::vector<Matrix> bindPose,
                 std::vector<Matrix> inverseBindPose,
                 std::vector<int> skeletonHierarchy)
        : AnimationClips(std::move(animationClips)),
          BindPose(std::move(bindPose)),
          InverseBindPose(std::move(inverseBindPose)),
          SkeletonHierarchy(std::move(skeletonHierarchy)) {}

    const std::string& GetTypeName() const override {
        static const std::string name = "SkinnedModel.SkinningData";
        return name;
    }

    AnimationClipDictionary AnimationClips;
    std::vector<Matrix> BindPose;
    std::vector<Matrix> InverseBindPose;
    std::vector<int> SkeletonHierarchy;
};

class AnimationPlayer {
public:
    explicit AnimationPlayer(const SkinningData& skinningData)
        : skinningData_(&skinningData),
          boneTransforms_(skinningData.BindPose.size()),
          worldTransforms_(skinningData.BindPose.size()),
          skinTransforms_(skinningData.BindPose.size()) {}

    void StartClip(const AnimationClip& clip) {
        currentClip_ = &clip;
        currentTime_ = System::TimeSpan::Zero;
        currentKeyframe_ = 0;
        boneTransforms_ = skinningData_->BindPose;
    }

    void Update(System::TimeSpan time, bool relativeToCurrentTime,
                const Matrix& rootTransform) {
        UpdateBoneTransforms(time, relativeToCurrentTime);
        UpdateWorldTransforms(rootTransform);
        UpdateSkinTransforms();
    }

    void UpdateBoneTransforms(System::TimeSpan time, bool relativeToCurrentTime) {
        if (!currentClip_) {
            throw std::runtime_error(
                "AnimationPlayer.Update was called before StartClip");
        }

        if (relativeToCurrentTime) {
            time = time + currentTime_;
            while (time >= currentClip_->Duration) {
                time = time - currentClip_->Duration;
            }
        }

        if (time < System::TimeSpan::Zero || time >= currentClip_->Duration) {
            throw std::out_of_range("time");
        }

        if (time < currentTime_) {
            currentKeyframe_ = 0;
            boneTransforms_ = skinningData_->BindPose;
        }

        currentTime_ = time;

        while (currentKeyframe_ < currentClip_->Keyframes.size()) {
            const Keyframe& keyframe =
                *currentClip_->Keyframes[currentKeyframe_];
            if (keyframe.Time > currentTime_) {
                break;
            }

            boneTransforms_[static_cast<std::size_t>(keyframe.Bone)] =
                keyframe.Transform;
            ++currentKeyframe_;
        }
    }

    void UpdateWorldTransforms(const Matrix& rootTransform) {
        worldTransforms_[0] = boneTransforms_[0] * rootTransform;

        for (std::size_t bone = 1; bone < worldTransforms_.size(); ++bone) {
            int parentBone = skinningData_->SkeletonHierarchy[bone];
            worldTransforms_[bone] = boneTransforms_[bone] *
                worldTransforms_[static_cast<std::size_t>(parentBone)];
        }
    }

    void UpdateSkinTransforms() {
        for (std::size_t bone = 0; bone < skinTransforms_.size(); ++bone) {
            skinTransforms_[bone] = skinningData_->InverseBindPose[bone] *
                worldTransforms_[bone];
        }
    }

    const std::vector<Matrix>& GetBoneTransforms() const {
        return boneTransforms_;
    }

    const std::vector<Matrix>& GetWorldTransforms() const {
        return worldTransforms_;
    }

    const std::vector<Matrix>& GetSkinTransforms() const {
        return skinTransforms_;
    }

    const AnimationClip* GetCurrentClip() const { return currentClip_; }
    System::TimeSpan GetCurrentTime() const { return currentTime_; }

private:
    const AnimationClip* currentClip_ = nullptr;
    System::TimeSpan currentTime_;
    std::size_t currentKeyframe_ = 0;
    std::vector<Matrix> boneTransforms_;
    std::vector<Matrix> worldTransforms_;
    std::vector<Matrix> skinTransforms_;
    const SkinningData* skinningData_;
};

} // namespace SkinnedModel
