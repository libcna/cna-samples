// SPDX-License-Identifier: MS-PL
#pragma once

// AnimSprite.hpp — C++ port of ShipGame/Graphics/AnimSprite.cs (XNA 4.0 Ship
// Game Starter Kit). One camera-facing animated quad: an explosion, a spawn
// flash or a shield, played out of a grid texture.

#include <algorithm>
#include <array>
#include <vector>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/ArgumentOutOfRangeException.hpp"

#include "../GameManager.hpp"
#include "AnimSpriteManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::VertexPositionTexture;

// Port of ShipGame/Graphics/AnimSprite.cs.
class AnimSprite {
public:
    // The animated sprite position
    Vector3 getPosition() const { return position_; }
    void setPosition(Vector3 value) { position_ = value; }

    // The animated sprite radius
    float getRadius() const { return radius_; }
    void setRadius(float value) { radius_ = value; }

    // Create a new animated sprite
    AnimSprite(AnimSpriteType type, Vector3 position, float radius, float viewOffset,
               Texture2D* texture, int frameSizeX, int frameSizeY, float frameRate, DrawMode mode,
               int player) {
        if (texture == nullptr) {
            throw System::ArgumentNullException("texture");
        }

        spriteType_ = type;
        position_ = position;
        radius_ = radius;
        viewOffset_ = viewOffset;
        texture_ = texture;
        player_ = player;
        frameRate_ = frameRate;
        drawMode_ = mode;

        // frame size
        float sizeX = (float)frameSizeX / (float)texture->getWidthProperty();
        float sizeY = (float)frameSizeY / (float)texture->getHeightProperty();
        frameSize_ = Vector2(sizeX, sizeY);

        // number of frames
        numberFramesX_ = texture->getWidthProperty() / frameSizeX;
        numberFramesY_ = texture->getHeightProperty() / frameSizeY;
        numberFrames_ = numberFramesX_ * numberFramesY_;

        // total animation time
        totalTime_ = (float)numberFrames_ / frameRate;
        elapsedTime_ = 0;
    }

    // Updates the animated sprite for given elapsed time and
    // return false when animation is finished and object can be released
    bool Update(float elapsedTime) {
        // add frame elapsed time
        elapsedTime_ += elapsedTime;

        // if total time reached, return false to destroy object
        if (elapsedTime_ > totalTime_)
            return false;

        // return true to keep object alive
        return true;
    }

    // Set the animation total time
    // (set to zero to delete object before it is finished)
    void SetTotalTime(float totalTime) { totalTime_ = totalTime; }

    // Add the animated sprite geometry to the given vertex array
    // aligning it to the given camera
    void AddToVertArray(std::vector<VertexPositionTexture>& vertexBuffer, int vertexBufferPosition,
                        Vector3 cameraPosition, Vector3 cameraUp, int player,
                        bool camera3rdPerson) {
        int maximumVertexBufferPosition = vertexBufferPosition + 5;
        if ((maximumVertexBufferPosition < vertexBufferPosition) ||        // overflow
            (maximumVertexBufferPosition >= (int)vertexBuffer.size())) {   // too many
            throw System::ArgumentOutOfRangeException("vertexBufferPosition");
        }

        // view direction
        Vector3 viewDirection = position_ - cameraPosition;

        // distance from camera to animated sprite
        float viewDistance = viewDirection.Length();

        // normalize view direction
        viewDirection *= 1.0f / viewDistance;

        // animated sprite X axis is the right vector
        Vector3 right = Vector3::Normalize(Vector3::Cross(cameraUp, viewDirection));
        // animated sprite Y axis is the up vector
        Vector3 up = Vector3::Normalize(Vector3::Cross(viewDirection, right));

        // view direction offset used to show the animated sprits in front
        // of other objects always moving in the view diretion
        Vector3 offset = Vector3::Zero;
        if (camera3rdPerson == true || player != player_) {
            if (viewOffset_ < viewDistance * 0.5f)
                offset = -viewDirection * viewOffset_;
            else
                offset = -viewDirection * (viewDistance * 0.5f);
        }

        // setup quad vertices
        vertexBuffer[vertexBufferPosition].Position = position_ + offset + radius_ * (right + up);
        vertexBuffer[vertexBufferPosition].TextureCoordinate = QuadTexCoords()[0];
        vertexBuffer[vertexBufferPosition + 1].Position =
            position_ + offset + radius_ * (-right + up);
        vertexBuffer[vertexBufferPosition + 1].TextureCoordinate = QuadTexCoords()[1];
        vertexBuffer[vertexBufferPosition + 2].Position =
            position_ + offset + radius_ * (-right - up);
        vertexBuffer[vertexBufferPosition + 2].TextureCoordinate = QuadTexCoords()[2];
        vertexBuffer[vertexBufferPosition + 3].Position =
            position_ + offset + radius_ * (right - up);
        vertexBuffer[vertexBufferPosition + 3].TextureCoordinate = QuadTexCoords()[3];
        vertexBuffer[vertexBufferPosition + 4] = vertexBuffer[vertexBufferPosition];
        vertexBuffer[vertexBufferPosition + 5] = vertexBuffer[vertexBufferPosition + 2];
    }

    // Set the effect parameters for this animated sprite
    DrawMode SetEffect(EffectParameter* effectTexture, EffectParameter* effectFrameOffset,
                       EffectParameter* effectFrameSize, EffectParameter* effectFrameBlend) {
        // set texture
        if (effectTexture != nullptr) {
            effectTexture->SetValue(texture_);
        }

        // calculate opacity based on squared normalized life time
        float opacity = std::min(1.0f, elapsedTime_ / totalTime_);
        opacity = 1.0f - opacity * opacity;

        // calculate the float frame position used for frame blending
        float floatFrame = elapsedTime_ * frameRate_;

        // get the two frames to blend
        int frame = std::min(numberFrames_ - 1, (int)floatFrame);
        int nextFrame = std::min(numberFrames_ - 1, (frame + 1));

        // set frame size
        if (effectFrameSize != nullptr) {
            effectFrameSize->SetValue(frameSize_);
        }

        // set frame offset
        Vector4 frameOffset((float)(frame % numberFramesX_), (float)(frame / numberFramesX_),
                            (float)(nextFrame % numberFramesX_),
                            (float)(nextFrame / numberFramesX_));
        if (effectFrameOffset != nullptr) {
            effectFrameOffset->SetValue(frameOffset);
        }

        // set blend factor
        float blendFactor = floatFrame - (float)frame;
        if (effectFrameBlend != nullptr) {
            effectFrameBlend->SetValue(Vector2(blendFactor, 2 * opacity));
        }

        // return true to enable additive blending (if false alpha blending is used)
        return drawMode_;
    }

private:
    // base quad texture coordinates
    static const std::array<Vector2, 4>& QuadTexCoords() {
        static const std::array<Vector2, 4> value = {Vector2(0, 0), Vector2(1, 0), Vector2(1, 1),
                                                      Vector2(0, 1)};
        return value;
    }

    AnimSpriteType spriteType_{}; // animated sprite type

    Vector3 position_;         // the sprite position
    float radius_ = 0.0f;      // the sprite radius
    float viewOffset_ = 0.0f;  // view offset moves sprite in direction of camera
    int player_ = -1;          // the player it is related to (-1 for no player)

    Texture2D* texture_ = nullptr; // the texture grid with the animation frames
    Vector2 frameSize_;            // frame size in X and Y directions
    DrawMode drawMode_{};          // drawing mode (alpha or additive and glow)

    float frameRate_ = 0.0f;   // framerate to play animation frames
    float elapsedTime_ = 0.0f; // elapsed time since animation start
    float totalTime_ = 0.0f;   // total animation time

    int numberFrames_ = 0;  // number of frames in texture grid
    int numberFramesX_ = 0; // number of frames per row in the texture grid
    int numberFramesY_ = 0; // number of frames per column in the texture grid
};

} // namespace ShipGame
