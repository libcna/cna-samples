// SPDX-License-Identifier: MS-PL

#include "RetroShip.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    RetroShip::RetroShip(Game* game)
        : VectorShape(game)
    {
        Create();
    }

    void RetroShip::FillBuffer(std::vector<VertexPositionColor>& data)
    {
        data[0] = VertexPositionColor(Vector3(0.0f, 1.5f, 0.0f), Color::White);
        data[1] = VertexPositionColor(Vector3(-1.0f, -1.5f, 0.0f), Color::White);
        data[2] = data[1];
        data[3] = VertexPositionColor(Vector3(0.0f, -1.0f, 0.0f), Color::White);
        data[4] = data[3];
        data[5] = VertexPositionColor(Vector3(1.0f, -1.5f, 0.0f), Color::White);
        data[6] = data[5];
        data[7] = data[0];
    }

    int RetroShip::getNumberOfVectorsProperty() const { return 4; }
    void RetroShip::Render() { VectorShape::Render(); }
}
