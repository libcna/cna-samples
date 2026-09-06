// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AnimSpriteManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AnimSpriteManager.hpp"

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "System/ArgumentNullException.hpp"

#include "../GameOptions.hpp"
#include "AnimSprite.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;

AnimSpriteManager::AnimSpriteManager() {
    vertices_.resize(GameOptions::MaxSprites * 6);
    vertexBuffer_.resize(GameOptions::MaxPlayers);
}

AnimSpriteManager::~AnimSpriteManager() { Dispose(true); }

bool AnimSpriteManager::Add(std::shared_ptr<AnimSprite> a) {
    if ((int)animatedSprites_.size() < GameOptions::MaxSprites) {
        animatedSprites_.push_back(std::move(a));
        return true;
    }
    return false;
}

void AnimSpriteManager::Update(float elapsedTime) {
    // empty deleted sprites list
    deleteSprites_.clear();

    // for each animated sprite
    for (auto node = animatedSprites_.begin(); node != animatedSprites_.end(); ++node) {
        // update animated sprite
        bool running = (*node)->Update(elapsedTime);

        // if finished running add to delete list
        if (running == false)
            deleteSprites_.push_back(node);
    }

    // delete all nodes in delete list
    for (const auto& s : deleteSprites_)
        animatedSprites_.erase(s);
}

void AnimSpriteManager::Draw(GraphicsDevice* gd, Vector3 cameraPos, Vector3 cameraUp,
                             Matrix viewProjection, int player, bool camera3rdPerson) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // if no sprites to render, return
    if (animatedSprites_.empty())
        return;

    // for each animated sprite
    int vertexBufferPosition = 0;
    for (const auto& s : animatedSprites_) {
        // add it to the vertex array
        s->AddToVertArray(vertices_, vertexBufferPosition, cameraPos, cameraUp, player,
                          camera3rdPerson);

        // update vertex buffer position
        vertexBufferPosition += 6;
    }

    // set the vertex buffer data
    vertexBuffer_[player]->SetData(vertices_.data(), 0, (int)animatedSprites_.size() * 6);

    // enable alpha blending and disable depth write
    gd->setBlendStateProperty(BlendState::AlphaBlend);
    gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);

    // select technique
    effect_->setCurrentTechniqueProperty(effectTechnique_);

    // set view projection matrix
    effectViewProjection_->SetValue(viewProjection);

    // set vertex buffer and declaration
    gd->SetVertexBuffer(&vertexBuffer_[player].value());

    // begin effect
    effect_->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

    // for each animated sprite
    vertexBufferPosition = 0;
    for (const auto& sprite : animatedSprites_) {
        // set effect parameters
        DrawMode mode = sprite->SetEffect(effectTexture_, effectFrameOffset_, effectFrameSize_,
                                          effectFrameBlend_);

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

        // draw the sprite quad
        gd->DrawPrimitives(PrimitiveType::TriangleList, vertexBufferPosition, 2);

        // update vertex buffer position
        vertexBufferPosition += 6;
    }

    // reset vertex buffer declaration
    gd->SetVertexBuffer(nullptr);

    // reset blend and depth write
    gd->setBlendStateProperty(BlendState::Additive);
    gd->setDepthStencilStateProperty(DepthStencilState::Default);
}

void AnimSpriteManager::LoadContent(GraphicsDevice* gd, ContentManager& content) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // load effect
    effect_ = content.Load<std::shared_ptr<Effect>>("shaders/AnimSprite");

    // get techinque
    effectTechnique_ = effect_->getTechniquesProperty()["AnimSprite"];

    // get parameters
    effectTexture_ = effect_->getParametersProperty()["Texture"];
    effectFrameOffset_ = effect_->getParametersProperty()["FrameOffset"];
    effectFrameSize_ = effect_->getParametersProperty()["FrameSize"];
    effectFrameBlend_ = effect_->getParametersProperty()["FrameBlend"];
    effectViewProjection_ = effect_->getParametersProperty()["ViewProj"];

    // create the vertex declaration
    vertexDeclaration_.emplace(
        VertexPositionTexture::getVertexDeclarationStatic().GetVertexElements());

    // create the vertex buffer
    vertexBuffer_[0].emplace(*gd, VertexPositionTexture::getVertexDeclarationStatic(),
                             GameOptions::MaxSprites * 6, BufferUsage::WriteOnly);
    vertexBuffer_[1].emplace(*gd, VertexPositionTexture::getVertexDeclarationStatic(),
                             GameOptions::MaxSprites * 6, BufferUsage::WriteOnly);
}

void AnimSpriteManager::UnloadContent() {
    // unload effect and parameters
    effectTexture_ = nullptr;
    effectFrameOffset_ = nullptr;
    effectFrameSize_ = nullptr;
    effectFrameBlend_ = nullptr;
    effectViewProjection_ = nullptr;
    effectTechnique_ = nullptr;
    effect_ = nullptr;
    // unload vertex buffer and declaration
    vertexBuffer_[0].reset();
    vertexBuffer_[1].reset();
    vertexDeclaration_.reset();
}

void AnimSpriteManager::Dispose() { Dispose(true); }

void AnimSpriteManager::Dispose(bool disposing) {
    if (disposing && !isDisposed_) {
        UnloadContent();
    }
}

} // namespace ShipGame
