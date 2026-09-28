// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Avatar.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Avatar.hpp"

#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimationPreset.hpp"

namespace AvatarShadows
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;

    System::Random Avatar::random;

    Avatar::Avatar()
    {
        // We pick a random animation and description for each avatar we create
        animation = std::make_unique<AvatarAnimation>(static_cast<AvatarAnimationPreset>(random.Next(30)));
        description = std::make_unique<AvatarDescription>(AvatarDescription::CreateRandom());
        renderer = std::make_unique<AvatarRenderer>(description.get(), false);
    }

    AvatarAnimation& Avatar::getAnimationProperty() const { return *animation; }
    AvatarDescription& Avatar::getDescriptionProperty() const { return *description; }
    AvatarRenderer& Avatar::getRendererProperty() const { return *renderer; }
    Matrix Avatar::getWorldProperty() const { return world; }
    void Avatar::setWorldProperty(const Matrix& value) { world = value; }
}
