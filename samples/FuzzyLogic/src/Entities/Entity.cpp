// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Entity.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Entity.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "System/Math.hpp"

#include <optional>

namespace FuzzyLogic
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Vector4;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;

    Entity::Entity(Rectangle levelBoundary)
        : levelBoundary(levelBoundary)
    {
    }

    void Entity::LoadContent(ContentManager& content)
    {
        texture = content.Load<Texture2D>(getTextureFileProperty());
    }

    void Entity::Draw(SpriteBatch& spriteBatch, const GameTime& gameTime)
    {
        Color tintColor = Color::White;

        // If the entity is highlighted, we want to make it pulse with a red tint.
        if (getIsHighlightedProperty())
        {
            // To do this, we'll first generate a value t, which we'll use to
            // determine how much tint to have.
            float t = (float)System::Math::Sin(
                10 * gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

            // Sin varies from -1 to 1, and we want t to go from 0 to 1, so we'll
            // scale it now.
            t = .5f + .5f * t;

            // Finally, we'll calculate our tint color by using Lerp to generate
            // a color in between Red and White.
            tintColor = Color(Vector4::Lerp(
                Color::Red.ToVector4(), Color::White.ToVector4(), t));
        }

        // Draw the entity, centered around its position, and using the orientation
        // and tint color.
        Vector2 textureCenter((float)(texture.getWidthProperty() / 2),
                              (float)(texture.getHeightProperty() / 2));
        spriteBatch.Draw(texture, getPositionProperty(), std::nullopt, tintColor,
            orientation, textureCenter, 1.0f, SpriteEffects::None, 0.0f);
    }

    void Entity::Update(const GameTime& gameTime)
    {
        // Use ChooseBehavior to decide what the next behavior is. this is an
        // abstract method that our subclasses will implement.
        ChooseBehavior(gameTime);

        if (getCurrentBehaviorProperty() != nullptr)
        {
            getCurrentBehaviorProperty()->Update();
        }

        Vector2 heading(
            (float)System::Math::Cos(getOrientationProperty()),
            (float)System::Math::Sin(getOrientationProperty()));

        setPositionProperty(getPositionProperty() + heading * getCurrentSpeedProperty());
        setPositionProperty(ClampToLevelBoundary(getPositionProperty()));
    }

    Vector2 Entity::ClampToLevelBoundary(Vector2 vector) const
    {
        vector.X = MathHelper::Clamp(vector.X, (float)levelBoundary.X,
                                     (float)levelBoundary.getRightProperty());
        vector.Y = MathHelper::Clamp(vector.Y, (float)levelBoundary.Y,
                                     (float)levelBoundary.getBottomProperty());
        return vector;
    }
}
