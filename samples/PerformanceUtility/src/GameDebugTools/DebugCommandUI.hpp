// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "CNA/CNAHelper.hpp"

// DebugCommandUI.hpp — C++ port of GameDebugTools/DebugCommandUI.cs (XNA 4.0
// PerformanceUtility sample). An in-game command console: type commands with
// the keyboard, toggle open/closed with Tab.
//
#include <algorithm>
#include <cctype>
#include <deque>
#include <memory>
#include <stdexcept>
#include <string>
#include "System/Collections/Generic/Dictionary.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Collections/Generic/Queue.hpp"
#include "System/Collections/Generic/Stack.hpp"
#include "System/String.hpp"
#include "System/Exception.hpp"
#include <vector>

#include "System/InvalidOperationException.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "DebugManager.hpp"
#include "IDebugCommandHost.hpp"
#include "KeyboardUtils.hpp"

namespace PerformanceUtility::GameDebugTools {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;

// Command window for debug purposes. Type commands with the keyboard; press
// Tab to open/close. Port of GameDebugTools/DebugCommandUI.cs.
class DebugCommandUI : public DrawableGameComponent, public IDebugCommandHost {
public:
    /** @brief Returns the original managed component identity. */
    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "PerformanceUtility.GameDebugTools.DebugCommandUI";
        return name;
    }

    static constexpr const char* DefaultPrompt = "CMD>";

    [[nodiscard]] const std::string& getPromptProperty() const { return prompt_; }
    void setPromptProperty(const std::string& value) { prompt_ = value; }

    [[nodiscard]] bool getFocusedProperty() const { return state_ != State::Closed; }

    explicit DebugCommandUI(Game& game) : DrawableGameComponent(game) {
        game.getServicesProperty().AddService<IDebugCommandHost>(this);

        // Draw the command UI on top of everything.
        setDrawOrderProperty(0x7fffffff);

        RegisterCommand("help", "Show Command helps",
            [this](IDebugCommandHost&, const std::string&, const System::Collections::Generic::IList<std::string>&) {
                int maxLen = 0;
                for (const auto& info : commandTable_.getValuesProperty())
                    maxLen = std::max(maxLen, static_cast<int>(info.command.size()));
                const std::string format = System::String::Format("{{0,-{0}}}    {{1}}", maxLen);
                for (const auto& info : commandTable_.getValuesProperty())
                    Echo(System::String::Format(format, info.command, info.description));
            });

        RegisterCommand("cls", "Clear Screen",
            [this](IDebugCommandHost&, const std::string&, const System::Collections::Generic::IList<std::string>&) {
                lines_.Clear();
            });

        RegisterCommand("echo", "Display Messages",
            [this](IDebugCommandHost&, const std::string& command, const System::Collections::Generic::IList<std::string>&) {
                Echo(System::String::Substring(command, 5));
            });
    }

    void Initialize() override {
        debugManager_ = getGameProperty().getServicesProperty().GetService<DebugManager>();
        if (debugManager_ == nullptr)
            throw System::InvalidOperationException("Coudn't find DebugManager.");

        DrawableGameComponent::Initialize();
    }

    // ---- IDebugCommandHost ----

    void RegisterCommand(const std::string& command, const std::string& description,
                          DebugCommandExecute callback) override {
        const std::string lower = System::String::ToLower(command);
        if (commandTable_.ContainsKey(lower))
            throw System::InvalidOperationException("Command \"" + command + "\" is already registered.");

        commandTable_.Add(lower, CommandInfo{command, description, std::move(callback)});
    }

    void UnregisterCommand(const std::string& command) override {
        const std::string lower = System::String::ToLower(command);
        if (!commandTable_.ContainsKey(lower))
            throw System::InvalidOperationException("Command \"" + command + "\" is not registered.");
        commandTable_.Remove(command);
    }

    void ExecuteCommand(const std::string& commandIn) override {
        if (executioners_.getCountProperty() != 0) {
            executioners_.Peek()->ExecuteCommand(commandIn);
            return;
        }

        Echo(prompt_ + commandIn);

        const std::string command = System::String::TrimStart(commandIn, {' '});
        System::Collections::Generic::List<std::string> args(System::String::Split(command, ' '));
        const std::string cmdText = args.getItem(0);
        args.RemoveAt(0);

        CommandInfo info;
        if (commandTable_.TryGetValue(System::String::ToLower(cmdText), info)) {
            try {
                info.callback(*this, command, args);
            } catch (const System::Exception& e) {
                EchoError("Unhandled Exception occurred");
                for (const std::string& line : System::String::Split(e.getMessageProperty(), '\n'))
                    EchoError(line);
            }
        } else {
            Echo("Unknown Command");
        }

        commandHistory_.Add(command);
        while (commandHistory_.getCountProperty() > MaxCommandHistory)
            commandHistory_.RemoveAt(0);

        commandHistoryIndex_ = commandHistory_.getCountProperty();
    }

    void RegisterEchoListner(IDebugEchoListner* listner) override { listeners_.Add(listner); }

    void UnregisterEchoListner(IDebugEchoListner* listner) override {
        listeners_.Remove(listner);
    }

    void Echo(DebugCommandMessage messageType, const std::string& text) override {
        lines_.Enqueue(text);
        while (lines_.getCountProperty() >= MaxLineCount)
            lines_.Dequeue();

        for (IDebugEchoListner* listner : listeners_)
            listner->Echo(messageType, text);
    }

    void Echo(const std::string& text) override { Echo(DebugCommandMessage::Standard, text); }
    void EchoWarning(const std::string& text) override { Echo(DebugCommandMessage::Warning, text); }
    void EchoError(const std::string& text) override { Echo(DebugCommandMessage::Error, text); }

    void PushExecutioner(IDebugCommandExecutioner* executioner) override { executioners_.Push(executioner); }
    void PopExecutioner() override { executioners_.Pop(); }

    // ---- Update and Draw ----

    void Show() {
        if (state_ == State::Closed) {
            stateTransition_ = 0.0f;
            state_ = State::Opening;
        }
    }

    void Hide() {
        if (state_ == State::Opened) {
            stateTransition_ = 1.0f;
            state_ = State::Closing;
        }
    }

    void Update(GameTime& gameTime) override {
        KeyboardState keyState = Keyboard::GetState();

        float dt = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        const float OpenSpeed = 8.0f;
        const float CloseSpeed = 8.0f;

        switch (state_) {
            case State::Closed:
                if (keyState.IsKeyDown(Keys::Tab))
                    Show();
                break;
            case State::Opening:
                stateTransition_ += dt * OpenSpeed;
                if (stateTransition_ > 1.0f) {
                    stateTransition_ = 1.0f;
                    state_ = State::Opened;
                }
                break;
            case State::Opened:
                ProcessKeyInputs(dt);
                break;
            case State::Closing:
                stateTransition_ -= dt * CloseSpeed;
                if (stateTransition_ < 0.0f) {
                    stateTransition_ = 0.0f;
                    state_ = State::Closed;
                }
                break;
        }

        prevKeyState_ = keyState;

        DrawableGameComponent::Update(gameTime);
    }

    void ProcessKeyInputs(float dt) {
        KeyboardState keyState = Keyboard::GetState();
        std::vector<Keys> keys = keyState.GetPressedKeys();

        bool shift = keyState.IsKeyDown(Keys::LeftShift) || keyState.IsKeyDown(Keys::RightShift);

        for (Keys key : keys) {
            if (!IsKeyPressed(key, dt)) continue;

            char ch;
            if (KeyboardUtils::KeyToString(key, shift, ch)) {
                commandLine_ = System::String::Insert(commandLine_, cursorIndex_, std::string(1, ch));
                cursorIndex_++;
            } else {
                switch (key) {
                    case Keys::Back:
                        if (cursorIndex_ > 0)
                            commandLine_ = System::String::Remove(commandLine_, --cursorIndex_, 1);
                        break;
                    case Keys::Delete:
                        if (cursorIndex_ < (int)commandLine_.size())
                            commandLine_ = System::String::Remove(commandLine_, cursorIndex_, 1);
                        break;
                    case Keys::Left:
                        if (cursorIndex_ > 0)
                            cursorIndex_--;
                        break;
                    case Keys::Right:
                        if (cursorIndex_ < (int)commandLine_.size())
                            cursorIndex_++;
                        break;
                    case Keys::Enter:
                        ExecuteCommand(commandLine_);
                        commandLine_.clear();
                        cursorIndex_ = 0;
                        break;
                    case Keys::Up:
                        if (commandHistory_.getCountProperty() != 0) {
                            commandHistoryIndex_ = std::max(0, commandHistoryIndex_ - 1);
                            commandLine_ = commandHistory_.getItem(commandHistoryIndex_);
                            cursorIndex_ = (int)commandLine_.size();
                        }
                        break;
                    case Keys::Down:
                        if (commandHistory_.getCountProperty() != 0) {
                            commandHistoryIndex_ =
                                std::min(commandHistory_.getCountProperty() - 1, commandHistoryIndex_ + 1);
                            commandLine_ = commandHistory_.getItem(commandHistoryIndex_);
                            cursorIndex_ = (int)commandLine_.size();
                        }
                        break;
                    case Keys::Tab:
                        Hide();
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void Draw(const GameTime&) override {
        if (state_ == State::Closed)
            return;

        SpriteFont& font = debugManager_->getDebugFontProperty();
        SpriteBatch& spriteBatch = debugManager_->getSpriteBatchProperty();

        float w = (float)getGraphicsDeviceProperty().getViewportProperty().getWidthProperty();
        float h = (float)getGraphicsDeviceProperty().getViewportProperty().getHeightProperty();
        float topMargin = h * 0.1f;
        float leftMargin = w * 0.1f;

        Rectangle rect;
        rect.X = (int)leftMargin;
        rect.Y = (int)topMargin;
        rect.Width = (int)(w * 0.8f);
        rect.Height = (int)((float)MaxLineCount * (float)font.getLineSpacingProperty());

        Matrix mtx = Matrix::CreateTranslation(Vector3(0.0f, -(float)rect.Height * (1.0f - stateTransition_), 0.0f));

        spriteBatch.Begin(SpriteSortMode::Deferred, nullptr, nullptr, nullptr, nullptr, nullptr, mtx);

        spriteBatch.Draw(debugManager_->getWhiteTextureProperty(), rect, Color(0, 0, 0, 200));

        Vector2 pos(leftMargin, topMargin);
        for (const std::string& line : lines_) {
            spriteBatch.DrawString(font, line, pos, Color::White);
            pos.Y += (float)font.getLineSpacingProperty();
        }

        std::string leftPart = prompt_ + System::String::Substring(commandLine_, 0, cursorIndex_);
        Vector2 cursorPos = pos + font.MeasureString(leftPart);
        cursorPos.Y = pos.Y;

        spriteBatch.DrawString(font, prompt_ + commandLine_, pos, Color::White);
        spriteBatch.DrawString(font, Cursor, cursorPos, Color::White);

        spriteBatch.End();
    }

private:
    static constexpr int MaxLineCount = 20;
    static constexpr int MaxCommandHistory = 32;
    static constexpr const char* Cursor = "_";

    enum class State { Closed, Opening, Opened, Closing };

    struct CommandInfo {
        std::string command;
        std::string description;
        DebugCommandExecute callback;
    };

    bool IsKeyPressed(Keys key, float dt) {
        if (prevKeyState_.IsKeyUp(key)) {
            keyRepeatTimer_ = keyRepeatStartDuration_;
            pressedKey_ = key;
            return true;
        }

        if (key == pressedKey_) {
            keyRepeatTimer_ -= dt;
            if (keyRepeatTimer_ <= 0.0f) {
                keyRepeatTimer_ += keyRepeatDuration_;
                return true;
            }
        }

        return false;
    }

    DebugManager* debugManager_ = nullptr;

    State state_ = State::Closed;
    float stateTransition_ = 0.0f;

    System::Collections::Generic::List<IDebugEchoListner*> listeners_;
    System::Collections::Generic::Stack<IDebugCommandExecutioner*> executioners_;
    System::Collections::Generic::Dictionary<std::string, CommandInfo> commandTable_;

    std::string commandLine_;
    std::string prompt_ = DefaultPrompt;
    int cursorIndex_ = 0;

    System::Collections::Generic::Queue<std::string> lines_;
    System::Collections::Generic::List<std::string> commandHistory_;
    int commandHistoryIndex_ = 0;

    KeyboardState prevKeyState_;
    Keys pressedKey_ = Keys::None;
    float keyRepeatTimer_ = 0.0f;
    float keyRepeatStartDuration_ = 0.3f;
    float keyRepeatDuration_ = 0.03f;
};

} // namespace PerformanceUtility::GameDebugTools
