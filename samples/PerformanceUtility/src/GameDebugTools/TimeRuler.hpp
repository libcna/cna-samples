// SPDX-License-Identifier: MS-PL
#pragma once

// TimeRuler.hpp — C++ port of GameDebugTools/TimeRuler.cs (XNA 4.0
// PerformanceUtility sample). Realtime CPU measuring tool: visualizes
// BeginMark/EndMark-instrumented sections of code as colored bars, plus an
// optional text log of min/max/avg times per marker.
#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cmath>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "System/Diagnostics/Stopwatch.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/IndexOutOfRangeException.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/OverflowException.hpp"
#include "System/Text/StringBuilder.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

#include "System/Int32.hpp"
#include "DebugManager.hpp"
#include "IDebugCommandHost.hpp"
#include "Layout.hpp"
#include "StringBuilderExtensions.hpp"

namespace PerformanceUtility::GameDebugTools {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Realtime CPU measuring tool. Port of GameDebugTools/TimeRuler.cs.
class TimeRuler : public DrawableGameComponent {
public:
    [[nodiscard]] bool getShowLogProperty() const { return showLog_; }
    void setShowLogProperty(bool value) { showLog_ = value; }

    [[nodiscard]] int getTargetSampleFramesProperty() const { return targetSampleFrames_; }
    void setTargetSampleFramesProperty(int value) { targetSampleFrames_ = value; }

    [[nodiscard]] Vector2 getPositionProperty() const { return position_; }
    void setPositionProperty(Vector2 value) { position_ = value; }

    [[nodiscard]] int getWidthProperty() const { return width_; }
    void setWidthProperty(int value) { width_ = value; }

    explicit TimeRuler(Game& game) : DrawableGameComponent(game) {
        game.getServicesProperty().AddService<TimeRuler>(this);
    }

    void Initialize() override {
        debugManager_ = getGameProperty().getServicesProperty().GetService<DebugManager>();
        if (debugManager_ == nullptr)
            throw System::InvalidOperationException("DebugManager is not registered.");

        auto* host = getGameProperty().getServicesProperty().GetService<IDebugCommandHost>();
        if (host != nullptr) {
            host->RegisterCommand("tr", "TimeRuler",
                [this](IDebugCommandHost& h, const std::string&, const std::vector<std::string>& args) {
                    CommandExecute(h, args);
                });
            setVisibleProperty(false);
        }

        logs_[0] = FrameLog{};
        logs_[1] = FrameLog{};
        sampleFrames_ = targetSampleFrames_ = 1;
        logString_.EnsureCapacity(512);

        // The TimeRuler's Update method doesn't need to get called.
        setEnabledProperty(false);

        DrawableGameComponent::Initialize();
    }

    void LoadContent() override {
        width_ = static_cast<int>(
            static_cast<float>(getGraphicsDeviceProperty().getViewportProperty().getWidthProperty()) * 0.8f);

        Layout layout(getGraphicsDeviceProperty().getViewportProperty());
        position_ = layout.Place(Vector2(static_cast<float>(width_), static_cast<float>(BarHeight)),
                                 0.0f, 0.01f, Alignment::BottomCenter);

        DrawableGameComponent::LoadContent();
    }

    // Starts a new frame. Call at the top of Game::Update.
    void StartFrame() {
        std::scoped_lock lock(mutex_);

        // We skip resetting the frame when this method gets called multiple times
        // (XNA's fixed-timestep catch-up calls Update more than once per Draw).
        int count = updateCount_.fetch_add(1) + 1;
        if (getVisibleProperty() && (1 < count && count < MaxSampleFrames))
            return;

        prevLog_ = &logs_[frameCount_++ & 0x1];
        curLog_ = &logs_[frameCount_ & 0x1];

        float endFrameTime = (float)stopwatch_.getElapsedProperty().getTotalMillisecondsProperty();

        for (int barIdx = 0; barIdx < MaxBars; ++barIdx) {
            MarkerCollection& prevBar = prevLog_->Bars[barIdx];
            MarkerCollection& nextBar = curLog_->Bars[barIdx];

            // Re-open markers that didn't get EndMark called in the previous frame.
            for (int nest = 0; nest < prevBar.NestCount; ++nest) {
                int markerIdx = prevBar.MarkerNests[nest];

                prevBar.Markers[markerIdx].EndTime = endFrameTime;

                nextBar.MarkerNests[nest] = nest;
                nextBar.Markers[nest].MarkerId = prevBar.Markers[markerIdx].MarkerId;
                nextBar.Markers[nest].BeginTime = 0.0f;
                nextBar.Markers[nest].EndTime = -1.0f;
                nextBar.Markers[nest].Color = prevBar.Markers[markerIdx].Color;
            }

            // Update the marker log.
            for (int markerIdx = 0; markerIdx < prevBar.MarkCount; ++markerIdx) {
                float duration = prevBar.Markers[markerIdx].EndTime - prevBar.Markers[markerIdx].BeginTime;

                int markerId = prevBar.Markers[markerIdx].MarkerId;
                MarkerInfo& m = markers_[markerId];

                m.Logs[barIdx].Color = prevBar.Markers[markerIdx].Color;

                if (!m.Logs[barIdx].Initialized) {
                    m.Logs[barIdx].Min = duration;
                    m.Logs[barIdx].Max = duration;
                    m.Logs[barIdx].Avg = duration;
                    m.Logs[barIdx].Initialized = true;
                } else {
                    m.Logs[barIdx].Min = std::min(m.Logs[barIdx].Min, duration);
                    m.Logs[barIdx].Max = std::min(m.Logs[barIdx].Max, duration);
                    m.Logs[barIdx].Avg += duration;
                    m.Logs[barIdx].Avg *= 0.5f;

                    if (m.Logs[barIdx].Samples++ >= LogSnapDuration) {
                        m.Logs[barIdx].SnapMin = m.Logs[barIdx].Min;
                        m.Logs[barIdx].SnapMax = m.Logs[barIdx].Max;
                        m.Logs[barIdx].SnapAvg = m.Logs[barIdx].Avg;
                        m.Logs[barIdx].Samples = 0;
                    }
                }
            }

            nextBar.MarkCount = prevBar.NestCount;
            nextBar.NestCount = prevBar.NestCount;
        }

        stopwatch_.Reset();
        stopwatch_.Start();
    }

    void BeginMark(const std::string& markerName, Color color) { BeginMark(0, markerName, color); }

    void BeginMark(int barIndex, const std::string& markerName, Color color) {
        std::scoped_lock lock(mutex_);

        if (barIndex < 0 || barIndex >= MaxBars)
            throw System::ArgumentOutOfRangeException("barIndex");

        MarkerCollection& bar = curLog_->Bars[barIndex];

        if (bar.MarkCount >= MaxSamples)
            throw System::OverflowException(
                "Exceeded sample count.\n"
                "Either set larger number to TimeRuler.MaxSmpale orlower sample count.");

        if (bar.NestCount >= MaxNestCall)
            throw System::OverflowException(
                "Exceeded nest count.\n"
                "Either set larget number to TimeRuler.MaxNestCall orlower nest calls.");

        int markerId = GetOrRegisterMarker(markerName);

        bar.MarkerNests[bar.NestCount++] = bar.MarkCount;

        bar.Markers[bar.MarkCount].MarkerId = markerId;
        bar.Markers[bar.MarkCount].Color = color;
        bar.Markers[bar.MarkCount].BeginTime = (float)stopwatch_.getElapsedProperty().getTotalMillisecondsProperty();
        bar.Markers[bar.MarkCount].EndTime = -1.0f;

        bar.MarkCount++;
    }

    void EndMark(const std::string& markerName) { EndMark(0, markerName); }

    void EndMark(int barIndex, const std::string& markerName) {
        std::scoped_lock lock(mutex_);

        if (barIndex < 0 || barIndex >= MaxBars)
            throw System::ArgumentOutOfRangeException("barIndex");

        MarkerCollection& bar = curLog_->Bars[barIndex];

        if (bar.NestCount <= 0)
            throw System::InvalidOperationException(
                "Call BeingMark method before call EndMark method.");

        auto it = markerNameToIdMap_.find(markerName);
        if (it == markerNameToIdMap_.end())
            throw System::InvalidOperationException(
                "Maker '" + markerName + "' is not registered."
                "Make sure you specifed same name as you used for BeginMark method.");

        int markerId = it->second;
        int markerIdx = bar.MarkerNests[--bar.NestCount];
        if (bar.Markers[markerIdx].MarkerId != markerId)
            throw System::InvalidOperationException(
                "Incorrect call order of BeginMark/EndMark method."
                "You call it like BeginMark(A), BeginMark(B), EndMark(B), EndMark(A)"
                " But you can't call it like BeginMark(A), BeginMark(B), EndMark(A), EndMark(B).");

        bar.Markers[markerIdx].EndTime = (float)stopwatch_.getElapsedProperty().getTotalMillisecondsProperty();
    }

    float GetAverageTime(int barIndex, const std::string& markerName) const {
        if (barIndex < 0 || barIndex >= MaxBars)
            throw System::ArgumentOutOfRangeException("barIndex");

        auto it = markerNameToIdMap_.find(markerName);
        if (it == markerNameToIdMap_.end())
            return 0.0f;

        return markers_[it->second].Logs[barIndex].Avg;
    }

    void ResetLog() {
        std::scoped_lock lock(mutex_);

        for (MarkerInfo& markerInfo : markers_) {
            for (int i = 0; i < MaxBars; ++i) {
                MarkerLog& log = markerInfo.Logs[i];
                log.Initialized = false;
                log.SnapMin = 0.0f;
                log.SnapMax = 0.0f;
                log.SnapAvg = 0.0f;
                log.Min = 0.0f;
                log.Max = 0.0f;
                log.Avg = 0.0f;
                log.Samples = 0;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        Draw(position_, width_);
        DrawableGameComponent::Draw(gameTime);
    }

    void Draw(Vector2 position, int width) {
        updateCount_.store(0);

        SpriteBatch& spriteBatch = debugManager_->getSpriteBatchProperty();
        SpriteFont& font = debugManager_->getDebugFontProperty();
        Texture2D& texture = debugManager_->getWhiteTextureProperty();

        int height = 0;
        float maxTime = 0.0f;
        for (const MarkerCollection& bar : prevLog_->Bars) {
            if (bar.MarkCount > 0) {
                height += BarHeight + BarPadding * 2;
                maxTime = std::max(maxTime, bar.Markers[bar.MarkCount - 1].EndTime);
            }
        }

        const float frameSpan = 1.0f / 60.0f * 1000.0f;
        float sampleSpan = (float)sampleFrames_ * frameSpan;

        if (maxTime > sampleSpan)
            frameAdjust_ = std::max(0, frameAdjust_) + 1;
        else
            frameAdjust_ = std::min(0, frameAdjust_) - 1;

        if (std::abs(frameAdjust_) > AutoAdjustDelay) {
            sampleFrames_ = std::min(MaxSampleFrames, sampleFrames_);
            sampleFrames_ = std::max(targetSampleFrames_, (int)(maxTime / frameSpan) + 1);
            frameAdjust_ = 0;
        }

        float msToPs = (float)width / sampleSpan;

        int startY = (int)position.Y - (height - BarHeight);
        int y = startY;

        spriteBatch.Begin();

        Rectangle rc((int)position.X, y, width, height);
        spriteBatch.Draw(texture, rc, Color(0, 0, 0, 128));

        rc.Height = BarHeight;
        for (const MarkerCollection& bar : prevLog_->Bars) {
            rc.Y = y + BarPadding;
            for (int j = 0; j < bar.MarkCount; ++j) {
                float bt = bar.Markers[j].BeginTime;
                float et = bar.Markers[j].EndTime;
                int sx = (int)(position.X + bt * msToPs);
                int ex = (int)(position.X + et * msToPs);
                rc.X = sx;
                rc.Width = std::max(ex - sx, 1);

                spriteBatch.Draw(texture, rc, bar.Markers[j].Color);
            }

            y += BarHeight + BarPadding;
        }

        // Grid lines: one per millisecond.
        rc = Rectangle((int)position.X, startY, 1, height);
        for (float t = 1.0f; t < sampleSpan; t += 1.0f) {
            rc.X = (int)(position.X + t * msToPs);
            spriteBatch.Draw(texture, rc, Color::Gray);
        }

        // Frame grid.
        for (int i = 0; i <= sampleFrames_; ++i) {
            rc.X = (int)(position.X + frameSpan * (float)i * msToPs);
            spriteBatch.Draw(texture, rc, Color::White);
        }

        if (showLog_) {
            y = startY - font.getLineSpacingProperty();
            logString_.setLengthProperty(0);
            for (const MarkerInfo& markerInfo : markers_) {
                for (int i = 0; i < MaxBars; ++i) {
                    if (markerInfo.Logs[i].Initialized) {
                        if (logString_.getLengthProperty() > 0)
                            logString_.Append("\n");

                        logString_.Append(" Bar ");
                        StringBuilderExtensions::AppendNumber(logString_, i);
                        logString_.Append(" ");
                        logString_.Append(markerInfo.Name);
                        logString_.Append(" Avg.:");
                        StringBuilderExtensions::AppendNumber(logString_, markerInfo.Logs[i].SnapAvg);
                        logString_.Append("ms ");

                        y -= font.getLineSpacingProperty();
                    }
                }
            }

            Vector2 size = font.MeasureString(logString_.ToString());
            rc = Rectangle((int)position.X, y, (int)size.X + 12, (int)size.Y);
            spriteBatch.Draw(texture, rc, Color(0, 0, 0, 128));

            spriteBatch.DrawString(font, logString_.ToString(),
                                   Vector2(position.X + 12.0f, (float)y), Color::White);

            y += (int)((float)font.getLineSpacingProperty() * 0.3f);
            rc = Rectangle((int)position.X + 4, y, 10, 10);
            Rectangle rc2((int)position.X + 5, y + 1, 8, 8);
            for (const MarkerInfo& markerInfo : markers_) {
                for (int i = 0; i < MaxBars; ++i) {
                    if (markerInfo.Logs[i].Initialized) {
                        rc.Y = y;
                        rc2.Y = y + 1;
                        spriteBatch.Draw(texture, rc, Color::White);
                        spriteBatch.Draw(texture, rc2, markerInfo.Logs[i].Color);

                        y += font.getLineSpacingProperty();
                    }
                }
            }
        }

        spriteBatch.End();
    }

private:
    static constexpr int MaxBars = 8;
    static constexpr int MaxSamples = 256;
    static constexpr int MaxNestCall = 32;
    static constexpr int MaxSampleFrames = 4;
    static constexpr int LogSnapDuration = 120;
    static constexpr int BarHeight = 8;
    static constexpr int BarPadding = 2;
    static constexpr int AutoAdjustDelay = 30;

    struct Marker {
        int MarkerId = 0;
        float BeginTime = 0.0f;
        float EndTime = 0.0f;
        Microsoft::Xna::Framework::Color Color = Microsoft::Xna::Framework::Color::White;
    };

    struct MarkerCollection {
        std::array<Marker, MaxSamples> Markers;
        int MarkCount = 0;

        std::array<int, MaxNestCall> MarkerNests{};
        int NestCount = 0;
    };

    struct FrameLog {
        std::array<MarkerCollection, MaxBars> Bars;
    };

    struct MarkerLog {
        float SnapMin = 0.0f, SnapMax = 0.0f, SnapAvg = 0.0f;
        float Min = 0.0f, Max = 0.0f, Avg = 0.0f;
        int Samples = 0;
        Microsoft::Xna::Framework::Color Color = Microsoft::Xna::Framework::Color::White;
        bool Initialized = false;
    };

    struct MarkerInfo {
        std::string Name;
        std::array<MarkerLog, MaxBars> Logs;
        explicit MarkerInfo(std::string name) : Name(std::move(name)) {}
    };

    void CommandExecute(IDebugCommandHost& host, const std::vector<std::string>& arguments) {
        bool previousVisible = getVisibleProperty();

        if (arguments.empty())
            setVisibleProperty(!getVisibleProperty());

        for (const std::string& orgArg : arguments) {
            std::string arg = orgArg;
            std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

            std::string sub0 = arg;
            std::string sub1;
            auto colon = arg.find(':');
            const bool hasSubArgument = colon != std::string::npos;
            if (colon != std::string::npos) {
                sub0 = arg.substr(0, colon);
                const auto nextColon = arg.find(':', colon + 1);
                sub1 = arg.substr(colon + 1, nextColon - colon - 1);
            }

            if (sub0 == "on") {
                setVisibleProperty(true);
            } else if (sub0 == "off") {
                setVisibleProperty(false);
            } else if (sub0 == "reset") {
                ResetLog();
            } else if (sub0 == "log") {
                if (hasSubArgument) {
                    if (sub1 == "on") showLog_ = true;
                    if (sub1 == "off") showLog_ = false;
                } else {
                    showLog_ = !showLog_;
                }
            } else if (sub0 == "frame") {
                if (!hasSubArgument)
                    throw System::IndexOutOfRangeException();
                int a = std::max(1, std::min(MaxSampleFrames, System::Int32::Parse(sub1)));
                targetSampleFrames_ = a;
            } else if (sub0 == "/?" || sub0 == "--help") {
                host.Echo("tr [log|on|off|reset|frame]");
                host.Echo("Options:");
                host.Echo("       on     Display TimeRuler.");
                host.Echo("       off    Hide TimeRuler.");
                host.Echo("       log    Show/Hide marker log.");
                host.Echo("       reset  Reset marker log.");
                host.Echo("       frame:sampleFrames");
                host.Echo("              Change target sample frame count");
            }
        }

        if (getVisibleProperty() != previousVisible)
            updateCount_.store(0);
    }

    int GetOrRegisterMarker(const std::string& markerName) {
        auto it = markerNameToIdMap_.find(markerName);
        if (it != markerNameToIdMap_.end())
            return it->second;

        int markerId = (int)markers_.size();
        markerNameToIdMap_.emplace(markerName, markerId);
        markers_.emplace_back(markerName);
        return markerId;
    }

    DebugManager* debugManager_ = nullptr;

    std::array<FrameLog, 2> logs_;
    FrameLog* prevLog_ = &logs_[0];
    FrameLog* curLog_ = &logs_[0];

    int frameCount_ = 0;
    System::Diagnostics::Stopwatch stopwatch_;

    std::vector<MarkerInfo> markers_;
    std::unordered_map<std::string, int> markerNameToIdMap_;

    int frameAdjust_ = 0;
    int sampleFrames_ = 1;

    std::atomic<int> updateCount_{0};
    std::mutex mutex_;

    bool showLog_ = false;
    int targetSampleFrames_ = 1;
    Vector2 position_;
    int width_ = 0;
    System::Text::StringBuilder logString_;
};

} // namespace PerformanceUtility::GameDebugTools
