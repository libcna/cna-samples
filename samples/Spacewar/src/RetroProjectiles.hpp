// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <vector>

#include "Projectiles.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectParameter;
    class VertexBuffer;
}

namespace Spacewar
{
    class RetroProjectiles final : public Projectiles
    {
    public:
        explicit RetroProjectiles(Microsoft::Xna::Framework::Game* game);
        void Add(Microsoft::Xna::Framework::PlayerIndex player,
                 Microsoft::Xna::Framework::Vector3 position,
                 Microsoft::Xna::Framework::Vector3 velocity,
                 float angle, System::TimeSpan time, Particles* particles) override;
        void Render() override;
        void Create();
        void OnCreateDevice() override;

    private:
        static constexpr int MaxTriangleCount = 200;
        static constexpr int MaxProjectileCount = MaxTriangleCount * 3;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect_;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldViewProjectionParam_ = nullptr;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> buffer_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> data_;
    };
}
