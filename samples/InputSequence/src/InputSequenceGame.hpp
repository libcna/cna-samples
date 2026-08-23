// SPDX-License-Identifier: MS-PL

#pragma once

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/TimeSpan.hpp"

#include "Direction.hpp"
#include "InputManager.hpp"
#include "Move.hpp"
#include "MoveList.hpp"

namespace InputSequenceSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /** @brief Demonstrates buffered recognition of keyboard and gamepad input sequences. */
    class InputSequenceSampleGame final : public Game
    {
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> spriteFont;

        std::vector<Move> moves;
        MoveList moveList;
        std::array<InputManager, 2> inputManagers;
        std::array<Move*, 2> playerMoves = {nullptr, nullptr};
        std::array<System::TimeSpan, 2> playerMoveTimes{};

        const System::TimeSpan MoveTimeOut = System::TimeSpan::FromSeconds(1.0);

        Texture2D upTexture;
        Texture2D downTexture;
        Texture2D leftTexture;
        Texture2D rightTexture;
        Texture2D upLeftTexture;
        Texture2D upRightTexture;
        Texture2D downLeftTexture;
        Texture2D downRightTexture;

        Texture2D aButtonTexture;
        Texture2D bButtonTexture;
        Texture2D xButtonTexture;
        Texture2D yButtonTexture;

        Texture2D plusTexture;
        Texture2D padFaceTexture;

        [[nodiscard]] static std::vector<Move> BuildMoves()
        {
            std::vector<Move> result;
            result.emplace_back("Jump", std::vector<Buttons>{Buttons::A});
            result.back().IsSubMove = true;
            result.emplace_back("Punch", std::vector<Buttons>{Buttons::X});
            result.back().IsSubMove = true;
            result.emplace_back("Double Jump", std::vector<Buttons>{Buttons::A, Buttons::A});
            result.emplace_back("Jump Kick", std::vector<Buttons>{Buttons::A | Buttons::X});
            result.emplace_back(
                "Quad Punch",
                std::vector<Buttons>{Buttons::X, Buttons::Y, Buttons::X, Buttons::Y});
            result.emplace_back(
                "Fireball",
                std::vector<Buttons>{
                    Direction::Down,
                    Direction::DownRight,
                    Direction::Right | Buttons::X});
            result.emplace_back(
                "Long Jump",
                std::vector<Buttons>{Direction::Up, Direction::Up, Buttons::A});
            result.emplace_back(
                "Back Flip",
                std::vector<Buttons>{Direction::Down, Direction::Down | Buttons::A});
            result.emplace_back(
                "30 Lives",
                std::vector<Buttons>{
                    Direction::Up,
                    Direction::Up,
                    Direction::Down,
                    Direction::Down,
                    Direction::Left,
                    Direction::Right,
                    Direction::Left,
                    Direction::Right,
                    Buttons::B,
                    Buttons::A});
            return result;
        }

        [[nodiscard]] static std::array<InputManager, 2> BuildInputManagers(int bufferSize)
        {
            return {
                InputManager(PlayerIndex::One, bufferSize),
                InputManager(PlayerIndex::Two, bufferSize),
            };
        }

    public:
        /** @brief Creates the Input Sequence sample and its original move list. */
        InputSequenceSampleGame()
            : graphics(this)
            , moves(BuildMoves())
            , moveList(moves)
            , inputManagers(BuildInputManagers(moveList.LongestMoveLength()))
        {
            getContentProperty().setRootDirectoryProperty("Content");
        }

        /**
         * @brief Returns the fully qualified runtime type name.
         *
         * @return Fully qualified .NET-compatible type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name =
                "InputSequenceSample.InputSequenceSampleGame";
            return name;
        }

    protected:
        /** @brief Loads the 15 original XNA Content Pipeline assets. */
        void LoadContent() override
        {
            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

            auto& content = getContentProperty();
            spriteFont.emplace(content.Load<SpriteFont>("Font"));

            upTexture = content.Load<Texture2D>("Up");
            downTexture = content.Load<Texture2D>("Down");
            leftTexture = content.Load<Texture2D>("Left");
            rightTexture = content.Load<Texture2D>("Right");
            upLeftTexture = content.Load<Texture2D>("UpLeft");
            upRightTexture = content.Load<Texture2D>("UpRight");
            downLeftTexture = content.Load<Texture2D>("DownLeft");
            downRightTexture = content.Load<Texture2D>("DownRight");

            aButtonTexture = content.Load<Texture2D>("A");
            bButtonTexture = content.Load<Texture2D>("B");
            xButtonTexture = content.Load<Texture2D>("X");
            yButtonTexture = content.Load<Texture2D>("Y");

            plusTexture = content.Load<Texture2D>("Plus");
            padFaceTexture = content.Load<Texture2D>("PadFace");
        }

        /**
         * @brief Updates both input histories and detects completed moves.
         *
         * @param gameTime Current timing snapshot.
         */
        void Update(GameTime& gameTime) override
        {
            for (std::size_t i = 0; i < inputManagers.size(); ++i)
            {
                if (gameTime.getTotalGameTimeProperty() - playerMoveTimes[i] > MoveTimeOut)
                {
                    playerMoves[i] = nullptr;
                }

                InputManager& inputManager = inputManagers[i];
                inputManager.Update(gameTime);

                if (inputManager.GamePadState.getButtonsProperty().getBackProperty() ==
                        ButtonState::Pressed ||
                    inputManager.KeyboardState.IsKeyDown(Keys::Escape))
                {
                    Exit();
                }

                Move* newMove = moveList.DetectMove(inputManager);
                if (newMove != nullptr)
                {
                    playerMoves[i] = newMove;
                    playerMoveTimes[i] = gameTime.getTotalGameTimeProperty();
                }
            }

            Game::Update(gameTime);
        }

        /**
         * @brief Draws the move list and both players' current input histories.
         *
         * @param gameTime Current timing snapshot.
         */
        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

            spriteBatch->Begin();

            const Vector2 topLeft(50.0f, 50.0f);
            const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
            const Vector2 bottomRight(
                static_cast<float>(viewport.getWidthProperty()) - topLeft.X,
                static_cast<float>(viewport.getHeightProperty()) - topLeft.Y);

            Vector2 position = topLeft;
            for (const Move& move : moves)
            {
                const Vector2 size = MeasureMove(move);
                if (position.X + size.X > bottomRight.X)
                {
                    position.X = topLeft.X;
                    position.Y += size.Y;
                }

                DrawMove(move, position);
                position.X += size.X + 30.0f;
            }

            position.Y += 90.0f;
            for (std::size_t i = 0; i < inputManagers.size(); ++i)
            {
                position.X = topLeft.X;
                DrawInput(i, position);
                position.Y += 80.0f;
            }

            spriteBatch->End();

            Game::Draw(gameTime);
        }

    private:
        [[nodiscard]] Vector2 MeasureMove(const Move& move) const
        {
            const Vector2 textSize = spriteFont->MeasureString(move.Name);
            const Vector2 sequenceSize = MeasureSequence(move.Sequence);
            return Vector2(
                std::max(textSize.X, sequenceSize.X),
                textSize.Y + sequenceSize.Y);
        }

        void DrawMove(const Move& move, Vector2 position)
        {
            DrawString(move.Name, position, Color::White);
            position.Y += spriteFont->MeasureString(move.Name).Y;
            DrawSequence(move.Sequence, position);
        }

        void DrawInput(std::size_t index, Vector2 position)
        {
            InputManager& inputManager = inputManagers[index];
            Move* move = playerMoves[index];

            const std::string text =
                std::string("Player ") + PlayerIndexName(inputManager.PlayerIndex) + " input  ";
            const Vector2 textSize = spriteFont->MeasureString(text);
            DrawString(text, position, Color::White);
            if (move != nullptr)
            {
                DrawString(
                    move->Name,
                    Vector2(position.X + textSize.X, position.Y),
                    Color::Red);
            }

            position.Y += textSize.Y;
            DrawSequence(inputManager.Buffer, position);
        }

        void DrawString(const std::string& text, Vector2 position, Color color)
        {
            spriteBatch->DrawString(
                *spriteFont,
                text,
                Vector2(position.X, position.Y + 1.0f),
                Color::Black);
            spriteBatch->DrawString(*spriteFont, text, position, color);
        }

        [[nodiscard]] Vector2 MeasureSequence(const std::vector<Buttons>& sequence) const
        {
            float width = 0.0f;
            for (Buttons buttons : sequence)
            {
                width += MeasureButtons(buttons).X;
            }
            return Vector2(width, static_cast<float>(padFaceTexture.getHeightProperty()));
        }

        void DrawSequence(const std::vector<Buttons>& sequence, Vector2 position)
        {
            for (Buttons buttons : sequence)
            {
                DrawButtons(buttons, position);
                position.X += MeasureButtons(buttons).X;
            }
        }

        [[nodiscard]] Vector2 MeasureButtons(Buttons buttons) const
        {
            const Buttons direction = Direction::FromButtons(buttons);
            float width;
            if (direction != Direction::None)
            {
                width = static_cast<float>(GetDirectionTexture(direction)->getWidthProperty());
                if ((buttons & ~direction) != Direction::None)
                {
                    width += static_cast<float>(plusTexture.getWidthProperty()) +
                        static_cast<float>(padFaceTexture.getWidthProperty());
                }
            }
            else
            {
                width = static_cast<float>(padFaceTexture.getWidthProperty());
            }

            return Vector2(width, static_cast<float>(padFaceTexture.getHeightProperty()));
        }

        void DrawButtons(Buttons buttons, Vector2 position)
        {
            const Buttons direction = Direction::FromButtons(buttons);
            const Texture2D* directionTexture = GetDirectionTexture(direction);

            if (directionTexture != nullptr)
            {
                spriteBatch->Draw(*directionTexture, position, Color::White);
                position.X += static_cast<float>(directionTexture->getWidthProperty());
            }

            if ((buttons & ~direction) != Direction::None)
            {
                if (directionTexture != nullptr)
                {
                    spriteBatch->Draw(plusTexture, position, Color::White);
                    position.X += static_cast<float>(plusTexture.getWidthProperty());
                }

                spriteBatch->Draw(padFaceTexture, position, Color::White);
                if ((buttons & Buttons::A) != Direction::None)
                {
                    spriteBatch->Draw(aButtonTexture, position, Color::White);
                }
                if ((buttons & Buttons::B) != Direction::None)
                {
                    spriteBatch->Draw(bButtonTexture, position, Color::White);
                }
                if ((buttons & Buttons::X) != Direction::None)
                {
                    spriteBatch->Draw(xButtonTexture, position, Color::White);
                }
                if ((buttons & Buttons::Y) != Direction::None)
                {
                    spriteBatch->Draw(yButtonTexture, position, Color::White);
                }
            }
        }

        [[nodiscard]] const Texture2D* GetDirectionTexture(Buttons direction) const
        {
            switch (direction)
            {
                case Direction::Up: return &upTexture;
                case Direction::Down: return &downTexture;
                case Direction::Left: return &leftTexture;
                case Direction::Right: return &rightTexture;
                case Direction::UpLeft: return &upLeftTexture;
                case Direction::UpRight: return &upRightTexture;
                case Direction::DownLeft: return &downLeftTexture;
                case Direction::DownRight: return &downRightTexture;
                default: return nullptr;
            }
        }

        [[nodiscard]] static const char* PlayerIndexName(PlayerIndex playerIndex)
        {
            switch (playerIndex)
            {
                case PlayerIndex::One: return "One";
                case PlayerIndex::Two: return "Two";
                case PlayerIndex::Three: return "Three";
                case PlayerIndex::Four: return "Four";
            }
            return "Unknown";
        }
    };
}
