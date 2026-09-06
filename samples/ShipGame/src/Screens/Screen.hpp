// SPDX-License-Identifier: MS-PL
#pragma once

// Screen.hpp — C++ port of ShipGame/Screens/Screen.cs (XNA 4.0 Ship Game
// Starter Kit). The screen list and the interface every screen implements.

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;

class FontManager;
class InputManager;

enum class ScreenType {
    ScreenIntro = 0,
    ScreenHelp,
    ScreenPlayer,
    ScreenLevel,
    ScreenGame,
    ScreenEnd
};

// Port of the Screen class in ShipGame/Screens/Screen.cs.
class Screen {
public:
    virtual ~Screen() = default;

    // called when screen gets or looses focus
    virtual void SetFocus(ContentManager* content, bool focus) = 0;

    // called to update input
    virtual void ProcessInput(float elapsedTime, InputManager* input) = 0;

    // called to update state
    virtual void Update(float elapsedTime) = 0;

    // called to draw the 3D world
    virtual void Draw3D(GraphicsDevice* gd) = 0;

    // called to draw the 2D info text and hud
    virtual void Draw2D(GraphicsDevice* gd, FontManager* font) = 0;
};

} // namespace ShipGame
