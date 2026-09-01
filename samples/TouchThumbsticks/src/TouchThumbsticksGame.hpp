// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// TouchThumbsticksGame.cs
//
// Microsoft Advanced Technology Group
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"

#include "Bullet.hpp"
#include "EnemyShip.hpp"
#include "PlayerShip.hpp"
#include "VirtualThumbsticks.hpp"

namespace TouchThumbsticks
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::RasterizerState;
    using Microsoft::Xna::Framework::Graphics::SamplerState;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;

    /**
     * @brief Demonstrates controlling movement and fire with two virtual touch thumbsticks.
     */
    class TouchThumbsticksGame : public Game
    {
    private:
        static constexpr int graphicsWidth = 800;
        static constexpr int graphicsHeight = 480;
        static constexpr int graphicsWidthHalf = graphicsWidth / 2;
        static constexpr int graphicsHeightHalf = graphicsHeight / 2;
        static constexpr int worldWidth = 1000;
        static constexpr int worldHeight = 1000;
        static constexpr int numStars = 1000;
        static constexpr int worldBorderThickness = 4;
        static constexpr int worldBorderThicknessDouble = worldBorderThickness * 2;

        System::TimeSpan spawnRate = System::TimeSpan::FromSeconds(2.0);
        Color worldBorderColor = Color::Red;
        inline static System::Random rand;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        Texture2D blank;
        Texture2D thumbstick;
        System::TimeSpan spawnTimer;
        std::vector<Vector3> stars;
        std::optional<PlayerShip> player;
        std::vector<EnemyShip> enemies;

        void DrawWorldBorder()
        {
            Rectangle rectangle(
                -worldWidth / 2 - worldBorderThickness,
                -worldHeight / 2 - worldBorderThickness,
                worldBorderThickness,
                worldHeight + worldBorderThicknessDouble);
            spriteBatch->Draw(blank, rectangle, worldBorderColor);

            rectangle = Rectangle(
                -worldWidth / 2 - worldBorderThickness,
                -worldHeight / 2 - worldBorderThickness,
                worldWidth + worldBorderThicknessDouble,
                worldBorderThickness);
            spriteBatch->Draw(blank, rectangle, worldBorderColor);

            rectangle = Rectangle(
                worldWidth / 2,
                -worldHeight / 2 - worldBorderThickness,
                worldBorderThickness,
                worldHeight + worldBorderThicknessDouble);
            spriteBatch->Draw(blank, rectangle, worldBorderColor);

            rectangle = Rectangle(
                -worldWidth / 2 - worldBorderThickness,
                worldHeight / 2,
                worldWidth + worldBorderThicknessDouble,
                worldBorderThickness);
            spriteBatch->Draw(blank, rectangle, worldBorderColor);
        }

    public:
        /** @brief Constructs the touch-thumbsticks sample. */
        TouchThumbsticksGame()
            : graphics(this)
        {
            graphics.setPreferredBackBufferHeightProperty(graphicsHeight);
            graphics.setPreferredBackBufferWidthProperty(graphicsWidth);
            graphics.setIsFullScreenProperty(true);
            getContentProperty().setRootDirectoryProperty("Content");
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        }

        /**
         * @brief Returns the fully qualified logical type name.
         *
         * @return `TouchThumbsticks.TouchThumbsticksGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name =
                "TouchThumbsticks.TouchThumbsticksGame";
            return name;
        }

    protected:
        /** @brief Loads authentic textures and creates the starfield. */
        void LoadContent() override
        {
            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

            auto& content = getContentProperty();
            Bullet::Texture = content.Load<Texture2D>("bullet");
            thumbstick = content.Load<Texture2D>("thumbstick");

            player.emplace(content.Load<Texture2D>("player1"));
            player->WorldWidth = worldWidth;
            player->WorldHeight = worldHeight;

            blank = Texture2D(getGraphicsDeviceProperty(), 1, 1);
            Color white = Color::White;
            blank.SetData(&white, 1);

            for (int i = 0; i < numStars; ++i)
            {
                stars.emplace_back(
                    static_cast<float>(rand.NextDouble()) *
                            (worldWidth + graphicsWidth) -
                        (worldWidth / 2.0f + graphicsWidthHalf),
                    static_cast<float>(rand.NextDouble()) *
                            (worldWidth + graphicsHeight) -
                        (worldWidth / 2.0f + graphicsHeightHalf),
                    static_cast<float>(rand.Next(1, 3)));
            }
        }

        /**
         * @brief Updates input, spawning, ships, bullets and collisions.
         *
         * @param gameTime Current game timing snapshot.
         */
        void Update(GameTime& gameTime) override
        {
            if (GamePad::GetState(PlayerIndex::One)
                    .getButtonsProperty().getBackProperty() == ButtonState::Pressed)
            {
                Exit();
            }

            VirtualThumbsticks::Update();

            spawnTimer -= gameTime.getElapsedGameTimeProperty();
            if (spawnTimer <= System::TimeSpan::Zero)
            {
                const int numToSpawn = rand.Next(1, 3);
                for (int i = 0; i < numToSpawn; ++i)
                {
                    EnemyShip enemy(
                        getContentProperty().Load<Texture2D>("alien"));
                    enemy.Player = &*player;

                    if (rand.Next() % 2 == 0)
                    {
                        enemy.Position.X =
                            -worldWidth / 2.0f - (graphicsWidthHalf + 10);
                    }
                    else
                    {
                        enemy.Position.X =
                            worldWidth / 2.0f + (graphicsWidthHalf + 10);
                    }

                    if (rand.Next() % 2 == 0)
                    {
                        enemy.Position.Y =
                            -worldHeight / 2.0f - (graphicsHeightHalf + 10);
                    }
                    else
                    {
                        enemy.Position.Y =
                            worldHeight / 2.0f + (graphicsHeightHalf + 10);
                    }

                    enemies.push_back(std::move(enemy));
                }
                spawnTimer = spawnRate;
            }

            player->Update(gameTime);
            for (EnemyShip& enemy : enemies)
                enemy.Update(gameTime);

            std::vector<std::size_t> enemiesToRemove;
            for (auto bullet = player->Bullets.begin();
                 bullet != player->Bullets.end();)
            {
                bool hit = false;
                for (std::size_t enemyIndex = 0;
                     enemyIndex < enemies.size();
                     ++enemyIndex)
                {
                    if (enemies[enemyIndex].ContainsPoint((*bullet)->Position))
                    {
                        enemiesToRemove.push_back(enemyIndex);
                        hit = true;
                        break;
                    }
                }
                bullet = hit ? player->Bullets.erase(bullet) : std::next(bullet);
            }

            std::sort(enemiesToRemove.begin(), enemiesToRemove.end());
            enemiesToRemove.erase(
                std::unique(enemiesToRemove.begin(), enemiesToRemove.end()),
                enemiesToRemove.end());
            for (auto index = enemiesToRemove.rbegin();
                 index != enemiesToRemove.rend();
                 ++index)
            {
                enemies.erase(
                    enemies.begin() + static_cast<std::ptrdiff_t>(*index));
            }

            Game::Update(gameTime);
        }

        /**
         * @brief Draws the world, actors and active touch-stick indicators.
         *
         * @param gameTime Current game timing snapshot.
         */
        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::Black);

            const Matrix cameraTransform = Matrix::CreateTranslation(
                -player->Position.X + graphicsWidthHalf,
                -player->Position.Y + graphicsHeightHalf,
                0.0f);

            spriteBatch->Begin(
                SpriteSortMode::Deferred,
                BlendState::AlphaBlend,
                const_cast<SamplerState*>(&SamplerState::LinearClamp),
                const_cast<DepthStencilState*>(&DepthStencilState::Default),
                const_cast<RasterizerState*>(&RasterizerState::CullNone),
                nullptr,
                cameraTransform);

            for (const Vector3& star : stars)
            {
                spriteBatch->Draw(
                    blank,
                    Rectangle(
                        static_cast<int>(star.X),
                        static_cast<int>(star.Y),
                        static_cast<int>(star.Z),
                        static_cast<int>(star.Z)),
                    Color::White);
            }

            DrawWorldBorder();

            for (const EnemyShip& enemy : enemies)
                enemy.Draw(*spriteBatch);
            player->Draw(*spriteBatch);

            spriteBatch->End();
            spriteBatch->Begin();

            const auto leftCenter =
                VirtualThumbsticks::getLeftThumbstickCenterProperty();
            if (leftCenter.has_value())
            {
                spriteBatch->Draw(
                    thumbstick,
                    *leftCenter - Vector2(
                        static_cast<float>(thumbstick.getWidthProperty()) / 2.0f,
                        static_cast<float>(thumbstick.getHeightProperty()) / 2.0f),
                    Color::Green);
            }

            const auto rightCenter =
                VirtualThumbsticks::getRightThumbstickCenterProperty();
            if (rightCenter.has_value())
            {
                spriteBatch->Draw(
                    thumbstick,
                    *rightCenter - Vector2(
                        static_cast<float>(thumbstick.getWidthProperty()) / 2.0f,
                        static_cast<float>(thumbstick.getHeightProperty()) / 2.0f),
                    Color::Blue);
            }

            spriteBatch->End();
            Game::Draw(gameTime);
        }
    };
}
