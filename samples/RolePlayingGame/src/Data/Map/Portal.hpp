#pragma once

// Portal.hpp -- C++ port of RolePlayingGameData/Map/Portal.cs.

#include <memory>
#include <optional>
#include <vector>
#include <string>

#include "Microsoft/Xna/Framework/Point.hpp"

#include "../ContentObject.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A transition point from one map to another.
class Portal : public ContentObject {
public:
    std::string Name;

    // The map coordinate that the party will automatically walk to after spawning on this portal.
    Microsoft::Xna::Framework::Point LandingMapPosition;

    std::string DestinationMapContentName;
    std::string DestinationMapPortalName;
};

// Reads a Portal object from the content pipeline.
class PortalReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Portal>> {
public:
    PortalReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Portal>>(
              "RolePlayingGameData.Portal") {}

protected:
    std::shared_ptr<Portal> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Portal>> existingInstance) override {
        std::shared_ptr<Portal> portal =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (portal == nullptr) {
            portal = std::make_shared<Portal>();
        }

        portal->SetAssetName(input.getAssetNameProperty());
        portal->Name = input.ReadString();
        portal->LandingMapPosition = input.ReadObject<Microsoft::Xna::Framework::Point>();
        portal->DestinationMapContentName = input.ReadString();
        portal->DestinationMapPortalName = input.ReadString();

        return portal;
    }
};

} // namespace RolePlayingGameData
