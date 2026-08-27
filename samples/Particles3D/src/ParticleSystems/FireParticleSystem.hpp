// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FireParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"

#include "../ParticleSystem.hpp"

namespace Particle3DSample
{
    /**
     * @brief Custom particle system for creating a flame effect.
     */
    class FireParticleSystem : public ParticleSystem
    {
    public:
        /**
         * @brief Constructs the particle system.
         * @param game    The game this component belongs to.
         * @param content The content manager the effect and texture are loaded from.
         */
        FireParticleSystem(Game& game, Content::ContentManager& content)
            : ParticleSystem(game, content)
        {
        }

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "Particle3DSample.FireParticleSystem".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /**
         * @brief Initalizes this system's tweakable settings.
         * @param settings The settings object to fill in.
         */
        void InitializeSettings(ParticleSettings& settings) override;
    };
}
