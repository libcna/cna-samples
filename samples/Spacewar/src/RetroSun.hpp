// SPDX-License-Identifier: MS-PL
#pragma once

#include "VectorShape.hpp"

namespace Spacewar
{
    class RetroSun final : public VectorShape
    {
    public:
        explicit RetroSun(Microsoft::Xna::Framework::Game* game);

    protected:
        void FillBuffer(std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor>& data) override;
        [[nodiscard]] int getNumberOfVectorsProperty() const override;
    };
}
