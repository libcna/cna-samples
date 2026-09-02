// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::Graphics
{
    struct TangentVertex
    {
        Microsoft::Xna::Framework::Vector3 pos;
        Microsoft::Xna::Framework::Vector2 uv;
        Microsoft::Xna::Framework::Vector3 normal;
        Microsoft::Xna::Framework::Vector3 tangent;

        [[nodiscard]] static int getSizeInBytesProperty();
        [[nodiscard]] float getUProperty() const;
        void setUProperty(float value);
        [[nodiscard]] float getVProperty() const;
        void setVProperty(float value);

        TangentVertex() = default;
        TangentVertex(Microsoft::Xna::Framework::Vector3 setPos,
                      Microsoft::Xna::Framework::Vector2 setUv,
                      Microsoft::Xna::Framework::Vector3 setNormal,
                      Microsoft::Xna::Framework::Vector3 setTangent);
        TangentVertex(float x, float y, float z,
                      float u, float v,
                      float normalX, float normalY, float normalZ,
                      float tangentX, float tangentY, float tangentZ);

        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] static const Microsoft::Xna::Framework::Graphics::VertexDeclaration&
        getVertexDeclarationStatic();
        [[nodiscard]] static bool IsTangentVertexDeclaration(
            const Microsoft::Xna::Framework::Graphics::VertexDeclaration& declaration);
    };

    static_assert(sizeof(TangentVertex) == 44);
}
