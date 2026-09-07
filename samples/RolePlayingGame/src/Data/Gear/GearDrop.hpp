#pragma once

// GearDrop.hpp -- C++ port of RolePlayingGameData/Gear/GearDrop.cs.

#include <memory>
#include <optional>
#include <algorithm>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// Description of how often a particular gear drops, typically from a Monster.
class GearDrop {
public:
    std::string GearName;

    int DropPercentage() const { return dropPercentage_; }
    void SetDropPercentage(int v) { dropPercentage_ = std::clamp(v, 0, 100); }

private:
    int dropPercentage_ = 0;
};

// Reads a GearDrop object from the content pipeline.
class GearDropReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<GearDrop>> {
public:
    GearDropReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<GearDrop>>(
              "RolePlayingGameData.GearDrop") {}

protected:
    std::shared_ptr<GearDrop> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<GearDrop>> existingInstance) override {
        std::shared_ptr<GearDrop> gearDrop =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (gearDrop == nullptr) {
            gearDrop = std::make_shared<GearDrop>();
        }

        gearDrop->GearName = input.ReadString();
        gearDrop->SetDropPercentage(static_cast<int>(input.ReadInt32()));

        return gearDrop;
    }
};

} // namespace RolePlayingGameData
