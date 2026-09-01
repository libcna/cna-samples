// SPDX-License-Identifier: MS-PL

#include "PerformanceUtilityGame.hpp"

#include <any>

#include "System/IAsyncResult.hpp"
#include "System/Globalization/CultureInfo.hpp"
#include "System/Single.hpp"
#include "System/String.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

#include "GameDebugTools/Layout.hpp"

namespace PerformanceUtility
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::GamerServices::Guide;
    using Microsoft::Xna::Framework::Input::Buttons;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;
    using Microsoft::Xna::Framework::Input::Touch::GestureType;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
    using GameDebugTools::Alignment;
    using GameDebugTools::DebugSystem;
    using GameDebugTools::Layout;

    PerformanceUtilityGame::PerformanceUtilityGame()
        : graphics_(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics_.setIsFullScreenProperty(true);
#endif
    }

    PerformanceUtilityGame::~PerformanceUtilityGame()
    {
        DebugSystem::Shutdown();
    }

    const std::string& PerformanceUtilityGame::GetTypeName() const
    {
        static const std::string name = "PerformanceUtility.PerformanceUtilityGame";
        return name;
    }

    void PerformanceUtilityGame::Initialize()
    {
        debugSystem_ = &DebugSystem::Initialize(*this, "Font");
        debugSystem_->getDebugCommandUIProperty().RegisterCommand(
            "pos",
            "set position",
            [this](GameDebugTools::IDebugCommandHost& host,
                   const std::string& command,
                   const std::vector<std::string>& arguments)
            {
                PosCommand(host, command, arguments);
            });

        TouchPanel::setEnabledGesturesProperty(GestureType::Flick | GestureType::Tap);

        Game::Initialize();
    }

    void PerformanceUtilityGame::LoadContent()
    {
        spriteBatch_ = std::make_unique<Microsoft::Xna::Framework::Graphics::SpriteBatch>(
            getGraphicsDeviceProperty());
        font_.emplace(getContentProperty().Load<Microsoft::Xna::Framework::Graphics::SpriteFont>("Font"));
        cat_.emplace(getContentProperty().Load<Microsoft::Xna::Framework::Graphics::Texture2D>("cat"));

        blank_.emplace(getGraphicsDeviceProperty(), 1, 1);
        const Color white = Color::White;
        blank_->SetData(&white, 1);

        Game::LoadContent();
    }

    void PerformanceUtilityGame::PosCommand(
        GameDebugTools::IDebugCommandHost& host,
        const std::string&,
        const std::vector<std::string>& arguments)
    {
        if (arguments.size() == 2)
        {
            const auto& invariantCulture =
                System::Globalization::CultureInfo::getInvariantCultureProperty();
            debugPos_.X = System::Single::Parse(arguments[0], &invariantCulture);
            debugPos_.Y = System::Single::Parse(arguments[1], &invariantCulture);
        }
        else
        {
            host.Echo(System::String::Format("Pos={0},{1}", debugPos_.X, debugPos_.Y));
        }
    }

    void PerformanceUtilityGame::Update(Microsoft::Xna::Framework::GameTime& gameTime)
    {
        debugSystem_->getTimeRulerProperty().StartFrame();
        debugSystem_->getTimeRulerProperty().BeginMark("Update", Color::Blue);

        HandleInput();
        HandleTouchInput();

        stopwatch_.Reset();
        stopwatch_.Start();
        while (stopwatch_.getElapsedMillisecondsProperty() < 1)
        {
        }

        Game::Update(gameTime);

        debugSystem_->getTimeRulerProperty().EndMark("Update");
    }

    void PerformanceUtilityGame::Draw(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        debugSystem_->getTimeRulerProperty().BeginMark("Draw", Color::Yellow);

        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        spriteBatch_->Begin();

        const std::string message =
            "A Button, A key: Show/Hide FPS Counter\n"
            "B Button, B key: Show/Hide Time Ruler\n"
            "X Button, X key: Show/Hide Time Ruler Log\n"
            "Tab key, flick down: Open debug command UI\n"
            "Tab key, flick up: Close debug command UI\n"
            "Tap: Show keyboard input panel";

        Vector2 size = font_->MeasureString(message);
        Layout layout(getGraphicsDeviceProperty().getViewportProperty());

        const float margin = static_cast<float>(font_->getLineSpacingProperty());
        Rectangle region(0, 0,
                         static_cast<int>(size.X + margin),
                         static_cast<int>(size.Y + margin));

        region = layout.Place(region, 0.01f, 0.01f, Alignment::TopRight);
        spriteBatch_->Draw(*blank_, region, Color::Black * 0.5f);

        layout.ClientArea = region;
        const Vector2 position = layout.Place(size, 0.0f, 0.0f, Alignment::Center);
        spriteBatch_->DrawString(*font_, message, position, Color::White);
        spriteBatch_->Draw(*cat_, debugPos_, Color::White);

        spriteBatch_->End();

        Game::Draw(gameTime);

        debugSystem_->getTimeRulerProperty().EndMark("Draw");
    }

    void PerformanceUtilityGame::HandleInput()
    {
        padState_ = GamePad::GetState(PlayerIndex::One);
        keyState_ = Keyboard::GetState();

        if (padState_.IsButtonDown(Buttons::Back) || keyState_.IsKeyDown(Keys::Escape))
            Exit();

        if (debugSystem_->getDebugCommandUIProperty().getFocusedProperty())
            keyState_ = KeyboardState{};

        if (IsButtonOrKeyPressed(Buttons::A, Keys::A))
        {
            auto& fpsCounter = debugSystem_->getFpsCounterProperty();
            fpsCounter.setVisibleProperty(!fpsCounter.getVisibleProperty());
        }

        if (IsButtonOrKeyPressed(Buttons::B, Keys::B))
        {
            auto& timeRuler = debugSystem_->getTimeRulerProperty();
            timeRuler.setVisibleProperty(!timeRuler.getVisibleProperty());
        }

        if (IsButtonOrKeyPressed(Buttons::X, Keys::X))
        {
            auto& timeRuler = debugSystem_->getTimeRulerProperty();
            timeRuler.setVisibleProperty(true);
            timeRuler.setShowLogProperty(!timeRuler.getShowLogProperty());
        }

        prevPadState_ = padState_;
        prevKeyState_ = keyState_;
    }

    bool PerformanceUtilityGame::IsButtonOrKeyPressed(Buttons button, Keys key) const
    {
        return (padState_.IsButtonDown(button) && prevPadState_.IsButtonUp(button)) ||
               (keyState_.IsKeyDown(key) && prevKeyState_.IsKeyUp(key));
    }

    void PerformanceUtilityGame::HandleTouchInput()
    {
        while (TouchPanel::getIsGestureAvailableProperty())
        {
            const GestureSample gesture = TouchPanel::ReadGesture();

            switch (gesture.getGestureTypeProperty())
            {
                case GestureType::Tap:
                    if (debugSystem_->getDebugCommandUIProperty().getFocusedProperty() &&
                        !Guide::getIsVisibleProperty())
                    {
                        [[maybe_unused]] System::IAsyncResult* keyboardInput =
                            Guide::BeginShowKeyboardInput(
                                PlayerIndex::One,
                                "Input Debug Command",
                                "type debug command\n'help' command shows available commands",
                                "",
                                [this](System::IAsyncResult& result)
                                {
                                    InputDebugCommandCallback(result);
                                },
                                std::any{});
                    }
                    break;

                case GestureType::Flick:
                    if (gesture.getDeltaProperty().Length() > 5.0f)
                    {
                        constexpr float Cos30 = 0.87f;
                        const Vector2 normalizedDelta = Vector2::Normalize(gesture.getDeltaProperty());
                        const float dot = -normalizedDelta.Y;

                        if (debugSystem_->getDebugCommandUIProperty().getFocusedProperty())
                        {
                            if (dot > Cos30)
                                debugSystem_->getDebugCommandUIProperty().Hide();
                        }
                        else if (dot < -Cos30)
                        {
                            debugSystem_->getDebugCommandUIProperty().Show();
                        }
                    }
                    break;

                default:
                    break;
            }
        }
    }

    void PerformanceUtilityGame::InputDebugCommandCallback(System::IAsyncResult& result)
    {
        const std::string command = Guide::EndShowKeyboardInput(&result);

        if (!System::String::IsNullOrEmpty(command))
            debugSystem_->getDebugCommandUIProperty().ExecuteCommand(command);

        delete &result;
    }
}
