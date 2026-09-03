// SPDX-License-Identifier: MS-PL

#include "Graphics/Material.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Vector4;
    using namespace Microsoft::Xna::Framework::Graphics;

    Material::Material(Effect& effect)
        : diffuseColor(Color(210, 210, 210).ToVector4()),
          ambientColor(Color(40, 40, 40).ToVector4()),
          specularColor(Color(255, 255, 255).ToVector4())
    {
        auto& parameters = effect.getParametersProperty();
        if (EffectParameter* parameter = parameters["diffuseTexture"])
            diffuseTexture = parameter->GetValueTexture2D();
        if (EffectParameter* parameter = parameters["normalTexture"])
            normalTexture = parameter->GetValueTexture2D();
        if (EffectParameter* parameter = parameters["diffuseColor"])
            diffuseColor = parameter->GetValueVector4();
        if (EffectParameter* parameter = parameters["ambientColor"])
            ambientColor = parameter->GetValueVector4();
        if (EffectParameter* parameter = parameters["specularColor"])
            specularColor = parameter->GetValueVector4();
        if (EffectParameter* parameter = parameters["specularPower"])
            specularPower = parameter->GetValueSingle();
    }

    Texture2D* Material::getDiffuseTextureProperty() const
    {
        return diffuseTexture;
    }

    Texture2D* Material::getNormalTextureProperty() const
    {
        return normalTexture;
    }

    const Vector4& Material::getDiffuseColorProperty() const
    {
        return diffuseColor;
    }

    const Vector4& Material::getAmbientColorProperty() const
    {
        return ambientColor;
    }

    const Vector4& Material::getSpecularColorProperty() const
    {
        return specularColor;
    }

    float Material::getSpecularPowerProperty() const
    {
        return specularPower;
    }

    bool Material::getHasAlphaProperty() const
    {
        if (!diffuseTexture) return false;
        const SurfaceFormat format = diffuseTexture->getFormatProperty();
        return format == SurfaceFormat::Dxt3 || format == SurfaceFormat::Dxt5;
    }

    bool Material::Equals(const Material& other) const
    {
        return diffuseTexture == other.diffuseTexture &&
               normalTexture == other.normalTexture &&
               diffuseColor == other.diffuseColor &&
               ambientColor == other.ambientColor &&
               specularColor == other.specularColor &&
               specularPower == other.specularPower;
    }
}
