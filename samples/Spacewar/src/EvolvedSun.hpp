// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <optional>

#include "Shape.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectParameter;
}

namespace Spacewar
{
    class EvolvedSun final : public Shape
    {
    public:
        explicit EvolvedSun(Microsoft::Xna::Framework::Game* game);
        void Create() override;
        void Update(System::TimeSpan timeSpan, System::TimeSpan elapsedTime) override;
        void Render() override;

    private:
        static constexpr int XCount = 1;
        static constexpr int YCount = 1;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect_;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldViewProjectionParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* sun0TextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* sun1TextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* blendFactor_ = nullptr;
        std::array<std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>, 5> sun_;
        int currentFrame_ = 0;
        double currentTime_ = 0.0;
    };
}
