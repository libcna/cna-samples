#pragma once

// Animation.hpp -- C++ port of RolePlayingGameData/Animation/Animation.cs.

#include "System/Xml/Serialization/XmlSerializer.hpp"
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "ContentObject.hpp"

namespace RolePlayingGameData {

// An animation description for an AnimatingSprite object.
class Animation : public ContentObject {
public:
    std::string Name;
    int StartingFrame = 0;
    int EndingFrame = 0;
    int Interval = 0;
    bool IsLoop = false;

    SHARP_XML_SERIALIZABLE(Animation, "Animation", SHARP_XML_M(Animation, Name),
                           SHARP_XML_M(Animation, StartingFrame),
                           SHARP_XML_M(Animation, EndingFrame),
                           SHARP_XML_M(Animation, Interval), SHARP_XML_M(Animation, IsLoop))

    Animation() = default;
    Animation(std::string name, int startingFrame, int endingFrame, int interval, bool isLoop)
        : Name(std::move(name)), StartingFrame(startingFrame), EndingFrame(endingFrame),
          Interval(interval), IsLoop(isLoop) {}
};

// Reads an Animation object from the content pipeline.
class AnimationReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Animation>> {
public:
    AnimationReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Animation>>(
              "RolePlayingGameData.Animation") {}

protected:
    std::shared_ptr<Animation> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Animation>> existingInstance) override {
        std::shared_ptr<Animation> animation =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (animation == nullptr) {
            animation = std::make_shared<Animation>();
        }

        animation->SetAssetName(input.getAssetNameProperty());
        animation->Name = input.ReadString();
        animation->StartingFrame = static_cast<int>(input.ReadInt32());
        animation->EndingFrame = static_cast<int>(input.ReadInt32());
        animation->Interval = static_cast<int>(input.ReadInt32());
        animation->IsLoop = input.ReadBoolean();

        return animation;
    }
};

} // namespace RolePlayingGameData
