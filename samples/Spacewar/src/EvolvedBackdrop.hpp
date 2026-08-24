// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>

#include "Shape.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectParameter;
}

namespace Spacewar
{
    class EvolvedBackdrop final : public Shape
    {
    public:
        explicit EvolvedBackdrop(Microsoft::Xna::Framework::Game* game);
        void Create() override;
        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Render() override;
        void Dispose() override;

    private:
        static constexpr int XCount = 1;
        static constexpr int YCount = 1;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect_;
        float layerFactor_ = 0.0f;
        float timeFactor1_ = 0.0f;
        float timeFactor2_ = 0.0f;
        Microsoft::Xna::Framework::Vector4 layer1Offset_;
        Microsoft::Xna::Framework::Vector4 layer2Offset_;
        Microsoft::Xna::Framework::Graphics::EffectParameter* layer1TextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* layer2TextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* layer3TextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* layerFactorParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* layer1OffsetParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* layer2OffsetParam_ = nullptr;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> layer1_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> layer2_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> layer3_;
    };
}
