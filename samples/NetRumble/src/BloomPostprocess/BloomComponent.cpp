// SPDX-License-Identifier: MS-PL
#include "BloomPostprocess/BloomComponent.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include <cmath>
#include <numbers>
#include <vector>
namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;
BloomComponent::BloomComponent(Game &g) : DrawableGameComponent(g) {
  content_ = std::make_unique<ContentManager>(&g.getServicesProperty(),
                                              "Content/BloomPostprocess");
}
const BloomSettings &BloomComponent::getSettingsProperty() const {
  return settings_;
}
void BloomComponent::setSettingsProperty(const BloomSettings &v) {
  settings_ = v;
}
BloomComponent::IntermediateBuffer
BloomComponent::getShowBufferProperty() const {
  return showBuffer_;
}
void BloomComponent::setShowBufferProperty(IntermediateBuffer v) {
  showBuffer_ = v;
}
const std::string &BloomComponent::GetTypeName() const {
  static const std::string n = "NetRumble.BloomComponent";
  return n;
}
void BloomComponent::LoadContent() {
  spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
  bloomExtractEffect_ =
      content_->Load<std::shared_ptr<Effect>>("Effects/BloomExtract");
  bloomCombineEffect_ =
      content_->Load<std::shared_ptr<Effect>>("Effects/BloomCombine");
  gaussianBlurEffect_ =
      content_->Load<std::shared_ptr<Effect>>("Effects/GaussianBlur");
  const auto &p =
      getGraphicsDeviceProperty().getPresentationParametersProperty();
  int w = p.getBackBufferWidthProperty(), h = p.getBackBufferHeightProperty();
  auto f = p.getBackBufferFormatProperty();
  sceneRenderTarget_ = std::make_unique<RenderTarget2D>(
      getGraphicsDeviceProperty(), w, h, false, f,
      p.getDepthStencilFormatProperty(), p.getMultiSampleCountProperty(),
      RenderTargetUsage::DiscardContents);
  w /= 2;
  h /= 2;
  renderTarget1_ = std::make_unique<RenderTarget2D>(
      getGraphicsDeviceProperty(), w, h, false, f, DepthFormat::None);
  renderTarget2_ = std::make_unique<RenderTarget2D>(
      getGraphicsDeviceProperty(), w, h, false, f, DepthFormat::None);
}
void BloomComponent::UnloadContent() {
  sceneRenderTarget_->Dispose();
  renderTarget1_->Dispose();
  renderTarget2_->Dispose();
  content_->Unload();
}
void BloomComponent::BeginDraw() {
  getGraphicsDeviceProperty().SetRenderTarget(sceneRenderTarget_.get());
}
void BloomComponent::Draw(const GameTime &) {
  getGraphicsDeviceProperty().getSamplerStatesProperty()[1] =
      SamplerState::LinearClamp;
  bloomExtractEffect_->getParametersProperty()["BloomThreshold"]->SetValue(
      settings_.BloomThreshold);
  DrawFullscreenQuad(*sceneRenderTarget_, *renderTarget1_,
                     bloomExtractEffect_.get(), IntermediateBuffer::PreBloom);
  SetBlurEffectParameters(1.f / renderTarget1_->getWidthProperty(), 0);
  DrawFullscreenQuad(*renderTarget1_, *renderTarget2_,
                     gaussianBlurEffect_.get(),
                     IntermediateBuffer::BlurredHorizontally);
  SetBlurEffectParameters(0, 1.f / renderTarget1_->getHeightProperty());
  DrawFullscreenQuad(*renderTarget2_, *renderTarget1_,
                     gaussianBlurEffect_.get(),
                     IntermediateBuffer::BlurredBothWays);
  getGraphicsDeviceProperty().SetRenderTarget(nullptr);
  auto &p = bloomCombineEffect_->getParametersProperty();
  p["BloomIntensity"]->SetValue(settings_.BloomIntensity);
  p["BaseIntensity"]->SetValue(settings_.BaseIntensity);
  p["BloomSaturation"]->SetValue(settings_.BloomSaturation);
  p["BaseSaturation"]->SetValue(settings_.BaseSaturation);
  getGraphicsDeviceProperty().getTexturesProperty()(1,
                                                    sceneRenderTarget_.get());
  auto v = getGraphicsDeviceProperty().getViewportProperty();
  DrawFullscreenQuad(*renderTarget1_, v.getWidthProperty(),
                     v.getHeightProperty(), bloomCombineEffect_.get(),
                     IntermediateBuffer::FinalResult);
}
void BloomComponent::DrawFullscreenQuad(Texture2D &t, RenderTarget2D &r,
                                        Effect *e, IntermediateBuffer b) {
  getGraphicsDeviceProperty().SetRenderTarget(&r);
  DrawFullscreenQuad(t, r.getWidthProperty(), r.getHeightProperty(), e, b);
}
void BloomComponent::DrawFullscreenQuad(Texture2D &t, int w, int h, Effect *e,
                                        IntermediateBuffer b) {
  if (showBuffer_ < b)
    e = nullptr;
  spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::Opaque, nullptr,
                      nullptr, nullptr, e);
  spriteBatch_->Draw(t, Rectangle(0, 0, w, h), Color::White);
  spriteBatch_->End();
}
void BloomComponent::SetBlurEffectParameters(float dx, float dy) {
  auto *wp = gaussianBlurEffect_->getParametersProperty()["SampleWeights"];
  auto *op = gaussianBlurEffect_->getParametersProperty()["SampleOffsets"];
  int c = wp->getElementsProperty().getCountProperty();
  std::vector<float> w(c);
  std::vector<Vector2> o(c);
  w[0] = ComputeGaussian(0);
  o[0] = Vector2::Zero;
  float total = w[0];
  for (int i = 0; i < c / 2; ++i) {
    float x = ComputeGaussian(float(i + 1));
    w[i * 2 + 1] = w[i * 2 + 2] = x;
    total += 2 * x;
    Vector2 d = Vector2(dx, dy) * (float(i) * 2 + 1.5f);
    o[i * 2 + 1] = d;
    o[i * 2 + 2] = -d;
  }
  for (float &x : w)
    x /= total;
  wp->SetValue(w);
  op->SetValue(o);
}
float BloomComponent::ComputeGaussian(float n) const {
  float t = settings_.BlurAmount;
  return float((1. / std::sqrt(2 * std::numbers::pi_v<double> * t)) *
               std::exp(-(n * n) / (2 * t * t)));
}
} // namespace NetRumble
