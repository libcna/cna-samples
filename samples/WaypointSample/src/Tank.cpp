// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Tank.hpp"

#include <optional>

#include "Behaviors/LinearBehavior.hpp"
#include "Behaviors/SteeringBehavior.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "System/Math.hpp"

namespace Waypoint
{
    using namespace Microsoft::Xna::Framework::Graphics;

    std::string BehaviorTypeToString(BehaviorType type)
    {
        switch (type)
        {
            case BehaviorType::Linear:   return "Linear";
            case BehaviorType::Steering: return "Steering";
        }
        return "Linear";
    }

    Tank::Tank(Game& game)
        : DrawableGameComponent(game)
    {
        location = Vector2::Zero;
        setBehaviorTypeProperty(BehaviorType::Linear);
    }

    Tank::~Tank() = default;

    const std::string& Tank::GetTypeName() const
    {
        static const std::string name = "Waypoint.Tank";
        return name;
    }

    void Tank::setBehaviorTypeProperty(BehaviorType value)
    {
        if (behaviorType != value || currentBehavior == nullptr)
        {
            behaviorType = value;
            switch (behaviorType)
            {
                case BehaviorType::Linear:
                    currentBehavior = std::make_unique<LinearBehavior>(*this);
                    break;
                case BehaviorType::Steering:
                    currentBehavior = std::make_unique<SteeringBehavior>(*this);
                    break;
                default:
                    break;
            }
        }
    }

    float Tank::getDistanceToDestinationProperty() const
    {
        return Vector2::Distance(location, waypoints.Peek());
    }

    bool Tank::getAtDestinationProperty() const
    {
        return getDistanceToDestinationProperty() < atDestinationLimit;
    }

    void Tank::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        tankTexture = getGameProperty().getContentProperty().Load<Texture2D>("tank");

        tankTextureCenter =
            Vector2((float)(tankTexture.getWidthProperty() / 2),
                    (float)(tankTexture.getHeightProperty() / 2));

        waypoints.LoadContent(getGameProperty().getContentProperty());
    }

    void Tank::Reset(Vector2 newLocation)
    {
        location = newLocation;
        waypoints.Clear();
    }

    void Tank::Update(GameTime& gameTime)
    {
        float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // If we have any waypoints, the first one on the list is where
        // we want to go
        if (waypoints.getCountProperty() > 0)
        {
            if (getAtDestinationProperty())
            {
                // If we're at the destination and there is at least one
                // waypoint in the list, get rid of the first one since we're
                // there now
                waypoints.Dequeue();
            }
            else
            {
                // If we're not at the destination, call Update on our
                // behavior and then move
                if (currentBehavior != nullptr)
                {
                    currentBehavior->Update(gameTime);
                }
                location = location + (getDirectionProperty() *
                    getMoveSpeedProperty() * elapsedTime);
            }
        }
    }

    void Tank::Draw(const GameTime& gameTime)
    {
        (void)gameTime;

        waypoints.Draw(*spriteBatch);

        float facingDirection = (float)System::Math::Atan2(
            getDirectionProperty().Y, getDirectionProperty().X);

        spriteBatch->Begin();
        spriteBatch->Draw(tankTexture, location, std::nullopt, Color::White, facingDirection,
            tankTextureCenter, 1.0f, SpriteEffects::None, 0.0f);

        spriteBatch->End();
    }

    void Tank::CycleBehaviorType()
    {
        switch (behaviorType)
        {
            case BehaviorType::Linear:
                setBehaviorTypeProperty(BehaviorType::Steering);
                break;
            case BehaviorType::Steering:
            default:
                setBehaviorTypeProperty(BehaviorType::Linear);
                break;
        }
    }
}
