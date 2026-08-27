// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleSystem.hpp"

#include <cmath>
#include <utility>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "System/Random.hpp"

namespace Particle3DSample
{
    namespace
    {
        // The original's `static Random random = new Random()`, shared by every particle system.
        System::Random& SharedRandom()
        {
            static System::Random random;
            return random;
        }
    }

    ParticleSystem::ParticleSystem(Game& game, Content::ContentManager& content,
                                   std::string settingsName)
        : DrawableGameComponent(game), content(&content),
          settingsName(std::move(settingsName))
    {
    }

    const std::string& ParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.ParticleSystem"};
        return typeName;
    }

    void ParticleSystem::LoadContent()
    {
        settings = content->Load<ParticleSettings>(settingsName);

        // Allocate the particle array, and fill in the corner fields (which never change).
        particles.assign(static_cast<std::size_t>(settings.MaxParticles) * 4, ParticleVertex{});

        for (int i = 0; i < settings.MaxParticles; i++)
        {
            particles[i * 4 + 0].Corner = PackedVector::Short2(-1, -1).getPackedValueProperty();
            particles[i * 4 + 1].Corner = PackedVector::Short2(1, -1).getPackedValueProperty();
            particles[i * 4 + 2].Corner = PackedVector::Short2(1, 1).getPackedValueProperty();
            particles[i * 4 + 3].Corner = PackedVector::Short2(-1, 1).getPackedValueProperty();
        }

        LoadParticleEffect();

        // Create a dynamic vertex buffer.
        vertexBuffer = std::make_unique<DynamicVertexBuffer>(
            getGraphicsDeviceProperty(), ParticleVertex::GetVertexDeclaration(),
            settings.MaxParticles * 4, BufferUsage::WriteOnly);

        // Create and populate the index buffer.
        std::vector<std::uint16_t> indices(static_cast<std::size_t>(settings.MaxParticles) * 6);

        for (int i = 0; i < settings.MaxParticles; i++)
        {
            indices[i * 6 + 0] = (std::uint16_t)(i * 4 + 0);
            indices[i * 6 + 1] = (std::uint16_t)(i * 4 + 1);
            indices[i * 6 + 2] = (std::uint16_t)(i * 4 + 2);

            indices[i * 6 + 3] = (std::uint16_t)(i * 4 + 0);
            indices[i * 6 + 4] = (std::uint16_t)(i * 4 + 2);
            indices[i * 6 + 5] = (std::uint16_t)(i * 4 + 3);
        }

        indexBuffer = std::make_unique<IndexBuffer>(
            getGraphicsDeviceProperty(), IndexElementSize::SixteenBits,
            (int)indices.size(), BufferUsage::WriteOnly);

        indexBuffer->SetData(indices.data(), (int)indices.size());
    }

    void ParticleSystem::LoadParticleEffect()
    {
        // CNA's Effect is not copyable, so the ContentManager hands it out shared.
        std::shared_ptr<Effect> effect = content->Load<std::shared_ptr<Effect>>("ParticleEffect");

        // If we have several particle systems, the content manager will return
        // a single shared effect instance to them all. But we want to preconfigure
        // the effect with parameters that are specific to this particular
        // particle system. By cloning the effect, we prevent one particle system
        // from stomping over the parameter settings of another.

        particleEffect = effect->Clone();

        EffectParameterCollection& parameters = particleEffect->getParametersProperty();

        // Look up shortcuts for parameters that change every frame.
        effectViewParameter = parameters["View"];
        effectProjectionParameter = parameters["Projection"];
        effectViewportScaleParameter = parameters["ViewportScale"];
        effectTimeParameter = parameters["CurrentTime"];

        // Set the values of parameters that do not change.
        parameters["Duration"]->SetValue((float)settings.Duration.getTotalSecondsProperty());
        parameters["DurationRandomness"]->SetValue(settings.DurationRandomness);
        parameters["Gravity"]->SetValue(settings.Gravity);
        parameters["EndVelocity"]->SetValue(settings.EndVelocity);
        parameters["MinColor"]->SetValue(settings.MinColor.ToVector4());
        parameters["MaxColor"]->SetValue(settings.MaxColor.ToVector4());

        parameters["RotateSpeed"]->SetValue(
            Vector2(settings.MinRotateSpeed, settings.MaxRotateSpeed));

        parameters["StartSize"]->SetValue(
            Vector2(settings.MinStartSize, settings.MaxStartSize));

        parameters["EndSize"]->SetValue(
            Vector2(settings.MinEndSize, settings.MaxEndSize));

        // Load the particle texture, and set it onto the effect.
        texture.emplace(content->Load<Texture2D>(settings.TextureName));

        parameters["Texture"]->SetValue(&*texture);
    }

    void ParticleSystem::Update(GameTime& gameTime)
    {
        currentTime += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        RetireActiveParticles();
        FreeRetiredParticles();

        // If we let our timer go on increasing for ever, it would eventually
        // run out of floating point precision, at which point the particles
        // would render incorrectly. An easy way to prevent this is to notice
        // that the time value doesn't matter when no particles are being drawn,
        // so we can reset it back to zero any time the active queue is empty.

        if (firstActiveParticle == firstFreeParticle)
            currentTime = 0;

        if (firstRetiredParticle == firstActiveParticle)
            drawCounter = 0;
    }

    void ParticleSystem::RetireActiveParticles()
    {
        const float particleDuration = (float)settings.Duration.getTotalSecondsProperty();

        while (firstActiveParticle != firstNewParticle)
        {
            // Is this particle old enough to retire?
            // We multiply the active particle index by four, because each
            // particle consists of a quad that is made up of four vertices.
            const float particleAge = currentTime - particles[firstActiveParticle * 4].Time;

            if (particleAge < particleDuration)
                break;

            // Remember the time at which we retired this particle.
            particles[firstActiveParticle * 4].Time = (float)drawCounter;

            // Move the particle from the active to the retired queue.
            firstActiveParticle++;

            if (firstActiveParticle >= settings.MaxParticles)
                firstActiveParticle = 0;
        }
    }

    void ParticleSystem::FreeRetiredParticles()
    {
        while (firstRetiredParticle != firstActiveParticle)
        {
            // Has this particle been unused long enough that
            // the GPU is sure to be finished with it?
            const int age = drawCounter - (int)particles[firstRetiredParticle * 4].Time;

            // The GPU is never supposed to get more than 2 frames behind the CPU.
            // We add 1 to that, just to be safe in case of buggy drivers that
            // might bend the rules and let the GPU get further behind.
            if (age < 3)
                break;

            // Move the particle from the retired to the free queue.
            firstRetiredParticle++;

            if (firstRetiredParticle >= settings.MaxParticles)
                firstRetiredParticle = 0;
        }
    }

    void ParticleSystem::Draw(const GameTime& gameTime)
    {
        (void)gameTime;
        GraphicsDevice& device = getGraphicsDeviceProperty();

        // Restore the vertex buffer contents if the graphics device was lost.
        if (vertexBuffer->getIsContentLostProperty())
        {
            vertexBuffer->SetData(particles.data(), (int)particles.size());
        }

        // If there are any particles waiting in the newly added queue,
        // we'd better upload them to the GPU ready for drawing.
        if (firstNewParticle != firstFreeParticle)
        {
            AddNewParticlesToVertexBuffer();
        }

        // If there are any active particles, draw them now!
        if (firstActiveParticle != firstFreeParticle)
        {
            device.setBlendStateProperty(settings.BlendState);
            device.setDepthStencilStateProperty(DepthStencilState::DepthRead);

            // Set an effect parameter describing the viewport size. This is
            // needed to convert particle sizes into screen space point sizes.
            effectViewportScaleParameter->SetValue(
                Vector2(0.5f / device.getViewportProperty().getAspectRatioProperty(), -0.5f));

            // Set an effect parameter describing the current time. All the vertex
            // shader particle animation is keyed off this value.
            effectTimeParameter->SetValue(currentTime);

            // Set the particle vertex and index buffer.
            device.SetVertexBuffer(vertexBuffer.get());
            device.setIndicesProperty(indexBuffer.get());

            // Activate the particle effect.
            for (EffectPass& pass : particleEffect->getCurrentTechniqueProperty()->getPassesProperty())
            {
                pass.Apply();

                if (firstActiveParticle < firstFreeParticle)
                {
                    // If the active particles are all in one consecutive range,
                    // we can draw them all in a single call.
                    device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0,
                                                 firstActiveParticle * 4,
                                                 (firstFreeParticle - firstActiveParticle) * 4,
                                                 firstActiveParticle * 6,
                                                 (firstFreeParticle - firstActiveParticle) * 2);
                }
                else
                {
                    // If the active particle range wraps past the end of the queue
                    // back to the start, we must split them over two draw calls.
                    device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0,
                                                 firstActiveParticle * 4,
                                                 (settings.MaxParticles - firstActiveParticle) * 4,
                                                 firstActiveParticle * 6,
                                                 (settings.MaxParticles - firstActiveParticle) * 2);

                    if (firstFreeParticle > 0)
                    {
                        device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0,
                                                     0, firstFreeParticle * 4,
                                                     0, firstFreeParticle * 2);
                    }
                }
            }

            // Reset some of the renderstates that we changed,
            // so as not to mess up any other subsequent drawing.
            device.setDepthStencilStateProperty(DepthStencilState::Default);
        }

        drawCounter++;
    }

    void ParticleSystem::AddNewParticlesToVertexBuffer()
    {
        const int stride = ParticleVertex::SizeInBytes;

        if (firstNewParticle < firstFreeParticle)
        {
            // If the new particles are all in one consecutive range,
            // we can upload them all in a single call.
            vertexBuffer->SetData(firstNewParticle * stride * 4, particles.data(),
                                  firstNewParticle * 4,
                                  (firstFreeParticle - firstNewParticle) * 4,
                                  stride, SetDataOptions::NoOverwrite);
        }
        else
        {
            // If the new particle range wraps past the end of the queue
            // back to the start, we must split them over two upload calls.
            vertexBuffer->SetData(firstNewParticle * stride * 4, particles.data(),
                                  firstNewParticle * 4,
                                  (settings.MaxParticles - firstNewParticle) * 4,
                                  stride, SetDataOptions::NoOverwrite);

            if (firstFreeParticle > 0)
            {
                vertexBuffer->SetData(0, particles.data(), 0, firstFreeParticle * 4,
                                      stride, SetDataOptions::NoOverwrite);
            }
        }

        // Move the particles we just uploaded from the new to the active queue.
        firstNewParticle = firstFreeParticle;
    }

    void ParticleSystem::SetCamera(const Matrix& view, const Matrix& projection)
    {
        effectViewParameter->SetValue(view);
        effectProjectionParameter->SetValue(projection);
    }

    void ParticleSystem::AddParticle(Vector3 position, Vector3 velocity)
    {
        // Figure out where in the circular queue to allocate the new particle.
        int nextFreeParticle = firstFreeParticle + 1;

        if (nextFreeParticle >= settings.MaxParticles)
            nextFreeParticle = 0;

        // If there are no free particles, we just have to give up.
        if (nextFreeParticle == firstRetiredParticle)
            return;

        // Adjust the input velocity based on how much
        // this particle system wants to be affected by it.
        velocity *= settings.EmitterVelocitySensitivity;

        // Add in some random amount of horizontal velocity.
        const float horizontalVelocity = MathHelper::Lerp(settings.MinHorizontalVelocity,
                                                          settings.MaxHorizontalVelocity,
                                                          (float)SharedRandom().NextDouble());

        const double horizontalAngle = SharedRandom().NextDouble() * MathHelper::TwoPi;

        velocity.X += horizontalVelocity * (float)std::cos(horizontalAngle);
        velocity.Z += horizontalVelocity * (float)std::sin(horizontalAngle);

        // Add in some random amount of vertical velocity.
        velocity.Y += MathHelper::Lerp(settings.MinVerticalVelocity,
                                       settings.MaxVerticalVelocity,
                                       (float)SharedRandom().NextDouble());

        // Choose four random control values. These will be used by the vertex
        // shader to give each particle a different size, rotation, and color.
        const Color randomValues((bytecs)SharedRandom().Next(255),
                                 (bytecs)SharedRandom().Next(255),
                                 (bytecs)SharedRandom().Next(255),
                                 (bytecs)SharedRandom().Next(255));

        // Fill in the particle vertex structure.
        for (int i = 0; i < 4; i++)
        {
            particles[firstFreeParticle * 4 + i].Position = position;
            particles[firstFreeParticle * 4 + i].Velocity = velocity;
            particles[firstFreeParticle * 4 + i].Random = randomValues.getPackedValueProperty();
            particles[firstFreeParticle * 4 + i].Time = currentTime;
        }

        firstFreeParticle = nextFreeParticle;
    }
}
