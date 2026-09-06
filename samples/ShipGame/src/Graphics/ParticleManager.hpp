// SPDX-License-Identifier: MS-PL
#pragma once

// ParticleManager.hpp — C++ port of ShipGame/Graphics/ParticleManager.cs (XNA
// 4.0 Ship Game Starter Kit). Owns every live particle system and packs them all
// into one shared vertex buffer drawn through Particle.fx.

#include <list>
#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/IDisposable.hpp"

#include "../GameOptions.hpp"
#include "ParticleSystem.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::EffectTechnique;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::VertexDeclaration;
using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;

// Port of ShipGame/Graphics/ParticleManager.cs.
class ParticleManager : public System::IDisposable {
public:
    // Create a new particle system manager
    ParticleManager() { vertices_.resize(GameOptions::MaxParticles); }

    ~ParticleManager() override { Dispose(true); }

    // Add a new particle system
    void Add(const std::shared_ptr<ParticleSystem>& ps) {
        // add to list of active particle systems
        systems_.push_back(ps);

        // set particles to vertex array
        int count = ps->AddToVertArray(vertices_, vertexCount_,
                                       GameOptions::MaxParticles - vertexCount_);

        // if any particles created
        if (count > 0) {
            // set new particles to vertex buffer
            vertexBuffer_->SetData(vertices_.data(), GameOptions::MaxParticles);

            // add the number particles created
            // (one vertex per particle as we are using point sprites)
            vertexCount_ += count;
        }
    }

    // Update all particle systems
    void Update(float elapsedTime) {
        // empty deleted particle systems list
        deleteSystems_.clear();

        // for each particle system
        for (auto node = systems_.begin(); node != systems_.end(); ++node) {
            // update animated sprite
            bool running = (*node)->Update(elapsedTime);

            // if finished running add to delete list
            if (running == false)
                deleteSystems_.push_back(node);
        }

        // delete all nodes in delete list
        for (const auto& s : deleteSystems_)
            systems_.erase(s);

        // if any particle systems deleted
        if (!deleteSystems_.empty()) {
            // re-cretae vertex array
            vertexCount_ = 0;
            for (const auto& ps : systems_)
                vertexCount_ += ps->AddToVertArray(vertices_, vertexCount_,
                                                   GameOptions::MaxParticles - vertexCount_);

            // set vertex buffer
            if (vertexCount_ > 0)
                vertexBuffer_->SetData(vertices_.data(), 0, vertexCount_);
        }
    }

    // Draw all particle systems
    void Draw(GraphicsDevice* gd, Matrix viewProjection) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }

        // if no particle systems or no vertices, return
        if (systems_.empty() || vertexCount_ == 0)
            return;

        // enable alpha blending and disable depth write
        gd->setBlendStateProperty(BlendState::AlphaBlend);
        gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);

        // enable point sprite
        // gd.RenderState.PointSpriteEnable = true;
        // gd.RenderState.PointSizeMax = 128;

        // select technique
        effect_->setCurrentTechniqueProperty(effectTechnique_);

        // set vertex buffer and declaration
        gd->SetVertexBuffer(&vertexBuffer_.value());

        // for each particle system
        int vertexPosition = 0;
        for (const auto& ps : systems_) {
            // set effect parameters
            DrawMode mode =
                ps->SetEffect(effectWorldViewProjection_, effectTexture_, effectStartColor_,
                              effectEndColor_, effectTimes_, effectPointSize_,
                              effectVelocityScale_, viewProjection);

            // if additive blend
            if (((int)mode & 1) != 0)
                // set additive blend
                gd->setBlendStateProperty(BlendState::Additive);
            else
                // set alpha blend
                gd->setBlendStateProperty(BlendState::AlphaBlend);

            // if glow enabled
            // if (((int)mode & 2) != 0)
            //     gd.RenderState.AlphaSourceBlend = Blend.One;
            // else
            //     gd.RenderState.AlphaSourceBlend = Blend.Zero;

            // get number of particles in this particle system
            int numberVertices = ps->getRenderCount();
            if (numberVertices > 0) {
                // draw the point sprites
                // gd.DrawPrimitives(PrimitiveType.PointList, vertexPosition,
                //     numberVertices);

                // apply effect pass
                effect_->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

                gd->DrawPrimitives(PrimitiveType::LineList, vertexPosition, numberVertices);

                // update vertex buffer position
                vertexPosition += numberVertices;
            } else
                // if negative count, particle system is disabled
                if (numberVertices < 0)
                // skip all vertices
                vertexPosition += -numberVertices;
        }

        // reset vertex buffer and declaration
        gd->SetVertexBuffer(nullptr);

        // reset blend and depth write
        gd->setBlendStateProperty(BlendState::Opaque);
        gd->setDepthStencilStateProperty(DepthStencilState::Default);

        // disable point sprite
        // gd.RenderState.PointSpriteEnable = false;
    }

    // Load content
    void LoadContent(GraphicsDevice* gd, ContentManager& content) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }

        // load effect
        effect_ = content.Load<std::shared_ptr<Effect>>("shaders/Particle");

        // get techinque
        effectTechnique_ = effect_->getTechniquesProperty()["Particle"];

        // get parameters
        effectWorldViewProjection_ = effect_->getParametersProperty()["WorldViewProj"];
        effectTexture_ = effect_->getParametersProperty()["Texture"];
        effectStartColor_ = effect_->getParametersProperty()["StartColor"];
        effectEndColor_ = effect_->getParametersProperty()["EndColor"];
        effectTimes_ = effect_->getParametersProperty()["Times"];
        effectPointSize_ = effect_->getParametersProperty()["PointSize"];
        effectVelocityScale_ = effect_->getParametersProperty()["VelocityScale"];

        // create the vertex buffer
        vertexBuffer_.emplace(*gd, VertexPositionNormalTexture::getVertexDeclarationStatic(),
                              GameOptions::MaxParticles, BufferUsage::WriteOnly);

        // create the vertex declaration
        vertexDeclaration_.emplace(
            VertexPositionNormalTexture::getVertexDeclarationStatic().GetVertexElements());

        // if any particles in vertex array set them to vertex buffer
        if (vertexCount_ > 0)
            vertexBuffer_->SetData(vertices_.data(), 0, vertexCount_);
    }

    // Unload content
    void UnloadContent() {
        // unload effect and parameters
        effectWorldViewProjection_ = nullptr;
        effectTexture_ = nullptr;
        effectStartColor_ = nullptr;
        effectEndColor_ = nullptr;
        effectTimes_ = nullptr;
        effectPointSize_ = nullptr;
        effectVelocityScale_ = nullptr;
        effectTechnique_ = nullptr;
        effect_ = nullptr;
        vertexBuffer_.reset();
        vertexDeclaration_.reset();
    }

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override { Dispose(true); }

private:
    void Dispose(bool disposing) {
        if (disposing && !isDisposed_) {
            UnloadContent();
        }
    }

    bool isDisposed_ = false;

    int vertexCount_ = 0; // number of vertices in vertex buffer

    std::optional<VertexBuffer> vertexBuffer_;           // the vertex buffer
    std::optional<VertexDeclaration> vertexDeclaration_; // the vertex declaration

    std::shared_ptr<Effect> effect_;                          // effect
    EffectTechnique* effectTechnique_ = nullptr;              // effect technique
    EffectParameter* effectWorldViewProjection_ = nullptr;    // effect world view proj parameter
    EffectParameter* effectTexture_ = nullptr;                // effect texture parameter
    EffectParameter* effectStartColor_ = nullptr;             // effect start color parameter
    EffectParameter* effectEndColor_ = nullptr;               // effect end color parameter
    EffectParameter* effectTimes_ = nullptr;                  // effect times parameter
    EffectParameter* effectPointSize_ = nullptr;              // effect point size parameter
    EffectParameter* effectVelocityScale_ = nullptr;          // effect velocity scale parameter

    // the vertices array
    std::vector<VertexPositionNormalTexture> vertices_;

    // linked list of active particle systems
    std::list<std::shared_ptr<ParticleSystem>> systems_;

    // linked list of nodes to delete from the particle systems list
    std::vector<std::list<std::shared_ptr<ParticleSystem>>::iterator> deleteSystems_;
};

} // namespace ShipGame
