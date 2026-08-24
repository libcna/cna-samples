// SPDX-License-Identifier: MS-PL
#pragma once

#include "VectorShape.hpp"

namespace Spacewar
{
    class RetroShip final : public VectorShape
    {
    public:
        explicit RetroShip(Microsoft::Xna::Framework::Game* game);
        void Render() override;

    protected:
        void FillBuffer(std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor>& data) override;
        [[nodiscard]] int getNumberOfVectorsProperty() const override;
    };
}
