// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Bird.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Animals/Bird.hpp"

#include <optional>

#include "Behaviors/AlignBehavior.hpp"
#include "Behaviors/CohesionBehavior.hpp"
#include "Behaviors/FleeBehavior.hpp"
#include "Behaviors/SeparationBehavior.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "System/Math.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework::Graphics;

    Bird::Bird(Texture2D tex, Vector2 dir, Vector2 loc, int screenWidth, int screenHeight)
        : Animal(tex, screenWidth, screenHeight),
          random((int)loc.X + (int)loc.Y)
    {
        direction = dir;
        direction.Normalize();
        location = loc;
        moveSpeed = 125.0f;
        fleeing = false;
        animaltype = AnimalType::Bird;
        BuildBehaviors();
    }

    void Bird::Update(const GameTime& gameTime, AIParameters& aiParams)
    {
        float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        Vector2 randomDir = Vector2::Zero;

        randomDir.X = (float)random.NextDouble() - 0.5f;
        randomDir.Y = (float)random.NextDouble() - 0.5f;
        Vector2::Normalize(randomDir, randomDir);

        if (aiNumSeen > 0)
        {
            aiNewDir = (direction * aiParams.MoveInOldDirectionInfluence) +
                (aiNewDir * (aiParams.MoveInFlockDirectionInfluence /
                (float)aiNumSeen));
        }
        else
        {
            aiNewDir = direction * aiParams.MoveInOldDirectionInfluence;
        }

        aiNewDir += (randomDir * aiParams.MoveInRandomDirectionInfluence);
        Vector2::Normalize(aiNewDir, aiNewDir);
        aiNewDir = ChangeDirection(direction, aiNewDir,
            aiParams.MaxTurnRadians * elapsedTime);
        direction = aiNewDir;

        if (direction.LengthSquared() > .01f)
        {
            Vector2 moveAmount = direction * moveSpeed * elapsedTime;
            location = location + moveAmount;

            // wrap bird to the other side of the screen if needed
            if (location.X < 0.0f)
            {
                location.X = boundryWidth + location.X;
            }
            else if (location.X > boundryWidth)
            {
                location.X = location.X - boundryWidth;
            }

            // The line above already added `direction.Y * moveSpeed * elapsedTime` as part
            // of moveAmount, so this adds it a second time and birds drift about twice as
            // fast vertically as horizontally. That is what the XNA 4.0 original does, and
            // it is reproduced deliberately: this port preserves the original's observable
            // behaviour, quirks included, rather than the behaviour it looks like it meant.
            location.Y += direction.Y * moveSpeed * elapsedTime;
            if (location.Y < 0.0f)
            {
                location.Y = boundryHeight + location.Y;
            }
            else if (location.Y > boundryHeight)
            {
                location.Y = location.Y - boundryHeight;
            }
        }
    }

    void Bird::Draw(SpriteBatch& spriteBatch, const GameTime& gameTime)
    {
        Color tintColor = color;
        float rotation = 0.0f;
        rotation = (float)System::Math::Atan2(direction.Y, direction.X);

        // if the entity is highlighted, we want to make it pulse with a red tint.
        if (fleeing)
        {
            // to do this, we'll first generate a value t, which we'll use to
            // determine how much tint to have.
            float t = (float)System::Math::Sin(
                10 * gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

            // Sin varies from -1 to 1, and we want t to go from 0 to 1, so we'll
            // scale it now.
            t = .5f + .5f * t;

            // finally, we'll calculate our tint color by using Lerp to generate
            // a color in between Red and White.
            tintColor = Color(Vector4::Lerp(
                Color::Red.ToVector4(), Color::White.ToVector4(), t));
        }

        // Draw the animal, centered around its position, and using the
        // orientation and tint color.
        spriteBatch.Draw(texture, location, std::nullopt, tintColor,
            rotation, textureCenter, 1.0f, SpriteEffects::None, 0.0f);
    }

    void Bird::BuildBehaviors()
    {
        Behaviors catReactions;
        catReactions.Add(std::make_shared<FleeBehavior>(this));
        behaviors.Add(AnimalType::Cat, catReactions);

        Behaviors birdReactions;
        birdReactions.Add(std::make_shared<AlignBehavior>(this));
        birdReactions.Add(std::make_shared<CohesionBehavior>(this));
        birdReactions.Add(std::make_shared<SeparationBehavior>(this));
        behaviors.Add(AnimalType::Bird, birdReactions);
    }

    void Bird::ResetThink()
    {
        setFleeingProperty(false);
        aiNewDir = Vector2::Zero;
        aiNumSeen = 0;
        reactionDistance = 0.0f;
        reactionLocation = Vector2::Zero;
    }

    void Bird::ClosestLocation(const Vector2& srcLocation, const Vector2& destLocation,
                               Vector2& outLocation) const
    {
        outLocation = Vector2();
        float x = destLocation.X;
        float y = destLocation.Y;
        float dX = System::Math::Abs(destLocation.X - srcLocation.X);
        float dY = System::Math::Abs(destLocation.Y - srcLocation.Y);

        // now see if the distance between birds is closer if going off one
        // side of the map and onto the other.
        if (System::Math::Abs(boundryWidth - destLocation.X + srcLocation.X) < dX)
        {
            dX = boundryWidth - destLocation.X + srcLocation.X;
            x = destLocation.X - boundryWidth;
        }
        if (System::Math::Abs(boundryWidth - srcLocation.X + destLocation.X) < dX)
        {
            dX = boundryWidth - srcLocation.X + destLocation.X;
            x = destLocation.X + boundryWidth;
        }

        if (System::Math::Abs(boundryHeight - destLocation.Y + srcLocation.Y) < dY)
        {
            dY = boundryHeight - destLocation.Y + srcLocation.Y;
            y = destLocation.Y - boundryHeight;
        }
        if (System::Math::Abs(boundryHeight - srcLocation.Y + destLocation.Y) < dY)
        {
            dY = boundryHeight - srcLocation.Y + destLocation.Y;
            y = destLocation.Y + boundryHeight;
        }

        outLocation.X = x;
        outLocation.Y = y;
    }

    void Bird::ReactTo(Animal* animal, AIParameters& AIparams)
    {
        if (animal != nullptr)
        {
            // setting the the reactionLocation and reactionDistance here is
            // an optimization, many of the possible reactions use the distance
            // and location of theAnimal, so we might as well figure them out
            // only once !
            Vector2 otherLocation = animal->getLocationProperty();
            ClosestLocation(location, otherLocation, reactionLocation);
            reactionDistance = Vector2::Distance(location, reactionLocation);

            // we only react if theAnimal is close enough that we can see it
            if (reactionDistance < AIparams.DetectionDistance)
            {
                Behaviors reactions;
                behaviors.TryGetValue(animal->getAnimalTypeProperty(), reactions);
                for (const std::shared_ptr<Behavior>& reaction : reactions)
                {
                    reaction->Update(animal, AIparams);
                    if (reaction->getReactedProperty())
                    {
                        aiNewDir += reaction->getReactionProperty();
                        aiNumSeen++;
                    }
                }
            }
        }
    }

    Vector2 Bird::ChangeDirection(Vector2 oldDir, Vector2 newDir, float maxTurnRadians)
    {
        float oldAngle = (float)System::Math::Atan2(oldDir.Y, oldDir.X);
        float desiredAngle = (float)System::Math::Atan2(newDir.Y, newDir.X);
        float newAngle = MathHelper::Clamp(desiredAngle, WrapAngle(
                oldAngle - maxTurnRadians), WrapAngle(oldAngle + maxTurnRadians));
        return Vector2((float)System::Math::Cos(newAngle), (float)System::Math::Sin(newAngle));
    }

    float Bird::WrapAngle(float radians)
    {
        while (radians < -MathHelper::Pi)
        {
            radians += MathHelper::TwoPi;
        }
        while (radians > MathHelper::Pi)
        {
            radians -= MathHelper::TwoPi;
        }
        return radians;
    }
}
