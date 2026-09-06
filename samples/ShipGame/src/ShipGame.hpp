// SPDX-License-Identifier: MS-PL
#pragma once

// ShipGame.hpp — C++ port of ShipGame/ShipGame.cs (XNA 4.0 Ship Game Starter
// Kit). The Game itself: the XACT engine, the three managers and the frame loop.

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::Audio::AudioEngine;
using Microsoft::Xna::Framework::Audio::SoundBank;
using Microsoft::Xna::Framework::Audio::WaveBank;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;

class FontManager;
class GameManager;
class ScreenManager;

// This is the main type for your game
class ShipGameGame : public Game {
public:
    ShipGameGame();

    ~ShipGameGame() override;

    // This is called to switch full screen mode.
    void ToggleFullScreen();

    static ShipGameGame* GetInstance();

    // The main entry point for the application.
    static int Main();

protected:
    // Allows the game to perform any initialization it needs to before starting to
    // run. This is where it can query for any required services and load any
    // non-graphic related content. Calling base.Initialize will enumerate through
    // any components and initialize them as well.
    void Initialize() override;

    // Load your graphics content.
    void LoadContent() override;

    // Unload your graphics content.
    void UnloadContent() override;

    // Allows the game to run logic such as updating the world,
    // checking for collisions, gathering input and playing audio.
    void Update(GameTime& gameTime) override;

    // This is called when the game should draw itself.
    void Draw(const GameTime& gameTime) override;

private:
    static ShipGameGame* instance_;

    GraphicsDeviceManager graphics_;
    std::unique_ptr<ScreenManager> screen_;
    std::unique_ptr<GameManager> game_;
    std::unique_ptr<FontManager> font_;

    std::optional<AudioEngine> audioEngine_;
    std::optional<WaveBank> waveBank_;
    std::optional<SoundBank> soundBank_;

    bool renderVsync_ = true;
};

} // namespace ShipGame
