// SPDX-License-Identifier: MS-PL
#pragma once

// ParticleSystem.hpp — C++ port of ShipGame/Graphics/ParticleSystem.cs (XNA 4.0
// Ship Game Starter Kit). One emitter's worth of point-sprite particles: the
// CPU holds their birth velocity and Particle.fx animates them from it.

#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include "System/Random.hpp"

#include "../GameManager.hpp"
#include "AnimSpriteManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;

// Port of the Particle class in ShipGame/Graphics/ParticleSystem.cs.
class Particle {
public:
    Vector3 position; // particle position
    Vector3 velocity; // particle velocity
    Vector2 random;   // two normalized random numbers

    // Create a new particle
    Particle(Vector3 particlePosition, Vector3 particleVelocity, Vector2 particleRandom)
        : position(particlePosition), velocity(particleVelocity), random(particleRandom) {}
};

// Port of ShipGame/Graphics/ParticleSystem.cs.
class ParticleSystem {
public:
    // Create a new particle system
    ParticleSystem(ParticleSystemType type, int count, float angle, float particleTime,
                   float totalTime, float minimumSize, float maximumSize, float minimumVelocity,
                   float maximumVelocity, Vector4 startColor, Vector4 endColor, Texture2D* texture,
                   DrawMode mode, Matrix transform) {
        particleType_ = type;
        enabled_ = true;
        loop_ = false;

        count_ = count;
        particles_.reserve(count);

        velocity_ = Vector2(minimumVelocity, maximumVelocity);

        pointSize_ = Vector2(minimumSize, maximumSize);

        emissionAngle_ = angle;
        velocityScale_ = 1.0f;

        totalTime_ = totalTime;
        particleTime_ = particleTime;

        texture_ = texture;
        drawMode_ = mode;

        transform_ = transform;

        startColor_ = startColor;
        endColor_ = endColor;

        // if particle life is less then system life we have a loop
        if (particleTime < totalTime) {
            // start at a negative time offset so particles flow
            // into birth when life turns positive
            elapsedTime_ = -particleTime;
            loop_ = true;
        }

        // create all particles
        CreateParticles();
    }

    // Set the particle system position and orientation
    void SetTransform(Matrix transform) { transform_ = transform; }

    // Set the total life of the particle system
    // (set to 0 to destroy it before end)
    void SetTotalTime(float totalTime) { totalTime_ = totalTime; }

    // Get/Set the current velocity scale factor (1.0 for no scaling)
    float getVelocityScale() const { return velocityScale_; }
    void setVelocityScale(float value) { velocityScale_ = value; }

    // Enable/Disable the particle system
    bool getEnabled() const { return enabled_; }
    void setEnabled(bool value) { enabled_ = value; }

    // Update particle systems
    bool Update(float elapsedTime) {
        // if enabled
        if (enabled_) {
            // add elapsed time for this frame
            elapsedTime_ += elapsedTime;

            // if partcile system is finished
            if (elapsedTime_ > totalTime_)
                // return false to delete object
                return false;
        }

        // return true to keep object alive
        return true;
    }

    // Add the particle system to the given vertex array
    int AddToVertArray(std::vector<VertexPositionNormalTexture>& vertexBufffer,
                       int vertexBufferPosition, int pointsLeft) {
        int count = 0;

        // for each particle
        for (const Particle& p : particles_) {
            // if still space in vertex array
            if (count >= pointsLeft)
                break;

            // set position
            vertexBufffer[vertexBufferPosition + count].Position = p.position;
            // set velocity in vertex normal
            vertexBufffer[vertexBufferPosition + count].Normal = p.velocity;
            // set random vaues in texture coordinates
            vertexBufffer[vertexBufferPosition + count].TextureCoordinate = p.random;

            count++;
        }

        // store number of particles in render vertex array
        renderCount_ = count;

        return count;
    }

    // Gets the number of particles to render
    // (if negative tells how many particles to skip when disabled)
    int getRenderCount() const { return (enabled_ ? renderCount_ : -renderCount_); }

    // Set the effect parameters for this particle system
    DrawMode SetEffect(EffectParameter* effectWorldViewProjection, EffectParameter* effectTexture,
                       EffectParameter* effectStartColor, EffectParameter* effectEndColor,
                       EffectParameter* effectTimes, EffectParameter* effectPointSize,
                       EffectParameter* effectVelocityScale, Matrix viewProjection) {
        // set world view projection matrix
        if (effectWorldViewProjection != nullptr) {
            effectWorldViewProjection->SetValue(transform_ * viewProjection);
        }

        // set texture
        if (effectTexture != nullptr) {
            effectTexture->SetValue(texture_);
        }

        // set start color
        if (effectStartColor != nullptr) {
            effectStartColor->SetValue(startColor_);
        }

        // set end color
        if (effectEndColor != nullptr) {
            effectEndColor->SetValue(endColor_);
        }

        // set elapsed time, particle time and total time
        if (effectTimes != nullptr) {
            effectTimes->SetValue(Vector3(elapsedTime_, particleTime_, totalTime_));
        }

        // set minimum and maximum point sizes
        if (effectPointSize != nullptr) {
            effectPointSize->SetValue(pointSize_);
        }

        // set velocity scale
        if (effectVelocityScale != nullptr) {
            effectVelocityScale->SetValue(velocityScale_);
        }

        // return true to enable additive blending (if false alpha blending is used)
        return drawMode_;
    }

private:
    // Create the particle systems
    void CreateParticles() {
        Vector3 randomVelocity;
        float lengthSquared;

        // for each particle
        for (int i = 0; i < count_; i++) {
            // get an equally distributed random direction
            do {
                randomVelocity = Vector3((float)random_.NextDouble() - 0.5f,
                                         (float)random_.NextDouble() - 0.5f,
                                         (float)random_.NextDouble() - 0.5f);
                lengthSquared = randomVelocity.LengthSquared();
                // if outside sphere get another sample
            } while (lengthSquared > 1.0f);

            // add to the Z direction for a cone like emmission
            randomVelocity.Z += emissionAngle_;

            // normalize direction
            randomVelocity = Vector3::Normalize(randomVelocity);

            // normalized random number
            float randomNumber = (float)random_.NextDouble();

            // velocity vector from range of min and max values
            randomVelocity *= velocity_.X * randomNumber + velocity_.Y * (1.0f - randomNumber);

            // random scale and time offset
            // (zero time offset to emitt all particles at same time)
            Vector2 randomVector((float)random_.NextDouble(),
                                 loop_ ? (float)random_.NextDouble() : 0);

            // add the particle to the list
            particles_.emplace_back(Vector3::Zero, randomVelocity, randomVector);
        }
    }

    ParticleSystemType particleType_{}; // particle system type

    bool enabled_ = false;  // is enabled?
                            // (when disabled will not update or render)
    DrawMode drawMode_{};   // drawing mode (alpha or additive and glow)

    bool loop_ = false; // is a loop? (if disabled will do a single burst)

    Texture2D* texture_ = nullptr; // texture map for the particles

    float elapsedTime_ = 0.0f;  // elapsed time since activated
    float totalTime_ = 0.0f;    // total time for particle system
    float particleTime_ = 0.0f; // particle life
                                // (if less than total time particles will loop)

    Vector4 startColor_; // start particle color and opacity
    Vector4 endColor_;   // end particle color and opacity

    Vector2 velocity_;  // start velocity range
                        // (random pick and end velocity is always zero)
    Vector2 pointSize_; // point size range (random pick)

    float emissionAngle_ = 0.0f; // emission cone
                                 // (0 for omni, >0 for more and more Z direction)
    float velocityScale_ = 1.0f; // scale factor for particle velocity

    int count_ = 0;       // total number of particle
    int renderCount_ = 0; // number of particles in render buffer

    Matrix transform_; // the particle system position and orientation

    // the list of particles
    std::vector<Particle> particles_;

    // random generator
    System::Random random_;
};

} // namespace ShipGame
