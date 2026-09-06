// SPDX-License-Identifier: MS-PL
#pragma once

// AnimSpriteManager.hpp — C++ port of ShipGame/Graphics/AnimSpriteManager.cs
// (XNA 4.0 Ship Game Starter Kit). Owns every live animated sprite, batches them
// into one vertex buffer per player and draws them through AnimSprite.fx.
//
// The class body is in AnimSpriteManager.cpp: it needs AnimSprite complete, and
// AnimSprite needs this file's DrawMode.

#include <array>
#include <list>
#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "System/IDisposable.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::EffectTechnique;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::VertexDeclaration;
using Microsoft::Xna::Framework::Graphics::VertexPositionTexture;

// Port of the DrawMode enum in ShipGame/Graphics/AnimSpriteManager.cs. Bit 0 selects additive
// blending and bit 1 the glow buffer, which is why the values are spelled out and tested as bits.
enum class DrawMode {
    Alpha = 0,
    Additive = 1,
    AlphaAndGlow = 2,
    AdditiveAndGlow = 3,
};

class AnimSprite;

// Port of ShipGame/Graphics/AnimSpriteManager.cs.
class AnimSpriteManager : public System::IDisposable {
public:
    // Create a new animated sprite manager
    AnimSpriteManager();
    ~AnimSpriteManager() override;

    // Add a new animated sprite
    bool Add(std::shared_ptr<AnimSprite> a);

    // Update all animated sprites
    void Update(float elapsedTime);

    // Draw all sprites aligning to given camera
    void Draw(GraphicsDevice* gd, Vector3 cameraPos, Vector3 cameraUp, Matrix viewProjection,
              int player, bool camera3rdPerson);

    // Load content
    void LoadContent(GraphicsDevice* gd, ContentManager& content);

    // Unload content
    void UnloadContent();

    // ---- IDisposable Members ----
    bool IsDisposed() const { return isDisposed_; }
    void Dispose() override;

private:
    void Dispose(bool disposing);

    bool isDisposed_ = false;

    // vertex buffer for each player with all running animated sprites
    std::vector<std::optional<VertexBuffer>> vertexBuffer_;
    std::optional<VertexDeclaration> vertexDeclaration_; // vertex delcaration

    std::shared_ptr<Effect> effect_;                // the effect
    EffectTechnique* effectTechnique_ = nullptr;    // effect technique
    EffectParameter* effectTexture_ = nullptr;      // effect texture parameter
    EffectParameter* effectFrameOffset_ = nullptr;  // effect frame offset parameter
    EffectParameter* effectFrameSize_ = nullptr;    // effect frame size parameter
    EffectParameter* effectFrameBlend_ = nullptr;   // effect frame blend parameter
    EffectParameter* effectViewProjection_ = nullptr; // effect view projection parameter

    // the vertex array for all running animated sprites
    std::vector<VertexPositionTexture> vertices_;

    // linked list of active animated sprites
    std::list<std::shared_ptr<AnimSprite>> animatedSprites_;

    // linked list of nodes to delete from the animated sprites list
    std::vector<std::list<std::shared_ptr<AnimSprite>>::iterator> deleteSprites_;
};

} // namespace ShipGame
