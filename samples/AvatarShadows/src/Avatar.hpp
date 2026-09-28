// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Avatar.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimation.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarDescription.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Random.hpp"

namespace AvatarShadows
{
    /**
     * @brief Contains all of the data needed to render a single avatar.
     */
    class Avatar
    {
    public:
        /** @brief Picks a random animation and description for the new avatar. */
        Avatar();

        /** @brief Gets the animation used by the avatar. @return The animation. */
        [[nodiscard]] Microsoft::Xna::Framework::GamerServices::AvatarAnimation& getAnimationProperty() const;
        /** @brief Gets the description of the avatar. @return The description. */
        [[nodiscard]] Microsoft::Xna::Framework::GamerServices::AvatarDescription& getDescriptionProperty() const;
        /** @brief Gets the renderer used by the avatar. @return The renderer. */
        [[nodiscard]] Microsoft::Xna::Framework::GamerServices::AvatarRenderer& getRendererProperty() const;

        /** @brief Gets the world matrix for drawing this avatar. @return World matrix. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getWorldProperty() const;
        /** @brief Sets the world matrix for drawing this avatar. @param value World matrix. */
        void setWorldProperty(const Microsoft::Xna::Framework::Matrix& value);

    private:
        // We use one random number generator for all avatars
        static System::Random random;

        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarAnimation> animation;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarDescription> description;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarRenderer> renderer;
        Microsoft::Xna::Framework::Matrix world;
    };
}
