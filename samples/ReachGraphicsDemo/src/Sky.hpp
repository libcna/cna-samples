#pragma once

#include <stdexcept>
#include <utility>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace GeneratedGeometry {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

class Sky {
public:
    Sky(Model model, Texture2D texture)
        : ModelValue(std::move(model)), Texture(std::move(texture)) {}

    void Draw(Matrix view, const Matrix& projection) {
        GraphicsDevice& graphicsDevice = *Texture.getGraphicsDeviceProperty();

        graphicsDevice.setBlendStateProperty(BlendState::Opaque);
        graphicsDevice.setRasterizerStateProperty(RasterizerState::CullNone);
        graphicsDevice.setDepthStencilStateProperty(DepthStencilState::None);
        graphicsDevice.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        view.setTranslationProperty(Vector3::Zero);

        for (ModelMesh* mesh : ModelValue.getMeshesProperty()) {
            for (Effect* effect : mesh->getEffectsPropertyMutable()) {
                auto* basicEffect = dynamic_cast<BasicEffect*>(effect);
                if (!basicEffect) {
                    throw std::runtime_error("Sky model contains a non-BasicEffect mesh.");
                }
                basicEffect->View = view;
                basicEffect->Projection = projection;
                basicEffect->setTextureProperty(&Texture);
                basicEffect->setTextureEnabledProperty(true);
            }

            mesh->Draw();
        }
    }

    Model ModelValue;
    Texture2D Texture;
};

} // namespace GeneratedGeometry
