// SPDX-License-Identifier: MS-PL
#pragma once
#include "BloomPostprocess/BloomSettings.hpp"
#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include <memory>
#include <string>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace NetRumble {
/** @brief Applies the original four-pass bloom postprocess. */
class BloomComponent final
    : public Microsoft::Xna::Framework::DrawableGameComponent {
public:
  /** @brief Selectable intermediate output. */ enum class IntermediateBuffer {
    PreBloom,
    BlurredHorizontally,
    BlurredBothWays,
    FinalResult
  };
  /** @brief Creates the component. @param game Owning game. */
  explicit BloomComponent(Microsoft::Xna::Framework::Game &game);
  /** @brief Gets settings. @return Current settings. */
  [[nodiscard]] const BloomSettings &getSettingsProperty() const;
  /** @brief Sets settings. @param value New settings. */ void
  setSettingsProperty(const BloomSettings &value);
  /** @brief Gets visible intermediate output. @return Selected output. */
  [[nodiscard]] IntermediateBuffer getShowBufferProperty() const;
  /** @brief Selects visible intermediate output. @param value Output. */ void
  setShowBufferProperty(IntermediateBuffer value);
  /** @brief Redirects scene drawing into the bloom source target. */ void
  BeginDraw();
  /** @brief Applies bloom to the captured scene. @param gameTime Timing
   * snapshot. */
  void
  Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;
  /** @brief Returns logical runtime type name. @return Type name. */ CNAEXT
      [[nodiscard]] const std::string &
      GetTypeName() const override;

protected:
  /** @brief Loads effects and render targets. */ void LoadContent() override;
  /** @brief Releases render targets. */ void UnloadContent() override;

private:
  void DrawFullscreenQuad(Microsoft::Xna::Framework::Graphics::Texture2D &,
                          Microsoft::Xna::Framework::Graphics::RenderTarget2D &,
                          Microsoft::Xna::Framework::Graphics::Effect *,
                          IntermediateBuffer);
  void DrawFullscreenQuad(Microsoft::Xna::Framework::Graphics::Texture2D &, int,
                          int, Microsoft::Xna::Framework::Graphics::Effect *,
                          IntermediateBuffer);
  void SetBlurEffectParameters(float, float);
  [[nodiscard]] float ComputeGaussian(float) const;
  std::unique_ptr<Microsoft::Xna::Framework::Content::ContentManager> content_;
  std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch>
      spriteBatch_;
  std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect>
      bloomExtractEffect_, bloomCombineEffect_, gaussianBlurEffect_;
  std::unique_ptr<Microsoft::Xna::Framework::Graphics::RenderTarget2D>
      sceneRenderTarget_, renderTarget1_, renderTarget2_;
  BloomSettings settings_ = BloomSettings::PresetSettings[0];
  IntermediateBuffer showBuffer_ = IntermediateBuffer::FinalResult;
};
} // namespace NetRumble
