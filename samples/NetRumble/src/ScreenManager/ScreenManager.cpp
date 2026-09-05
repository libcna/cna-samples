// SPDX-License-Identifier: MS-PL
#include "ScreenManager/ScreenManager.hpp"

#include <algorithm>
#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "System/Diagnostics/Debug.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;

ScreenManager::ScreenManager(Game &game) : DrawableGameComponent(game) {
  content_ =
      std::make_unique<ContentManager>(&game.getServicesProperty(), "Content");
}

Game &ScreenManager::getGamePublicProperty() { return getGameProperty(); }
GraphicsDevice &ScreenManager::getGraphicsDevicePublicProperty() {
  return getGraphicsDeviceProperty();
}
ContentManager &ScreenManager::getContentProperty() { return *content_; }
SpriteBatch &ScreenManager::getSpriteBatchProperty() { return *spriteBatch_; }
SpriteFont &ScreenManager::getFontProperty() { return *font_; }
bool ScreenManager::getTraceEnabledProperty() const { return traceEnabled_; }
void ScreenManager::setTraceEnabledProperty(bool value) {
  traceEnabled_ = value;
}
Rectangle ScreenManager::getTitleSafeAreaProperty() const {
  return titleSafeArea_;
}

void ScreenManager::LoadContent() {
  spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
  font_.emplace(content_->Load<SpriteFont>("Fonts/MenuFont"));
  blankTexture_.emplace(content_->Load<Texture2D>("Textures/blank"));
  contentLoaded_ = true;
  for (const auto &screen : screens_)
    screen->LoadContent();
  const Viewport viewport = getGraphicsDeviceProperty().getViewportProperty();
  titleSafeArea_ = Rectangle(
      static_cast<int>(std::floor(viewport.getXProperty() +
                                  viewport.getWidthProperty() * 0.05f)),
      static_cast<int>(std::floor(viewport.getYProperty() +
                                  viewport.getHeightProperty() * 0.05f)),
      static_cast<int>(std::floor(viewport.getWidthProperty() * 0.9f)),
      static_cast<int>(std::floor(viewport.getHeightProperty() * 0.9f)));
}

void ScreenManager::UnloadContent() {
  content_->Unload();
  for (const auto &screen : screens_)
    screen->UnloadContent();
  contentLoaded_ = false;
}

void ScreenManager::Update(GameTime &gameTime) {
  input_.Update();
  screensToUpdate_ = screens_;
  bool otherScreenHasFocus = !getGameProperty().getIsActiveProperty();
  bool coveredByOtherScreen = false;
  while (!screensToUpdate_.empty()) {
    const auto screen = screensToUpdate_.back();
    screensToUpdate_.pop_back();
    screen->Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    if (screen->getScreenStateProperty() == ScreenState::TransitionOn ||
        screen->getScreenStateProperty() == ScreenState::Active) {
      if (!otherScreenHasFocus) {
        screen->HandleInput(input_);
        screen->UpdatePresence();
        otherScreenHasFocus = true;
      }
      if (!screen->getIsPopupProperty())
        coveredByOtherScreen = true;
    }
  }
  if (traceEnabled_)
    TraceScreens();
}

void ScreenManager::Draw(const GameTime &gameTime) {
  const auto screensToDraw = screens_;
  for (const auto &screen : screensToDraw)
    if (screen->getScreenStateProperty() != ScreenState::Hidden)
      screen->Draw(gameTime);
}

void ScreenManager::DrawRectangle(const Rectangle &rectangle,
                                  const Color &color) {
  spriteBatch_->Begin();
  spriteBatch_->Draw(*blankTexture_, rectangle, color);
  spriteBatch_->End();
}

void ScreenManager::AddScreen(std::shared_ptr<GameScreen> screen) {
  screen->setScreenManagerProperty(*this);
  if (contentLoaded_)
    screen->LoadContent();
  screens_.push_back(std::move(screen));
}

void ScreenManager::RemoveScreen(GameScreen *screen) {
  if (contentLoaded_)
    screen->UnloadContent();
  EraseByPointer(screens_, screen);
  EraseByPointer(screensToUpdate_, screen);
}

std::vector<std::shared_ptr<GameScreen>> ScreenManager::GetScreens() const {
  return screens_;
}

void ScreenManager::FadeBackBufferToBlack(int alpha) {
  const Viewport viewport = getGraphicsDeviceProperty().getViewportProperty();
  spriteBatch_->Begin();
  spriteBatch_->Draw(*blankTexture_,
                     Rectangle(0, 0, viewport.getWidthProperty(),
                               viewport.getHeightProperty()),
                     Color(0, 0, 0,
                           static_cast<SharpRuntime::intcs>(alpha)));
  spriteBatch_->End();
}

void ScreenManager::EraseByPointer(
    std::vector<std::shared_ptr<GameScreen>> &screens, GameScreen *screen) {
  screens.erase(std::remove_if(screens.begin(), screens.end(),
                               [screen](const auto &item) {
                                 return item.get() == screen;
                               }),
                screens.end());
}

void ScreenManager::TraceScreens() const {
  std::string names;
  for (const auto &screen : screens_) {
    if (!names.empty())
      names += ", ";
    names += screen->GetTypeName();
  }
  System::Diagnostics::Debug::WriteLine(names);
}

const std::string &ScreenManager::GetTypeName() const {
  static const std::string name = "NetRumble.ScreenManager";
  return name;
}
} // namespace NetRumble
