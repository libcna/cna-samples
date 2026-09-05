// SPDX-License-Identifier: MS-PL
#include "Rendering/Starfield.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "RandomMath.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;

const std::array<Color, Starfield::numberOfLayers_> Starfield::layerColors_ = {
    Color(255, 255, 255, 255), Color(255, 255, 255, 216),
    Color(255, 255, 255, 192), Color(255, 255, 255, 160),
    Color(255, 255, 255, 128), Color(255, 255, 255, 96),
    Color(255, 255, 255, 64),  Color(255, 255, 255, 32),
};

const std::array<float, Starfield::numberOfLayers_>
    Starfield::movementFactors_ = {
        0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f,
};

const Color Starfield::backgroundColor_(0, 0, 32);

Starfield::Starfield(Vector2 position, GraphicsDevice &graphicsDevice,
                     ContentManager &contentManager)
    : graphicsDevice_(&graphicsDevice), contentManager_(&contentManager) {
  Reset(position);
}

Starfield::~Starfield() { Dispose(false); }

void Starfield::LoadContent() {
  cloudTexture_.emplace(contentManager_->Load<Texture2D>("Textures/clouds"));
  cloudEffect_ =
      contentManager_->Load<std::shared_ptr<Effect>>("Effects/Clouds");
  cloudEffectPosition_ = cloudEffect_->getParametersProperty()["Position"];

  starTexture_.emplace(*graphicsDevice_, 1, 1, false, SurfaceFormat::Color);
  const Color white = Color::White;
  starTexture_->SetData(&white, 1);
  spriteBatch_ = std::make_unique<SpriteBatch>(*graphicsDevice_);
}

void Starfield::UnloadContent() {
  cloudTexture_.reset();
  cloudEffect_.reset();
  cloudEffectPosition_ = nullptr;

  if (starTexture_.has_value()) {
    starTexture_->Dispose();
    starTexture_.reset();
  }
  if (spriteBatch_ != nullptr) {
    spriteBatch_->Dispose();
    spriteBatch_.reset();
  }
}

void Starfield::Reset(Vector2 position) {
  const int viewportWidth =
      graphicsDevice_->getViewportProperty().getWidthProperty();
  const int viewportHeight =
      graphicsDevice_->getViewportProperty().getHeightProperty();
  for (Vector2 &star : stars_) {
    star = Vector2(static_cast<float>(
                       RandomMath::getRandomProperty().Next(0, viewportWidth)),
                   static_cast<float>(RandomMath::getRandomProperty().Next(
                       0, viewportHeight)));
  }
  lastPosition_ = position_ = position;
}

void Starfield::Draw(Vector2 position) {
  lastPosition_ = position_;
  position_ = position;
  const Vector2 movement = -1.0f * (position - lastPosition_);

  const Rectangle starfieldRectangle(
      0, 0, graphicsDevice_->getViewportProperty().getWidthProperty(),
      graphicsDevice_->getViewportProperty().getHeightProperty());

  spriteBatch_->Begin();
  spriteBatch_->Draw(*starTexture_, starfieldRectangle, backgroundColor_);
  spriteBatch_->End();

  cloudEffectPosition_->SetValue(position_);
  spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::NonPremultiplied,
                      nullptr, nullptr, nullptr, cloudEffect_.get());
  spriteBatch_->Draw(*cloudTexture_, starfieldRectangle, std::nullopt,
                     Color::White, 0.0f, Vector2::Zero, SpriteEffects::None,
                     1.0f);
  spriteBatch_->End();

  if (movement.Length() > maximumMovementPerUpdate_) {
    Reset(position);
    return;
  }

  spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::NonPremultiplied);
  for (int i = 0; i < numberOfStars_; ++i) {
    const int depth = i % movementFactors_.size();
    stars_[i] += movement * movementFactors_[depth];

    if (stars_[i].X < starfieldRectangle.X) {
      stars_[i].X =
          static_cast<float>(starfieldRectangle.X + starfieldRectangle.Width);
      stars_[i].Y = static_cast<float>(
          starfieldRectangle.Y +
          RandomMath::getRandomProperty().Next(starfieldRectangle.Height));
    }
    if (stars_[i].X > starfieldRectangle.X + starfieldRectangle.Width) {
      stars_[i].X = static_cast<float>(starfieldRectangle.X);
      stars_[i].Y = static_cast<float>(
          starfieldRectangle.Y +
          RandomMath::getRandomProperty().Next(starfieldRectangle.Height));
    }
    if (stars_[i].Y < starfieldRectangle.Y) {
      stars_[i].X = static_cast<float>(
          starfieldRectangle.X +
          RandomMath::getRandomProperty().Next(starfieldRectangle.Width));
      stars_[i].Y =
          static_cast<float>(starfieldRectangle.Y + starfieldRectangle.Height);
    }
    if (stars_[i].Y >
        starfieldRectangle.Y +
            graphicsDevice_->getViewportProperty().getHeightProperty()) {
      stars_[i].X = static_cast<float>(
          starfieldRectangle.X +
          RandomMath::getRandomProperty().Next(starfieldRectangle.Width));
      stars_[i].Y = static_cast<float>(starfieldRectangle.Y);
    }

    spriteBatch_->Draw(*starTexture_,
                       Rectangle(static_cast<int>(stars_[i].X),
                                 static_cast<int>(stars_[i].Y), starSize_,
                                 starSize_),
                       std::nullopt, layerColors_[depth]);
  }
  spriteBatch_->End();
}

void Starfield::Dispose() { Dispose(true); }

void Starfield::Dispose(bool disposing) {
  if (!disposed_ && disposing) {
    UnloadContent();
  }
  disposed_ = true;
}
} // namespace NetRumble
