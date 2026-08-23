// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <cmath>
#include <cstdio>
#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadCapabilities.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadDeadZone.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadType.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "ChargeSwitchDeadZone.hpp"
#include "ChargeSwitchExit.hpp"
#include "InputReporterResources.hpp"

namespace InputReporter
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /** @brief Displays live input values for all connected controllers. */
    class InputReporterGame final : public Game
    {
        inline static const std::array<Vector2, 4> connectedControllerPositions = {
            Vector2(606.0f, 60.0f),
            Vector2(656.0f, 60.0f),
            Vector2(606.0f, 110.0f),
            Vector2(656.0f, 110.0f),
        };
        inline static const std::array<Vector2, 4> selectedControllerPositions = {
            Vector2(594.0f, 36.0f),
            Vector2(686.0f, 36.0f),
            Vector2(594.0f, 137.0f),
            Vector2(686.0f, 137.0f),
        };

        inline static const Vector2 titlePosition = Vector2(180.0f, 73.0f);
        inline static const Vector2 typeCenterPosition = Vector2(660.0f, 270.0f);
        inline static const Vector2 descriptionColumn1Position = Vector2(65.0f, 135.0f);
        inline static const Vector2 valueColumn1Position = Vector2(220.0f, 135.0f);
        inline static const Vector2 descriptionColumn2Position = Vector2(310.0f, 135.0f);
        inline static const Vector2 valueColumn2Position = Vector2(472.0f, 135.0f);
        inline static const Vector2 deadZoneInstructionsPosition = Vector2(570.0f, 380.0f);
        inline static const Vector2 exitInstructionsPosition = Vector2(618.0f, 425.0f);

        inline static const Color titleColor = Color(60, 134, 11, 255);
        inline static const Color typeColor = Color(38, 108, 87, 255);
        inline static const Color descriptionColor = Color(33, 89, 15, 255);
        inline static const Color valueColor = Color(38, 108, 87, 255);
        inline static const Color disabledColor = Color(171, 171, 171, 255);
        inline static const Color instructionsColor = Color(127, 130, 127, 255);

        static constexpr float deadZoneChargeSwitchDuration = 2.0f;
        static constexpr float exitChargeSwitchDuration = 2.0f;

        int selectedPlayer = 0;
        std::array<GamePadState, 4> gamePadStates;
        std::array<GamePadCapabilities, 4> gamePadCapabilities;
        KeyboardState lastKeyboardState;

        GamePadDeadZone deadZone = GamePadDeadZone::IndependentAxes;
        std::string deadZoneString;
        Vector2 deadZoneStringPosition;
        Vector2 deadZoneStringCenterPosition;

        ChargeSwitchExit exitSwitch;
        ChargeSwitchDeadZone deadZoneSwitch;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> titleFont;
        std::optional<SpriteFont> dataFont;
        std::optional<SpriteFont> dataActiveFont;
        std::optional<SpriteFont> typeFont;
        std::optional<SpriteFont> instructionsFont;
        std::optional<SpriteFont> instructionsActiveFont;
        Texture2D backgroundTexture;
        std::array<Texture2D, 4> connectedControllerTextures;
        std::array<Texture2D, 4> selectedControllerTextures;
        float dataSpacing = 0.0f;

    public:
        /**
         * @brief Gets the dead-zone processing mode used when reading controllers.
         *
         * @return Current dead-zone mode.
         */
        [[nodiscard]] GamePadDeadZone getDeadZoneProperty() const
        {
            return deadZone;
        }

        /**
         * @brief Sets the dead-zone processing mode and updates its centered label.
         *
         * @param value New dead-zone mode.
         */
        void setDeadZoneProperty(GamePadDeadZone value)
        {
            deadZone = value;
            deadZoneString = std::string("(") + DeadZoneName(deadZone) + ")";
            if (dataFont.has_value())
            {
                const Vector2 deadZoneStringSize = dataFont->MeasureString(deadZoneString);
                deadZoneStringPosition = Vector2(
                    static_cast<float>(std::floor(
                        deadZoneStringCenterPosition.X - deadZoneStringSize.X / 2.0f)),
                    static_cast<float>(std::floor(
                        deadZoneStringCenterPosition.Y - deadZoneStringSize.Y / 2.0f)));
            }
        }

        /** @brief Creates the Input Reporter game. */
        InputReporterGame()
            : exitSwitch(exitChargeSwitchDuration)
            , deadZoneSwitch(deadZoneChargeSwitchDuration)
            , graphics(this)
        {
            graphics.setPreferredBackBufferWidthProperty(853);
            graphics.setPreferredBackBufferHeightProperty(480);
            getContentProperty().setRootDirectoryProperty("Content");
            exitSwitch.Fire += [this]() { exitSwitch_Fire(); };
            deadZoneSwitch.Fire += [this]() { ToggleDeadZone(); };
        }

        /**
         * @brief Returns the fully qualified runtime type name.
         *
         * @return Fully qualified .NET-compatible type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "InputReporter.InputReporterGame";
            return name;
        }

    protected:
        /** @brief Resets controller selection, charge switches, and dead-zone mode. */
        void Initialize() override
        {
            selectedPlayer = 0;

            exitSwitch.Reset(exitChargeSwitchDuration);
            deadZoneSwitch.Reset(deadZoneChargeSwitchDuration);

            Game::Initialize();

            setDeadZoneProperty(GamePadDeadZone::IndependentAxes);
        }

        /** @brief Loads all original XNA content assets. */
        void LoadContent() override
        {
            spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
            auto& content = getContentProperty();
            titleFont.emplace(content.Load<SpriteFont>("Fonts\\TitleFont"));
            dataFont.emplace(content.Load<SpriteFont>("Fonts\\DataFont"));
            dataActiveFont.emplace(content.Load<SpriteFont>("Fonts\\DataActiveFont"));
            typeFont.emplace(content.Load<SpriteFont>("Fonts\\TypeFont"));
            instructionsFont.emplace(content.Load<SpriteFont>("Fonts\\InstructionsFont"));
            instructionsActiveFont.emplace(
                content.Load<SpriteFont>("Fonts\\InstructionsActiveFont"));
            dataSpacing = static_cast<float>(
                std::floor(static_cast<float>(dataFont->getLineSpacingProperty()) * 1.3f));
            deadZoneStringCenterPosition = Vector2(
                687.0f,
                static_cast<float>(std::floor(
                    deadZoneInstructionsPosition.Y +
                    static_cast<float>(dataFont->getLineSpacingProperty()) * 1.7f)));

            backgroundTexture = content.Load<Texture2D>("Textures\\Background");
            connectedControllerTextures[0] =
                content.Load<Texture2D>("Textures\\connected_controller1");
            connectedControllerTextures[1] =
                content.Load<Texture2D>("Textures\\connected_controller2");
            connectedControllerTextures[2] =
                content.Load<Texture2D>("Textures\\connected_controller3");
            connectedControllerTextures[3] =
                content.Load<Texture2D>("Textures\\connected_controller4");
            selectedControllerTextures[0] =
                content.Load<Texture2D>("Textures\\select_controller1");
            selectedControllerTextures[1] =
                content.Load<Texture2D>("Textures\\select_controller2");
            selectedControllerTextures[2] =
                content.Load<Texture2D>("Textures\\select_controller3");
            selectedControllerTextures[3] =
                content.Load<Texture2D>("Textures\\select_controller4");
        }

        /**
         * @brief Updates keyboard, controller selection, and charge switches.
         *
         * @param gameTime Current timing snapshot.
         */
        void Update(GameTime& gameTime) override
        {
            const KeyboardState keyboardState = Keyboard::GetState();
            if (keyboardState.IsKeyDown(Keys::Escape))
            {
                Exit();
            }
            if (keyboardState.IsKeyDown(Keys::Space) &&
                !lastKeyboardState.IsKeyDown(Keys::Space))
            {
                ToggleDeadZone();
            }

            bool setSelectedPlayer = false;
            for (int i = 0; i < 4; ++i)
            {
                gamePadStates[i] = GamePad::GetState(
                    static_cast<PlayerIndex>(i), deadZone);
                gamePadCapabilities[i] = GamePad::GetCapabilities(
                    static_cast<PlayerIndex>(i));
                if (!setSelectedPlayer && IsActiveGamePad(gamePadStates[i]))
                {
                    selectedPlayer = i;
                    setSelectedPlayer = true;
                }
            }

            deadZoneSwitch.Update(gameTime, gamePadStates[selectedPlayer]);
            exitSwitch.Update(gameTime, gamePadStates[selectedPlayer]);

            Game::Update(gameTime);

            lastKeyboardState = keyboardState;
        }

        /**
         * @brief Draws the complete controller report.
         *
         * @param gameTime Current timing snapshot.
         */
        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::Black);

            Game::Draw(gameTime);

            spriteBatch->Begin();

            spriteBatch->Draw(backgroundTexture, Vector2::Zero, Color::White);

            for (int i = 0; i < 4; ++i)
            {
                if (gamePadStates[i].getIsConnectedProperty())
                {
                    spriteBatch->Draw(
                        connectedControllerTextures[i],
                        connectedControllerPositions[i],
                        Color::White);
                }
            }
            spriteBatch->Draw(
                selectedControllerTextures[selectedPlayer],
                selectedControllerPositions[selectedPlayer],
                Color::White);

            std::string text = std::string(InputReporterResources::Title) +
                PlayerIndexName(static_cast<PlayerIndex>(selectedPlayer));
            spriteBatch->DrawString(*titleFont, text, titlePosition, titleColor);

            text = GamePadTypeName(
                gamePadCapabilities[selectedPlayer].getGamePadTypeProperty());
            const Vector2 textSize = typeFont->MeasureString(text);
            spriteBatch->DrawString(
                *typeFont,
                text,
                Vector2(
                    static_cast<float>(std::floor(
                        typeCenterPosition.X - textSize.X / 2.0f)),
                    static_cast<float>(std::floor(
                        typeCenterPosition.Y - textSize.Y / 2.0f))),
                typeColor);

            DrawData(gamePadStates[selectedPlayer], gamePadCapabilities[selectedPlayer]);

            spriteBatch->DrawString(
                deadZoneSwitch.getActiveProperty()
                    ? *instructionsActiveFont
                    : *instructionsFont,
                InputReporterResources::DeadZoneInstructions,
                deadZoneInstructionsPosition,
                instructionsColor);
            spriteBatch->DrawString(
                *instructionsFont,
                deadZoneString,
                deadZoneStringPosition,
                instructionsColor);
            spriteBatch->DrawString(
                exitSwitch.getActiveProperty()
                    ? *instructionsActiveFont
                    : *instructionsFont,
                InputReporterResources::ExitInstructions,
                exitInstructionsPosition,
                instructionsColor);

            spriteBatch->End();
        }

    private:
        [[nodiscard]] static bool IsActiveGamePad(const GamePadState& gamePadState)
        {
            return gamePadState.getIsConnectedProperty() &&
                (gamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getYProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getStartProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getLeftShoulderProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getRightShoulderProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getLeftStickProperty() == ButtonState::Pressed ||
                 gamePadState.getButtonsProperty().getRightStickProperty() == ButtonState::Pressed ||
                 gamePadState.getDPadProperty().getUpProperty() == ButtonState::Pressed ||
                 gamePadState.getDPadProperty().getLeftProperty() == ButtonState::Pressed ||
                 gamePadState.getDPadProperty().getRightProperty() == ButtonState::Pressed ||
                 gamePadState.getDPadProperty().getDownProperty() == ButtonState::Pressed);
        }

        void DrawData(
            const GamePadState& gamePadState,
            const GamePadCapabilities& gamePadCapabilities)
        {
            Vector2 descriptionPosition = descriptionColumn1Position;
            Vector2 valuePosition = valueColumn1Position;

            DrawValue(
                InputReporterResources::LeftThumbstickX,
                descriptionPosition,
                FormatThreeDecimals(gamePadState.getThumbSticksProperty().getLeftProperty().X),
                valuePosition,
                gamePadCapabilities.getHasLeftXThumbStickProperty(),
                gamePadState.getThumbSticksProperty().getLeftProperty().X != 0.0f);
            DrawValue(
                InputReporterResources::LeftThumbstickY,
                descriptionPosition,
                FormatThreeDecimals(gamePadState.getThumbSticksProperty().getLeftProperty().Y),
                valuePosition,
                gamePadCapabilities.getHasLeftYThumbStickProperty(),
                gamePadState.getThumbSticksProperty().getLeftProperty().Y != 0.0f);

            DrawValue(
                InputReporterResources::RightThumbstickX,
                descriptionPosition,
                FormatThreeDecimals(gamePadState.getThumbSticksProperty().getRightProperty().X),
                valuePosition,
                gamePadCapabilities.getHasRightXThumbStickProperty(),
                gamePadState.getThumbSticksProperty().getRightProperty().X != 0.0f);
            DrawValue(
                InputReporterResources::RightThumbstickY,
                descriptionPosition,
                FormatThreeDecimals(gamePadState.getThumbSticksProperty().getRightProperty().Y),
                valuePosition,
                gamePadCapabilities.getHasRightYThumbStickProperty(),
                gamePadState.getThumbSticksProperty().getRightProperty().Y != 0.0f);

            descriptionPosition.Y += dataSpacing;
            valuePosition.Y += dataSpacing;

            DrawValue(
                InputReporterResources::LeftTrigger,
                descriptionPosition,
                FormatThreeDecimals(gamePadState.getTriggersProperty().getLeftProperty()),
                valuePosition,
                gamePadCapabilities.getHasLeftTriggerProperty(),
                gamePadState.getTriggersProperty().getLeftProperty() != 0.0f);
            DrawValue(
                InputReporterResources::RightTrigger,
                descriptionPosition,
                FormatThreeDecimals(gamePadState.getTriggersProperty().getRightProperty()),
                valuePosition,
                gamePadCapabilities.getHasRightTriggerProperty(),
                gamePadState.getTriggersProperty().getRightProperty() != 0.0f);

            descriptionPosition.Y += dataSpacing;
            valuePosition.Y += dataSpacing;

            DrawValue(
                InputReporterResources::DPadUp,
                descriptionPosition,
                ButtonStateName(gamePadState.getDPadProperty().getUpProperty()),
                valuePosition,
                gamePadCapabilities.getHasDPadUpButtonProperty(),
                gamePadState.getDPadProperty().getUpProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::DPadDown,
                descriptionPosition,
                ButtonStateName(gamePadState.getDPadProperty().getDownProperty()),
                valuePosition,
                gamePadCapabilities.getHasDPadDownButtonProperty(),
                gamePadState.getDPadProperty().getDownProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::DPadLeft,
                descriptionPosition,
                ButtonStateName(gamePadState.getDPadProperty().getLeftProperty()),
                valuePosition,
                gamePadCapabilities.getHasDPadLeftButtonProperty(),
                gamePadState.getDPadProperty().getLeftProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::DPadRight,
                descriptionPosition,
                ButtonStateName(gamePadState.getDPadProperty().getRightProperty()),
                valuePosition,
                gamePadCapabilities.getHasDPadRightButtonProperty(),
                gamePadState.getDPadProperty().getRightProperty() == ButtonState::Pressed);

            descriptionPosition.Y += dataSpacing;
            valuePosition.Y += dataSpacing;

            if (gamePadCapabilities.getHasLeftVibrationMotorProperty())
            {
                if (gamePadCapabilities.getHasRightVibrationMotorProperty())
                {
                    spriteBatch->DrawString(
                        *dataFont,
                        InputReporterResources::BothVibrationMotors,
                        descriptionPosition,
                        descriptionColor);
                }
                else
                {
                    spriteBatch->DrawString(
                        *dataFont,
                        InputReporterResources::LeftVibrationMotor,
                        descriptionPosition,
                        descriptionColor);
                }
            }
            else if (gamePadCapabilities.getHasRightVibrationMotorProperty())
            {
                spriteBatch->DrawString(
                    *dataFont,
                    InputReporterResources::RightVibrationMotor,
                    descriptionPosition,
                    descriptionColor);
            }
            else
            {
                spriteBatch->DrawString(
                    *dataFont,
                    InputReporterResources::NoVibration,
                    descriptionPosition,
                    descriptionColor);
            }

            descriptionPosition = descriptionColumn2Position;
            valuePosition = valueColumn2Position;

            DrawValue(
                InputReporterResources::A,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getAProperty()),
                valuePosition,
                gamePadCapabilities.getHasAButtonProperty(),
                gamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::B,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getBProperty()),
                valuePosition,
                gamePadCapabilities.getHasBButtonProperty(),
                gamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::X,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getXProperty()),
                valuePosition,
                gamePadCapabilities.getHasXButtonProperty(),
                gamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::Y,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getYProperty()),
                valuePosition,
                gamePadCapabilities.getHasYButtonProperty(),
                gamePadState.getButtonsProperty().getYProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::LeftShoulder,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getLeftShoulderProperty()),
                valuePosition,
                gamePadCapabilities.getHasLeftShoulderButtonProperty(),
                gamePadState.getButtonsProperty().getLeftShoulderProperty() ==
                    ButtonState::Pressed);
            DrawValue(
                InputReporterResources::RightShoulder,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getRightShoulderProperty()),
                valuePosition,
                gamePadCapabilities.getHasRightShoulderButtonProperty(),
                gamePadState.getButtonsProperty().getRightShoulderProperty() ==
                    ButtonState::Pressed);
            DrawValue(
                InputReporterResources::LeftStick,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getLeftStickProperty()),
                valuePosition,
                gamePadCapabilities.getHasLeftStickButtonProperty(),
                gamePadState.getButtonsProperty().getLeftStickProperty() ==
                    ButtonState::Pressed);
            DrawValue(
                InputReporterResources::RightStick,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getRightStickProperty()),
                valuePosition,
                gamePadCapabilities.getHasRightStickButtonProperty(),
                gamePadState.getButtonsProperty().getRightStickProperty() ==
                    ButtonState::Pressed);
            DrawValue(
                InputReporterResources::Start,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getStartProperty()),
                valuePosition,
                gamePadCapabilities.getHasStartButtonProperty(),
                gamePadState.getButtonsProperty().getStartProperty() == ButtonState::Pressed);
            DrawValue(
                InputReporterResources::Back,
                descriptionPosition,
                ButtonStateName(gamePadState.getButtonsProperty().getBackProperty()),
                valuePosition,
                gamePadCapabilities.getHasBackButtonProperty(),
                gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed);

            descriptionPosition.Y += dataSpacing;
            valuePosition.Y += dataSpacing;

            DrawValue(
                InputReporterResources::PacketNumber,
                descriptionPosition,
                std::to_string(gamePadState.getPacketNumberProperty()),
                valuePosition,
                gamePadCapabilities.getIsConnectedProperty(),
                false);
        }

        void DrawValue(
            const std::string& description,
            Vector2& descriptionPosition,
            const std::string& value,
            Vector2& valuePosition,
            bool enabled,
            bool active)
        {
            spriteBatch->DrawString(
                *dataFont,
                description,
                descriptionPosition,
                enabled ? descriptionColor : disabledColor);
            descriptionPosition.Y += dataSpacing;
            spriteBatch->DrawString(
                active ? *dataActiveFont : *dataFont,
                value,
                valuePosition,
                enabled ? valueColor : disabledColor);
            valuePosition.Y += dataSpacing;
        }

        void ToggleDeadZone()
        {
            switch (getDeadZoneProperty())
            {
                case GamePadDeadZone::IndependentAxes:
                    setDeadZoneProperty(GamePadDeadZone::Circular);
                    break;
                case GamePadDeadZone::Circular:
                    setDeadZoneProperty(GamePadDeadZone::None);
                    break;
                case GamePadDeadZone::None:
                    setDeadZoneProperty(GamePadDeadZone::IndependentAxes);
                    break;
            }
        }

        void exitSwitch_Fire()
        {
            Exit();
        }

        [[nodiscard]] static const char* ButtonStateName(ButtonState state)
        {
            return state == ButtonState::Pressed
                ? InputReporterResources::ButtonPressed
                : InputReporterResources::ButtonReleased;
        }

        [[nodiscard]] static std::string FormatThreeDecimals(float value)
        {
            char result[32];
            std::snprintf(
                result,
                sizeof(result),
                "%.3f",
                static_cast<double>(value == 0.0f ? 0.0f : value));
            return result;
        }

        [[nodiscard]] static const char* PlayerIndexName(PlayerIndex value)
        {
            switch (value)
            {
                case PlayerIndex::One: return "One";
                case PlayerIndex::Two: return "Two";
                case PlayerIndex::Three: return "Three";
                case PlayerIndex::Four: return "Four";
            }
            return "Unknown";
        }

        [[nodiscard]] static const char* DeadZoneName(GamePadDeadZone value)
        {
            switch (value)
            {
                case GamePadDeadZone::None: return "None";
                case GamePadDeadZone::IndependentAxes: return "IndependentAxes";
                case GamePadDeadZone::Circular: return "Circular";
            }
            return "Unknown";
        }

        [[nodiscard]] static const char* GamePadTypeName(GamePadType value)
        {
            switch (value)
            {
                case GamePadType::Unknown: return "Unknown";
                case GamePadType::GamePad: return "GamePad";
                case GamePadType::Wheel: return "Wheel";
                case GamePadType::ArcadeStick: return "ArcadeStick";
                case GamePadType::FlightStick: return "FlightStick";
                case GamePadType::DancePad: return "DancePad";
                case GamePadType::Guitar: return "Guitar";
                case GamePadType::AlternateGuitar: return "AlternateGuitar";
                case GamePadType::DrumKit: return "DrumKit";
                case GamePadType::BigButtonPad: return "BigButtonPad";
            }
            return "Unknown";
        }
    };
}
