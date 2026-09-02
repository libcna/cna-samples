// SPDX-License-Identifier: MS-PL

#include "Graphics/TangentVertex.hpp"

#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Graphics::VertexDeclaration;
    using Microsoft::Xna::Framework::Graphics::VertexElement;
    using Microsoft::Xna::Framework::Graphics::VertexElementFormat;
    using Microsoft::Xna::Framework::Graphics::VertexElementUsage;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    int TangentVertex::getSizeInBytesProperty() { return 44; }
    float TangentVertex::getUProperty() const { return uv.X; }
    void TangentVertex::setUProperty(float value) { uv.X = value; }
    float TangentVertex::getVProperty() const { return uv.Y; }
    void TangentVertex::setVProperty(float value) { uv.Y = value; }

    TangentVertex::TangentVertex(Vector3 setPos, Vector2 setUv,
                                 Vector3 setNormal, Vector3 setTangent)
        : pos(setPos), uv(setUv), normal(setNormal), tangent(setTangent)
    {
    }

    TangentVertex::TangentVertex(float x, float y, float z,
                                 float u, float v,
                                 float normalX, float normalY, float normalZ,
                                 float tangentX, float tangentY, float tangentZ)
        : TangentVertex(Vector3(x, y, z), Vector2(u, v),
                        Vector3(normalX, normalY, normalZ),
                        Vector3(tangentX, tangentY, tangentZ))
    {
    }

    std::string TangentVertex::ToString() const
    {
        return "TangentVertex(pos=" + pos.ToString() + ", uv=" + uv.ToString() +
               ", normal=" + normal.ToString() + ", tangent=" + tangent.ToString() + ")";
    }

    const VertexDeclaration& TangentVertex::getVertexDeclarationStatic()
    {
        static const VertexDeclaration declaration(
            44,
            {
                VertexElement(0, VertexElementFormat::Vector3,
                              VertexElementUsage::Position, 0),
                VertexElement(12, VertexElementFormat::Vector2,
                              VertexElementUsage::TextureCoordinate, 0),
                VertexElement(20, VertexElementFormat::Vector3,
                              VertexElementUsage::Normal, 0),
                VertexElement(32, VertexElementFormat::Vector3,
                              VertexElementUsage::Tangent, 0),
            });
        return declaration;
    }

    bool TangentVertex::IsTangentVertexDeclaration(const VertexDeclaration& declaration)
    {
        const auto& actual = declaration.GetVertexElements();
        const auto& expected = getVertexDeclarationStatic().GetVertexElements();
        return declaration.getVertexStrideProperty() == 44 && actual == expected;
    }
}
