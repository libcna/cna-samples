// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "Shape.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectParameter;
}

namespace Spacewar
{
    class RetroStarfield final : public Shape
    {
    public:
        explicit RetroStarfield(Microsoft::Xna::Framework::Game* game);
        void Create() override;
        void Render() override;
        void OnCreateDevice() override;

    private:
        static constexpr int NumberOfTriangles = 800;
        static constexpr int NumberOfPoints = NumberOfTriangles * 3;
        static constexpr int PercentBigStars = 20;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect_;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldViewProjectionParam_ = nullptr;
    };
}
