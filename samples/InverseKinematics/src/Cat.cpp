// SPDX-License-Identifier: MS-PL

#include "Cat.hpp"

#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include <optional>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace InverseKinematicsSample
{
    Cat::Cat(GraphicsDevice& device)
        : graphicsDevice(device), basicEffect(device)
    {
        vertices[0].Position = Vector3(1.0f, 1.0f, 0.0f);
        vertices[1].Position = Vector3(-1.0f, 1.0f, 0.0f);
        vertices[2].Position = Vector3(-1.0f, -1.0f, 0.0f);
        vertices[3].Position = Vector3(1.0f, 1.0f, 0.0f);
        vertices[4].Position = Vector3(-1.0f, -1.0f, 0.0f);
        vertices[5].Position = Vector3(1.0f, -1.0f, 0.0f);
    }

    Single Cat::getScaleProperty() const { return scale; }
    void Cat::setScaleProperty(const Single value) { scale = value; }
    Vector3 Cat::getPositionProperty() const { return position; }
    void Cat::setPositionProperty(const Vector3& value) { position = value; }
    Vector3 Cat::getUpProperty() const { return up; }
    void Cat::setUpProperty(const Vector3& value) { up = value; }
    Texture2D* Cat::getTextureProperty() const { return texture; }
    void Cat::setTextureProperty(Texture2D* value) { texture = value; }

    void Cat::Draw(const Vector3& cameraPosition, const Matrix& view, const Matrix& projection)
    {
        const Matrix world = Matrix::CreateTranslation(0.0f, 1.0f, 0.0f)
                           * Matrix::CreateScale(scale)
                           * Matrix::CreateConstrainedBillboard(
                                 position, cameraPosition, up, std::nullopt, std::nullopt);
        DrawQuad(texture, 1.0f, world, view, projection);
    }

    void Cat::DrawQuad(Texture2D* quadTexture, const Single textureRepeats,
                       const Matrix& world, const Matrix& view, const Matrix& projection)
    {
        basicEffect.setTextureProperty(quadTexture);
        basicEffect.setTextureEnabledProperty(true);
        basicEffect.setWorldProperty(world);
        basicEffect.setViewProperty(view);
        basicEffect.setProjectionProperty(projection);

        vertices[0].TextureCoordinate = Vector2(0.0f, 0.0f);
        vertices[1].TextureCoordinate = Vector2(textureRepeats, 0.0f);
        vertices[2].TextureCoordinate = Vector2(textureRepeats, textureRepeats);
        vertices[3].TextureCoordinate = Vector2(0.0f, 0.0f);
        vertices[4].TextureCoordinate = Vector2(textureRepeats, textureRepeats);
        vertices[5].TextureCoordinate = Vector2(0.0f, textureRepeats);

        basicEffect.getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
        graphicsDevice.DrawUserPrimitives(
            PrimitiveType::TriangleList, vertices.data(), 0, 2);
    }
}
