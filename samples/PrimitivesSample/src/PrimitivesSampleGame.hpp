// SPDX-License-Identifier: MS-PL

#pragma once

#include <cmath>
#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Random.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"

#include "PrimitiveBatch.hpp"

namespace PrimitivesSample
{
    using SharpRuntime::bytecs;
    using SharpRuntime::intcs;
    using SharpRuntime::Single;
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using System::Collections::Generic::List;

    /** @brief Recreates the Spacewar retro scene using batched lines and points. */
    class PrimitivesSampleGame : public Microsoft::Xna::Framework::Game
    {
        static constexpr intcs NumStars = 500;
        static constexpr Single PercentBigStars = 0.2f;
        static constexpr bytecs MinimumStarBrightness = 56;
        static constexpr bytecs MaximumStarBrightness = 255;
        static constexpr Single ShipSizeX = 10.0f;
        static constexpr Single ShipSizeY = 15.0f;
        static constexpr Single ShipCutoutSize = 5.0f;
        static constexpr Single SunSize = 30.0f;

        GraphicsDeviceManager graphics;
        std::unique_ptr<PrimitiveBatch> primitiveBatch;
        List<Vector2> stars;
        List<Color> starColors;

        void CreateStars()
        {
            System::Random random;
            const auto& vp         = graphics.getGraphicsDeviceProperty()->getViewportProperty();
            const intcs screenWidth = vp.getWidthProperty();
            const intcs screenHeight = vp.getHeightProperty();

            for (intcs i = 0; i < NumStars; ++i)
            {
                Vector2 where(static_cast<Single>(random.Next(0, screenWidth)),
                              static_cast<Single>(random.Next(0, screenHeight)));

                const bytecs greyValue = static_cast<bytecs>(
                    random.Next(MinimumStarBrightness, MaximumStarBrightness));
                Color color(static_cast<intcs>(greyValue),
                            static_cast<intcs>(greyValue),
                            static_cast<intcs>(greyValue));

                if (static_cast<Single>(random.NextDouble()) > PercentBigStars)
                {
                    starColors.Add(color);
                    stars.Add(where);
                }
                else
                {
                    for (intcs j = 0; j < 4; ++j)
                        starColors.Add(color);

                    stars.Add(where);
                    stars.Add(where + Vector2::UnitX);
                    stars.Add(where + Vector2::UnitY);
                    stars.Add(where + Vector2::One);
                }
            }
        }

        void DrawStars()
        {
            primitiveBatch->Begin(PrimitiveType::TriangleList);
            for (intcs i = 0; i < stars.getCountProperty(); ++i)
            {
                primitiveBatch->AddVertex(stars[i],                  starColors[i]);
                primitiveBatch->AddVertex(stars[i] + Vector2::UnitX, starColors[i]);
                primitiveBatch->AddVertex(stars[i] + Vector2::UnitY, starColors[i]);
            }
            primitiveBatch->End();
        }

        void DrawShip(Vector2 where)
        {
            primitiveBatch->Begin(PrimitiveType::LineList);

            primitiveBatch->AddVertex(where + Vector2(0.0f,         -ShipSizeY),              Color::White);
            primitiveBatch->AddVertex(where + Vector2(-ShipSizeX,    ShipSizeY),              Color::White);

            primitiveBatch->AddVertex(where + Vector2(-ShipSizeX,    ShipSizeY),              Color::White);
            primitiveBatch->AddVertex(where + Vector2(0.0f,          ShipSizeY - ShipCutoutSize), Color::White);

            primitiveBatch->AddVertex(where + Vector2(0.0f,          ShipSizeY - ShipCutoutSize), Color::White);
            primitiveBatch->AddVertex(where + Vector2(ShipSizeX,     ShipSizeY),              Color::White);

            primitiveBatch->AddVertex(where + Vector2(ShipSizeX,     ShipSizeY),              Color::White);
            primitiveBatch->AddVertex(where + Vector2(0.0f,         -ShipSizeY),              Color::White);

            primitiveBatch->End();
        }

        void DrawSun(Vector2 where)
        {
            primitiveBatch->Begin(PrimitiveType::LineList);

            primitiveBatch->AddVertex(where + Vector2(0.0f,   SunSize),  Color::White);
            primitiveBatch->AddVertex(where + Vector2(0.0f,  -SunSize),  Color::White);

            primitiveBatch->AddVertex(where + Vector2( SunSize, 0.0f),   Color::White);
            primitiveBatch->AddVertex(where + Vector2(-SunSize, 0.0f),   Color::White);

            Single diagonal = std::cos(MathHelper::PiOver4);
            diagonal *= SunSize;

            primitiveBatch->AddVertex(where + Vector2(-diagonal,  diagonal), Color::Gray);
            primitiveBatch->AddVertex(where + Vector2( diagonal, -diagonal), Color::Gray);

            primitiveBatch->AddVertex(where + Vector2( diagonal,  diagonal), Color::Gray);
            primitiveBatch->AddVertex(where + Vector2(-diagonal, -diagonal), Color::Gray);

            primitiveBatch->End();
        }

    public:
        /** @brief Constructs the Windows version of the Primitives sample. */
        PrimitivesSampleGame() : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
            graphics.setPreferredBackBufferWidthProperty(853);
            graphics.setPreferredBackBufferHeightProperty(480);
        }

        /** @brief Destroys the sample and its owned primitive batch. */
        ~PrimitivesSampleGame() override = default;

        /**
         * @brief Returns the fully qualified runtime type name.
         *
         * @return Fully qualified .NET-style type name.
         */
        CNAEXT const std::string& GetTypeName() const override
        {
            static const std::string name = "PrimitivesSample.PrimitivesSampleGame";
            return name;
        }

    protected:
        /** @brief Initializes the game and generates its random star field. */
        void Initialize() override
        {
            Game::Initialize();
            CreateStars();
        }

        /** @brief Creates the PrimitiveBatch used by this sample. */
        void LoadContent() override
        {
            primitiveBatch = std::make_unique<PrimitiveBatch>(
                *graphics.getGraphicsDeviceProperty());
        }

        /**
         * @brief Processes the original Back/Escape exit controls.
         *
         * @param gameTime Current frame timing information.
         */
        void Update(GameTime& gameTime) override
        {
            if (GamePad::GetState(PlayerIndex::One)
                        .getButtonsProperty()
                        .getBackProperty() == ButtonState::Pressed ||
                Keyboard::GetState().IsKeyDown(Keys::Escape))
            {
                Exit();
            }
            Game::Update(gameTime);
        }

        /**
         * @brief Draws the original stars, ships, and sun scene.
         *
         * @param gameTime Current frame timing information.
         */
        void Draw(const GameTime& gameTime) override
        {
            graphics.getGraphicsDeviceProperty()->Clear(Color::Black);

            const auto& vp        = graphics.getGraphicsDeviceProperty()->getViewportProperty();
            const intcs screenWidth = vp.getWidthProperty();
            const intcs screenHeight = vp.getHeightProperty();

            DrawSun(Vector2(static_cast<Single>(screenWidth / 2),
                            static_cast<Single>(screenHeight / 2)));
            DrawShip(Vector2(100.0f, static_cast<Single>(screenHeight / 2)));
            DrawShip(Vector2(static_cast<Single>(screenWidth - 100),
                             static_cast<Single>(screenHeight / 2)));
            DrawStars();

            Game::Draw(gameTime);
        }
    };
}
