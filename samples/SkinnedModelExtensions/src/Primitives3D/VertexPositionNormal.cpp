// SPDX-License-Identifier: MS-PL

#include "VertexPositionNormal.hpp"

#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"

namespace Primitives3D
{
    using namespace Microsoft::Xna::Framework::Graphics;

    const Microsoft::Xna::Framework::Graphics::VertexDeclaration
        VertexPositionNormal::VertexDeclaration(
            {
                VertexElement(0, VertexElementFormat::Vector3,
                              VertexElementUsage::Position, 0),
                VertexElement(12, VertexElementFormat::Vector3,
                              VertexElementUsage::Normal, 0),
            });
}
