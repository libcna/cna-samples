#pragma once

// WorldObject.hpp -- C++ port of RolePlayingGameData/WorldObject.cs.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "System/ArgumentNullException.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"

#include "ContentObject.hpp"

namespace RolePlayingGameData {

// Common base class for all objects that are visible in the world.
class WorldObject : public ContentObject {
public:
    const std::string& Name() const { return name_; }
    void SetName(const std::string& v) { name_ = v; }

private:
    std::string name_;
};

// Read a WorldObject object from the content pipeline.
//
// The original is a nested ContentTypeReader<WorldObject>. Every derived reader invokes it with
// ReadRawObject<WorldObject>(instance) to read the base part in place, so this reader never
// constructs anything: an existing instance is required, exactly as the original requires one.
class WorldObjectReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<WorldObject>> {
public:
    WorldObjectReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<WorldObject>>(
              "RolePlayingGameData.WorldObject") {}

protected:
    std::shared_ptr<WorldObject> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<WorldObject>> existingInstance) override {
        // we cannot create this object, so there must be an existing instance
        if (!existingInstance.has_value() || *existingInstance == nullptr) {
            throw System::ArgumentNullException("existingInstance");
        }

        const std::shared_ptr<WorldObject>& worldObject = *existingInstance;
        worldObject->SetAssetName(input.getAssetNameProperty());
        worldObject->SetName(input.ReadString());

        return worldObject;
    }
};

} // namespace RolePlayingGameData
