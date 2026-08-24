// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <vector>

#include "Shape.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectParameter;
}

namespace Spacewar
{
    class VectorShape : public Shape
    {
    public:
        explicit VectorShape(Microsoft::Xna::Framework::Game* game);

        void Create() override;
        void Render() override;
        void OnCreateDevice() override;

    protected:
        virtual void FillBuffer(std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor>& data) = 0;
        [[nodiscard]] virtual int getNumberOfVectorsProperty() const = 0;

    private:
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect_;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldViewProjectionParam_ = nullptr;
    };
}
