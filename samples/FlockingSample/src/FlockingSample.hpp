// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FlockingSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "AIParameters.hpp"
#include "Animals/Cat.hpp"
#include "Flock.hpp"
#include "InputState.hpp"

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief This is the main type for your game.
     */
    class FlockingSample : public Microsoft::Xna::Framework::Game
    {
        /** X location to start drawing the HUD from */
        static constexpr int hudLocX = 200;
        /** Y location to start drawing the HUD from */
        static constexpr int hudLocY = 30;
        /** Min value for the distance sliders */
        static constexpr float sliderMin = 0.0f;
        /** Max value for the distance sliders */
        static constexpr float sliderMax = 100.0f;
        /** Width of the slider button */
        static constexpr int sliderButtonWidth = 10;

        // Default value for the AI parameters
        static constexpr float detectionDefault = 70.0f;
        static constexpr float separationDefault = 50.0f;
        static constexpr float moveInOldDirInfluenceDefault = 1.0f;
        static constexpr float moveInFlockDirInfluenceDefault = 1.0f;
        static constexpr float moveInRandomDirInfluenceDefault = 0.05f;
        static constexpr float maxTurnRadiansDefault = 6.0f;
        static constexpr float perMemberWeightDefault = 1.0f;
        static constexpr float perDangerWeightDefault = 50.0f;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        InputState inputState;
        // XNA's SpriteFont is a reference type, so the original's field is null until
        // LoadContent assigns it; std::optional is this port's established stand-in.
        std::optional<SpriteFont> hudFont;

        // Do we need to update AI parameers this Update
        bool aiParameterUpdate = false;
        bool moveCat = false;

#if defined(WINDOWS) || defined(XBOX)
        Texture2D bButton;
        Texture2D xButton;
        Texture2D yButton;
#endif
        Texture2D onePixelWhite;
        Texture2D birdTexture;
        Texture2D catTexture;

        std::unique_ptr<Cat> cat;
        std::unique_ptr<Flock> flock;
        AIParameters flockParams;

        // Definte the dimensions of the controls
        Rectangle barDetectionDistance = Rectangle(205, 45, 85, 40);
        Rectangle barSeparationDistance = Rectangle(205, 125, 85, 40);
        Rectangle buttonResetDistance = Rectangle(105, 205, 140, 40);
        Rectangle buttonResetFlock = Rectangle(105, 285, 140, 40);
        Rectangle buttonToggleCat = Rectangle(105, 365, 140, 40);

        int selectionNum = 0;

    public:
        /** @brief Constructs the game and sets the AI parameters to their defaults. */
        FlockingSample();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Flocking.FlockingSample".
         */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Enables the gestures the sample cares about. */
        void Initialize() override;

        /** @brief Loads the textures, the HUD font and the one-pixel white texture. */
        void LoadContent() override;

        /**
         * @brief Advances the cat and the flock.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

        /** @brief Creates the flock if it does not exist yet. */
        void SpawnFlock();

        /** @brief Adds the cat if there is none, removes it if there is. */
        void ToggleCat();

    private:
        void HandleInput();
        void SliderInputHelper(Rectangle touchRectangle);
        void DrawButton(Rectangle button, const std::string& label);
        void DrawBar(Rectangle bar, float barWidthNormalized, const std::string& label,
                     const GameTime& gameTime, bool highlighted);
        void ResetAIParams();
    };
}
