// SPDX-License-Identifier: MS-PL
#pragma once

// ScreenManager.hpp — C++ port of ShipGame/Screens/ScreenManager.cs (XNA 4.0
// Ship Game Starter Kit). Owns the six screens, the fade between them, and the
// colour/glow render-target pipeline every screen draws through.

#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/IDisposable.hpp"

#include "Screen.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::RenderTarget2D;
using Microsoft::Xna::Framework::Graphics::Texture2D;

class BlurManager;
class FontManager;
class GameManager;
class InputManager;
class ScreenEnd;
class ScreenGame;
class ScreenIntro;
class ScreenLevel;
class ScreenPlayer;
class ShipGameGame;

// Port of ShipGame/Screens/ScreenManager.cs.
class ScreenManager : public System::IDisposable {
public:
    // constructor
    ScreenManager(ShipGameGame* shipGame, FontManager* font, GameManager* game);

    ~ScreenManager() override;

    // process input
    void ProcessInput(float elapsedTime);

    // update for given elapsed time
    void Update(float elapsedTime);

    // draw a texture with destination rectangle, color and blend mode
    void DrawTexture(Texture2D* texture, Rectangle rect, Color color, BlendState blend);

    // draw a texture with source and destination rectangles, color and blend mode
    void DrawTexture(Texture2D* texture, Rectangle destinationRect, Rectangle sourceRect,
                     Color color, BlendState blend);

    // draw a texture with desination rectange, rotation, color and blend settings
    void DrawTexture(Texture2D* texture, Rectangle rect, float rotation, Color color,
                     BlendState blend);

    // draw the background animated image
    void DrawBackground(GraphicsDevice* gd);

    // draws the currently active screen
    void Draw(GraphicsDevice* gd);

    // load all content
    void LoadContent(GraphicsDevice* gd, ContentManager& content);

    // unload all content
    void UnloadContent();

    // starts a transition to a new screen
    // using a 1 sec fade time to custom color
    bool SetNextScreen(ScreenType screenType, Vector4 fadeColor, float fadeTime);

    // starts a transition to a new screen
    // using a 1 sec fade time to custom color
    bool SetNextScreen(ScreenType screenType, Vector4 fadeColor);

    // starts a transition to a new screen
    // using a 1 sec fade time to black
    bool SetNextScreen(ScreenType screenType);

    // get screen with given type
    Screen* GetScreen(ScreenType screenType);

    // get intro screen
    ScreenIntro* getScreenIntro();

    // get help screen. The upstream property is declared to return ScreenIntro, which the help
    // screen is not; nothing calls it, and the cast is preserved here as a checked one.
    ScreenIntro* getScreenHelp();

    // get player screen
    ScreenPlayer* getScreenPlayer();

    // get level screen
    ScreenLevel* getScreenLevel();

    // get game screen
    ScreenGame* getScreenGame();

    // get end screen
    ScreenEnd* getScreenEnd();

    // exit game
    void Exit();

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override;

private:
    void Dispose(bool disposing);

    // blur the color render target using the alpha channel and blur intensity
    void BlurGlowRenterTarget(GraphicsDevice* gd);

    // draw render target as fullscreen texture with given intensity and blend mode
    void DrawRenderTargetTexture(GraphicsDevice* gd, RenderTarget2D* renderTarget, float intensity,
                                 bool additiveBlend);

    bool isDisposed_ = false;

    ShipGameGame* shipGame_ = nullptr;          // xna game
    GameManager* gameManager_ = nullptr;        // game manager
    FontManager* fontManager_ = nullptr;        // font manager
    std::unique_ptr<InputManager> inputManager_; // input manager
    ContentManager* contentManager_ = nullptr;  // content manager

    std::vector<std::unique_ptr<Screen>> screens_; // list of available screens
    Screen* current_ = nullptr;                    // currently active screen
    Screen* next_ = nullptr;                       // next screen on a transition
                                                   // (null for no transition)

    float fadeTime_ = 1.0f;              // total fade time when in a transition
    float fade_ = 0.0f;                  // current fade time when in a transition
    Vector4 fadeColor_ = Vector4::One;   // color fading in and out

    std::optional<RenderTarget2D> colorRT_; // render target for main color buffer
    std::optional<RenderTarget2D> glowRT1_; // render target for glow horizontal blur
    std::optional<RenderTarget2D> glowRT2_; // render target for glow vertical blur

    std::unique_ptr<BlurManager> blurManager_; // blur manager

    int frameRate_ = 0;          // current game frame rate (in frames per sec)
    int frameRateCount_ = 0;     // current frame count since last frame rate update
    float frameRateTime_ = 0.0f; // elapsed time since last frame rate update

    std::optional<Texture2D> textureBackground_; // the background texture used on menus
    float backgroundTime_ = 0.0f; // time for background animation used on menus
};

} // namespace ShipGame
