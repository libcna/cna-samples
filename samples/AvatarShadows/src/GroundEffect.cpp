// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// GroundEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "GroundEffect.hpp"

#include <utility>

#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"

namespace AvatarShadows
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    GroundEffect::GroundEffect(std::shared_ptr<Effect> effect)
        : baseEffect(std::move(effect))
    {
        // Get the parameters from the effect
        auto& parameters = baseEffect->getParametersProperty();
        world = parameters["World"];
        view = parameters["View"];
        projection = parameters["Projection"];
        texture = parameters["Texture"];
        shadow = parameters["ShadowTexture"];
    }

    Effect& GroundEffect::getBaseEffectProperty() const { return *baseEffect; }

    Matrix GroundEffect::getWorldProperty() const { return world->GetValueMatrix(); }
    void GroundEffect::setWorldProperty(const Matrix& value) { world->SetValue(value); }
    Matrix GroundEffect::getViewProperty() const { return view->GetValueMatrix(); }
    void GroundEffect::setViewProperty(const Matrix& value) { view->SetValue(value); }
    Matrix GroundEffect::getProjectionProperty() const { return projection->GetValueMatrix(); }
    void GroundEffect::setProjectionProperty(const Matrix& value) { projection->SetValue(value); }

    Texture2D* GroundEffect::getTextureProperty() const { return texture->GetValueTexture2D(); }
    void GroundEffect::setTextureProperty(Texture2D* value) { texture->SetValue(value); }
    Texture2D* GroundEffect::getShadowProperty() const { return shadow->GetValueTexture2D(); }
    void GroundEffect::setShadowProperty(Texture2D* value) { shadow->SetValue(value); }
}
