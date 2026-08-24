// SPDX-License-Identifier: MS-PL

#include "RetroSun.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    RetroSun::RetroSun(Game* game)
        : VectorShape(game)
    {
        Create();
    }

    void RetroSun::FillBuffer(std::vector<VertexPositionColor>& data)
    {
        const float r2 = std::sqrt(2.0f);
        data[0] = VertexPositionColor(Vector3(0, 2, 0), Color::White);
        data[1] = VertexPositionColor(Vector3(0, -2, 0), Color::White);
        data[2] = VertexPositionColor(Vector3(2, 0, 0), Color::White);
        data[3] = VertexPositionColor(Vector3(-2, 0, 0), Color::White);
        data[4] = VertexPositionColor(Vector3(-r2, r2, 0), Color::Gray);
        data[5] = VertexPositionColor(Vector3(r2, -r2, 0), Color::Gray);
        data[6] = VertexPositionColor(Vector3(r2, r2, 0), Color::Gray);
        data[7] = VertexPositionColor(Vector3(-r2, -r2, 0), Color::Gray);
    }

    int RetroSun::getNumberOfVectorsProperty() const { return 4; }
}
