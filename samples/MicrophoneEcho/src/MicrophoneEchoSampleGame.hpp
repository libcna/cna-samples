// SPDX-License-Identifier: MS-PL
#pragma once

#include <CNA/CNAHelper.hpp>
#include <Microsoft/Xna/Framework/Game.hpp>
#include <Microsoft/Xna/Framework/GraphicsDeviceManager.hpp>
#include <Microsoft/Xna/Framework/GameTime.hpp>
#include <Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp>
#include <Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp>
#include <Microsoft/Xna/Framework/Graphics/SpriteFont.hpp>
#include <Microsoft/Xna/Framework/Graphics/BasicEffect.hpp>
#include <Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp>
#include <Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp>
#include <Microsoft/Xna/Framework/Graphics/BlendState.hpp>
#include <Microsoft/Xna/Framework/Input/Keyboard.hpp>
#include <Microsoft/Xna/Framework/Input/KeyboardState.hpp>
#include <Microsoft/Xna/Framework/Input/GamePad.hpp>
#include <Microsoft/Xna/Framework/Input/GamePadState.hpp>
#include <Microsoft/Xna/Framework/Input/Keys.hpp>
#include <Microsoft/Xna/Framework/Input/Buttons.hpp>
#include <Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp>
#include <Microsoft/Xna/Framework/Input/Touch/GestureType.hpp>
#include <Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp>
#include <Microsoft/Xna/Framework/PlayerIndex.hpp>
#include <Microsoft/Xna/Framework/Audio/Microphone.hpp>
#include <Microsoft/Xna/Framework/Audio/MicrophoneState.hpp>
#include <Microsoft/Xna/Framework/Audio/NoMicrophoneConnectedException.hpp>
#include <Microsoft/Xna/Framework/Audio/DynamicSoundEffectInstance.hpp>
#include <Microsoft/Xna/Framework/Audio/AudioChannels.hpp>
#include <Microsoft/Xna/Framework/Vector2.hpp>
#include <Microsoft/Xna/Framework/Vector3.hpp>
#include <Microsoft/Xna/Framework/Matrix.hpp>
#include <Microsoft/Xna/Framework/Color.hpp>
#include <System/TimeSpan.hpp>
#include <System/BitConverter.hpp>
#include <System/ArgumentException.hpp>
#include <System/ArgumentOutOfRangeException.hpp>
#include <System/IndexOutOfRangeException.hpp>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;
using namespace Microsoft::Xna::Framework::Input::Touch;
using namespace Microsoft::Xna::Framework::Audio;

namespace MicrophoneEchoSample {

class MicrophoneExtensions final {
public:
    static bool IsConnected(Microphone& microphone) {
        try {
            [[maybe_unused]] MicrophoneState state = microphone.getStateProperty();
            return true;
        } catch (const NoMicrophoneConnectedException&) {
            return false;
        }
    }
};

class MicrophoneEchoSampleGame : public Game {
public:
    MicrophoneEchoSampleGame() {
        graphics_ = std::make_unique<GraphicsDeviceManager>(this);

#if defined(WINDOWS_PHONE)
        graphics_->setIsFullScreenProperty(true);
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
#endif

        graphics_->setPreferredBackBufferWidthProperty(800);
        graphics_->setPreferredBackBufferHeightProperty(480);
        graphics_->setSupportedOrientationsProperty(DisplayOrientation::LandscapeLeft);

        TouchPanel::setEnabledGesturesProperty(GestureType::Tap | GestureType::DoubleTap);
        setIsMouseVisibleProperty(true);
        getContentProperty().setRootDirectoryProperty("Content");
    }

    ~MicrophoneEchoSampleGame() override {
        if (activeMicrophone_ != nullptr && bufferReadyToken_.has_value()) {
            activeMicrophone_->BufferReady.Remove(*bufferReadyToken_);
        }
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "MicrophoneEchoSample.MicrophoneEchoSampleGame";
        return name;
    }

protected:
    void LoadContent() override {
        spriteBatch_.emplace(getGraphicsDeviceProperty());
        font_.emplace(getContentProperty().Load<SpriteFont>("MyFont"));
        InitializeMicrophone();
    }

    void Update(GameTime& gameTime) override {
        // Picks a microphone to start recording - if one isn't picked already.
        InitializeMicrophone();
        // Handle input to start/stop recording.
        HandleInput();
        // Check and update microphone status.
        UpdateMicrophoneStatus();

        Game::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        DrawWaveform();
        Game::Draw(gameTime);
    }

private:
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::optional<SpriteBatch> spriteBatch_;
    std::optional<SpriteFont>  font_;

    // The most recent microphone samples.
    std::vector<SharpRuntime::bytecs> micSamples_;
    // A circular buffer that we feedback into from micSamples_.
    std::vector<SharpRuntime::bytecs> echoBuffer_;
    // Tracks the position into the echo buffer.
    int echoBufferPosition_ = 0;
    // Used to playback the captured audio after processing it for echo.
    std::unique_ptr<DynamicSoundEffectInstance> dynamicSound_;
    // Microphone used for recording (owned by CNA's Microphone::getAllProperty() storage).
    Microphone* activeMicrophone_ = nullptr;
    std::optional<System::EventHandler<System::EventArgs>::Token> bufferReadyToken_;
    // Follow these instructions.
#if defined(WINDOWS_PHONE)
    static constexpr const char* instructions = "Tap to start or DoubleTap to stop recording";
#else
    static constexpr const char* instructions = "Press 'A' to start and 'B' to stop recording";
#endif

    KeyboardState currentKeyboardState_;
    KeyboardState previousKeyboardState_;
    GamePadState  currentGamePadState_;
    GamePadState  previousGamePadState_;

    // Used to communicate the microphone status to the user.
    std::string microphoneStatus_;

    // Echo processing constants.
    // Delay applied in seconds.
    static constexpr float echoDelay = 0.15f;
    // Rate of echo decay.
    static constexpr float echoAmount = 0.5f;

    // On big endian systems audio samples need to be swapped because
    // the byte buffer is always written/read little-endian here.
    bool bigEndian_ = !System::BitConverter::IsLittleEndian;

    // Used for drawing the audio waveform.
    std::optional<BasicEffect> effect_;
    std::vector<VertexPositionColor> vertexPosColor_;

    // Handles input for starting and stopping the recording.
    void HandleInput() {
        // Allows the game to exit.
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
            Keyboard::GetState().IsKeyDown(Keys::Escape)) {
            Exit();
        }

        if (TouchPanel::getIsGestureAvailableProperty()) {
            GestureSample gesture = TouchPanel::ReadGesture();
            if (gesture.getGestureTypeProperty() == GestureType::Tap) {
                StartMicrophone();
            } else if (gesture.getGestureTypeProperty() == GestureType::DoubleTap) {
                StopMicrophone();
            }
        } else {
            previousGamePadState_ = currentGamePadState_;
            previousKeyboardState_ = currentKeyboardState_;

            currentGamePadState_ = GamePad::GetState(PlayerIndex::One);
            currentKeyboardState_ = Keyboard::GetState();

            if ((currentGamePadState_.IsButtonDown(Buttons::A) && previousGamePadState_.IsButtonUp(Buttons::A)) ||
                (currentKeyboardState_.IsKeyDown(Keys::A) && previousKeyboardState_.IsKeyUp(Keys::A))) {
                StartMicrophone();
            }

            if ((currentGamePadState_.IsButtonDown(Buttons::B) && previousGamePadState_.IsButtonUp(Buttons::B)) ||
                (currentKeyboardState_.IsKeyDown(Keys::B) && previousKeyboardState_.IsKeyUp(Keys::B))) {
                StopMicrophone();
            }
        }
    }

    // Draws the audio waveform being played back.
    void DrawWaveform() {
        spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
        spriteBatch_->DrawString(*font_, instructions, Vector2(10.0f, 20.0f), Color::White);
        spriteBatch_->DrawString(*font_, microphoneStatus_, Vector2(10.0f, 50.0f), Color::White);
        if (!echoBuffer_.empty()) {
            int sampleCount = (int)echoBuffer_.size() / (int)sizeof(SharpRuntime::shortcs);
            auto& vp = getGraphicsDeviceProperty().getViewportProperty();
            for (int index = 0; index < (int)echoBuffer_.size(); index += (int)sizeof(SharpRuntime::shortcs)) {
                int sampleIndex = index / (int)sizeof(SharpRuntime::shortcs);
                vertexPosColor_[sampleIndex].Position.X =
                    sampleIndex * ((float)vp.getWidthProperty() / (float)sampleCount);
                vertexPosColor_[sampleIndex].Position.Y =
                    (vp.getHeightProperty() / 2) -
                    ((float)ReadSample(echoBuffer_, index) / std::numeric_limits<SharpRuntime::shortcs>::max() *
                     (vp.getHeightProperty() / 2));
            }
            effect_->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
            getGraphicsDeviceProperty().DrawUserPrimitives(PrimitiveType::LineStrip,
                vertexPosColor_.data(), 0, (int)vertexPosColor_.size() - 1);
        }
        spriteBatch_->End();
    }

    // Finds a good microphone to use and sets up everything to start recording and playback.
    // Once a microphone is selected the game uses it throughout its lifetime. If it gets
    // disconnected it will tell the user to reconnect it.
    void InitializeMicrophone() {
        // We already have a microphone, skip out early.
        if (activeMicrophone_ != nullptr) return;

        try {
            // Find the first microphone that's ready to rock.
            activeMicrophone_ = PickFirstConnectedMicrophone();
            if (activeMicrophone_ != nullptr) {
                // Set the capture buffer size for low latency. Microphone will call the game
                // back when it has captured at least that much audio data.
                activeMicrophone_->setBufferDurationProperty(System::TimeSpan::FromMilliseconds(100));
                // Subscribe to the event that's raised when the capture buffer is filled.
                bufferReadyToken_ = activeMicrophone_->BufferReady.Add(
                    [this](System::Object* sender, const System::EventArgs& e) {
                        BufferReady(sender, e);
                    });

                // We will put the mic samples in this buffer. We only want to allocate it once.
                micSamples_.assign(
                    activeMicrophone_->GetSampleSizeInBytes(activeMicrophone_->getBufferDurationProperty()), 0);

                // This is a circular buffer. Samples from the mic will be mixed with the oldest
                // sample in this buffer and written back out to this buffer. This feedback
                // creates an echo effect.
                echoBuffer_.assign(
                    activeMicrophone_->GetSampleSizeInBytes(System::TimeSpan::FromSeconds(echoDelay)), 0);

                // Create a DynamicSoundEffectInstance in the right format to playback the
                // captured audio.
                dynamicSound_ = std::make_unique<DynamicSoundEffectInstance>(
                    activeMicrophone_->getSampleRateProperty(), AudioChannels::Mono);
                dynamicSound_->Play();

                // Success - now allocate everything we need to draw the audio waveform.
                effect_.emplace(getGraphicsDeviceProperty());
                auto& vp = getGraphicsDeviceProperty().getViewportProperty();
                auto bounds = vp.getBoundsProperty();
                effect_->setProjectionProperty(
                    Matrix::CreateTranslation(-0.5f, -0.5f, 0.0f) *
                    Matrix::CreateOrthographicOffCenter((float)bounds.getLeftProperty(), (float)bounds.getRightProperty(),
                                                         (float)bounds.getBottomProperty(), (float)bounds.getTopProperty(),
                                                         -1.0f, 1.0f));
                int sampleCount = (int)echoBuffer_.size() / (int)sizeof(SharpRuntime::shortcs);
                vertexPosColor_.assign(sampleCount, VertexPositionColor(Vector3(), Color::White));
            }
        } catch (const NoMicrophoneConnectedException&) {
            // Uh oh, the microphone was disconnected in the middle of initialization. Let's
            // clean up everything so we can look for another microphone again on the next update.
            if (activeMicrophone_ != nullptr && bufferReadyToken_.has_value()) {
                activeMicrophone_->BufferReady.Remove(*bufferReadyToken_);
            }
            bufferReadyToken_.reset();
            activeMicrophone_ = nullptr;
        }
    }

    // Start the microphone.
    void StartMicrophone() {
        if (activeMicrophone_ == nullptr) return;
        try {
            activeMicrophone_->Start();
        } catch (const NoMicrophoneConnectedException&) {
            UpdateMicrophoneStatus();
        }
    }

    // Stop the microphone.
    void StopMicrophone() {
        if (activeMicrophone_ == nullptr) return;
        try {
            activeMicrophone_->Stop();
            // And clear the echo buffer.
            std::fill(echoBuffer_.begin(), echoBuffer_.end(), (SharpRuntime::bytecs)0);
        } catch (const NoMicrophoneConnectedException&) {
            UpdateMicrophoneStatus();
        }
    }

    // Look for a good microphone to start recording.
    Microphone* PickFirstConnectedMicrophone() {
        // Let's pick the default microphone if it's ready.
        Microphone* def = Microphone::getDefaultProperty();
        if (def != nullptr && MicrophoneExtensions::IsConnected(*def)) {
            return def;
        }

        // Default microphone seems to be disconnected so look for another microphone that we
        // can use. And if the default was null then the list will be empty and we'll skip the
        // search.
        for (Microphone* microphone : Microphone::getAllProperty()) {
            if (MicrophoneExtensions::IsConnected(*microphone)) {
                return microphone;
            }
        }

        // There are no microphones hooked up to the system!
        return nullptr;
    }

    // Keep track of the microphone status to communicate to the user.
    void UpdateMicrophoneStatus() {
        if (activeMicrophone_ == nullptr) {
            microphoneStatus_ = "Waiting for microphone connection...";
        } else {
            try {
                // Update the status - if the microphone gets disconnected this will throw.
                MicrophoneState state = activeMicrophone_->getStateProperty();
                microphoneStatus_ = activeMicrophone_->Name + " is " +
                    (state == MicrophoneState::Started ? "Started" : "Stopped");
            } catch (const NoMicrophoneConnectedException&) {
                // Microphone got disconnected - Let's ask the user to reconnect it.
                microphoneStatus_ = "Please reconnect " + activeMicrophone_->Name;
                // Clear the echo buffer.
                std::fill(echoBuffer_.begin(), echoBuffer_.end(), (SharpRuntime::bytecs)0);
            }
        }
    }

    // This is called each time a microphone buffer has been filled.
    void BufferReady(System::Object* /*sender*/, const System::EventArgs& /*e*/) {
        try {
            // Copy the captured audio data into the pre-allocated array.
            activeMicrophone_->GetData(micSamples_, 0, (SharpRuntime::intcs)micSamples_.size());
            ProcessEcho();
        } catch (const NoMicrophoneConnectedException&) {
            // Microphone was disconnected - let the user know.
            UpdateMicrophoneStatus();
        }
    }

    // Captured audio is processed for echo in following steps:
    //   1) Mix each sample with a delayed sample from the echo buffer.
    //   2) Write mixed sample back into echoBuffer_ so it can echo back later.
    //   3) Submit echo buffer to dynamicSound_.
    void ProcessEcho() {
        for (int index = 0; index < (int)micSamples_.size(); index += (int)sizeof(SharpRuntime::shortcs)) {
            SharpRuntime::shortcs micSample  = ReadSample(micSamples_, index);
            SharpRuntime::shortcs echoSample = ReadSample(echoBuffer_, echoBufferPosition_);

            // Mix the echo back into the buffer.
            SharpRuntime::shortcs outputSample = (SharpRuntime::shortcs)(
                (float)micSample * (1.0f - echoAmount) + (float)echoSample * echoAmount);
            WriteSample(echoBuffer_, echoBufferPosition_, outputSample);
            echoBufferPosition_ += (int)sizeof(SharpRuntime::shortcs);

            // Play back the echo buffer if it's filled.
            if (echoBufferPosition_ == (int)echoBuffer_.size()) {
                dynamicSound_->SubmitBuffer(echoBuffer_, 0, (SharpRuntime::intcs)echoBuffer_.size());
                // Reset the position to the beginning of the buffer.
                echoBufferPosition_ = 0;
            }
        }
    }

    // Returns a sample value from the passed buffer, taking into account the endian-ness of
    // the system.
    SharpRuntime::shortcs ReadSample(const std::vector<SharpRuntime::bytecs>& buffer, int index) const {
        if (index % (int)sizeof(SharpRuntime::shortcs) != 0) {
            throw System::ArgumentException("index");
        }
        if (index >= (int)buffer.size()) {
            throw System::ArgumentOutOfRangeException("index");
        }
        if (index < 0) {
            // std::vector does not provide the managed array's implicit negative-index check.
            throw System::IndexOutOfRangeException();
        }

        if (bigEndian_) {
            return (SharpRuntime::shortcs)(buffer[index] << 8 | (buffer[index + 1] & 0xff));
        }
        return (SharpRuntime::shortcs)((buffer[index] & 0xff) | (buffer[index + 1] << 8));
    }

    // Writes the passed sample value to the buffer, taking into account the endian-ness of
    // the system.
    void WriteSample(std::vector<SharpRuntime::bytecs>& buffer, int index, SharpRuntime::shortcs sample) const {
        if (index % (int)sizeof(SharpRuntime::shortcs) != 0) {
            throw System::ArgumentException("index");
        }
        if (index >= (int)buffer.size()) {
            throw System::ArgumentOutOfRangeException("index");
        }
        if (index < 0) {
            // std::vector does not provide the managed array's implicit negative-index check.
            throw System::IndexOutOfRangeException();
        }

        if (bigEndian_) {
            buffer[index]     = (SharpRuntime::bytecs)(sample >> 8);
            buffer[index + 1] = (SharpRuntime::bytecs)sample;
        } else {
            buffer[index]     = (SharpRuntime::bytecs)sample;
            buffer[index + 1] = (SharpRuntime::bytecs)(sample >> 8);
        }
    }

};

} // namespace MicrophoneEchoSample
