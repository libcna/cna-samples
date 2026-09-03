// SPDX-License-Identifier: MS-PL

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "GameLogic/MobileControls.hpp"
#include "GameLogic/MobileInput.hpp"
#include "CNA/Platform/CurrentPlatform.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace
{
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using RacingGame::GameLogic::MobileControlLayout;
    using RacingGame::GameLogic::MobileControlMapper;
    using RacingGame::GameLogic::MobileControlPreferences;
    using RacingGame::GameLogic::MobileInput;
    using RacingGame::GameLogic::MobileTouchPoint;
    using RacingGame::GameLogic::TiltSteeringFilter;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    int passed = 0;
    int failed = 0;

    void Check(const bool condition, const std::string& name)
    {
        if (condition)
        {
            ++passed;
            std::cout << "PASS " << name << '\n';
        }
        else
        {
            ++failed;
            std::cout << "FAIL " << name << '\n';
        }
    }

    bool Inside(const Rectangle& inner, const Rectangle& outer)
    {
        return inner.X >= outer.X && inner.Y >= outer.Y &&
               inner.X + inner.Width <= outer.X + outer.Width &&
               inner.Y + inner.Height <= outer.Y + outer.Height;
    }

    Vector2 Centre(const Rectangle& value)
    {
        return Vector2(
            static_cast<float>(value.X) + static_cast<float>(value.Width) * 0.5f,
            static_cast<float>(value.Y) + static_cast<float>(value.Height) * 0.5f);
    }

    MobileTouchPoint Touch(
        const int id, const Rectangle& rectangle,
        const MobileTouchPoint::Phase phase = MobileTouchPoint::Phase::Pressed)
    {
        return MobileTouchPoint{id, Centre(rectangle), phase};
    }
}

int main()
{
    Check(
        MobileControlLayout::MapClientSafeAreaToDisplay(
            Rectangle(0, 0, 2400, 1080),
            Rectangle(120, 54, 2160, 972), 800, 480) ==
            Rectangle(40, 24, 720, 432),
        "window safe area maps independently onto both backbuffer axes");
    Check(
        MobileControlLayout::MapClientSafeAreaToDisplay(
            Rectangle(0, 0, 800, 360),
            Rectangle(40, 18, 720, 324), 800, 480) ==
            Rectangle(40, 24, 720, 432),
        "display density does not change logical safe-area placement");

    const Rectangle safeArea(32, 24, 2336, 1032);
    MobileControlPreferences preferences;
    const MobileControlLayout rightHanded = MobileControlLayout::Create(
        2400, 1080, safeArea, preferences);

    Check(rightHanded.safeArea == safeArea, "safe area is preserved");
    Check(Inside(rightHanded.steering, safeArea), "steering is safe-area contained");
    Check(Inside(rightHanded.throttle, safeArea), "throttle is safe-area contained");
    Check(Inside(rightHanded.brake, safeArea), "brake is safe-area contained");
    Check(Inside(rightHanded.handbrake, safeArea), "handbrake is safe-area contained");
    Check(Inside(rightHanded.back, safeArea), "back is safe-area contained");
    Check(Inside(rightHanded.camera, safeArea), "camera is safe-area contained");
    Check(rightHanded.steering.X < safeArea.X + safeArea.Width / 2,
          "right-handed steering is on the left");
    Check(rightHanded.throttle.X > safeArea.X + safeArea.Width / 2,
          "right-handed pedals are on the right");
    Check(rightHanded.camera.getRightProperty() <
              safeArea.X + safeArea.Width / 2 &&
              rightHanded.back.X > safeArea.X + safeArea.Width / 2,
          "top controls avoid the corner HUD and straddle the safe-area centre");

    preferences.leftHanded = true;
    const MobileControlLayout leftHanded = MobileControlLayout::Create(
        2400, 1080, safeArea, preferences);
    const auto mirrorX = [&](const Rectangle& rectangle)
    {
        return safeArea.X + safeArea.Width - (rectangle.X - safeArea.X) -
               rectangle.Width;
    };
    Check(leftHanded.steering.X == mirrorX(rightHanded.steering),
          "left-handed steering mirrors exactly");
    Check(leftHanded.throttle.X == mirrorX(rightHanded.throttle),
          "left-handed throttle mirrors exactly");
    Check(leftHanded.back.X == mirrorX(rightHanded.back),
          "left-handed top buttons mirror exactly");

    preferences = {};
    MobileControlMapper mapper;
    const MobileControlLayout layout = MobileControlLayout::Create(
        2400, 1080, safeArea, preferences);
    std::vector<MobileTouchPoint> contacts{
        MobileTouchPoint{
            1,
            Vector2(
                static_cast<float>(layout.steering.X + layout.steering.Width - 1),
                Centre(layout.steering).Y),
            MobileTouchPoint::Phase::Pressed},
        MobileTouchPoint{
            2,
            Vector2(
                Centre(layout.throttle).X,
                static_cast<float>(layout.throttle.Y + layout.throttle.Height - 1)),
            MobileTouchPoint::Phase::Pressed},
        Touch(3, layout.handbrake),
    };
    auto state = mapper.Map(contacts, 2400, 1080, safeArea, preferences);
    Check(state.steering > 0.98f, "steering reaches the positive analog limit");
    Check(state.throttle > 0.99f, "throttle reaches the analog limit");
    Check(state.handbrakePressed, "handbrake supports a simultaneous third finger");
    Check(state.drivingTouchActive, "multi-touch reports active driving input");
    Check(state.hasSteeringPosition, "steering thumb position is retained for rendering");

    contacts = {
        MobileTouchPoint{
            1,
            Vector2(static_cast<float>(layout.steering.X - 500),
                    Centre(layout.steering).Y),
            MobileTouchPoint::Phase::Moved},
        Touch(2, layout.throttle, MobileTouchPoint::Phase::Moved),
    };
    state = mapper.Map(contacts, 2400, 1080, safeArea, preferences);
    Check(state.steering < -0.99f,
          "captured steering finger remains assigned outside the pad");
    Check(state.throttle >= 0.25f && state.throttle <= 1.0f,
          "pedal movement remains bounded");

    contacts = {
        Touch(1, layout.steering, MobileTouchPoint::Phase::Released),
        Touch(2, layout.throttle, MobileTouchPoint::Phase::Released),
    };
    state = mapper.Map(contacts, 2400, 1080, safeArea, preferences);
    Check(state.steering == 0.0f && state.throttle == 0.0f,
          "released fingers clear analog controls");
    Check(!state.drivingTouchActive, "released fingers clear driving activity");

    contacts = {Touch(4, layout.back), Touch(5, layout.camera)};
    state = mapper.Map(contacts, 2400, 1080, safeArea, preferences);
    Check(state.backPressed && state.backJustPressed,
          "back button emits held and edge states");
    Check(state.cameraPressed && state.cameraJustPressed,
          "camera button emits held and edge states");
    contacts = {
        Touch(4, layout.back, MobileTouchPoint::Phase::Moved),
        Touch(5, layout.camera, MobileTouchPoint::Phase::Moved),
    };
    state = mapper.Map(contacts, 2400, 1080, safeArea, preferences);
    Check(state.backPressed && !state.backJustPressed,
          "back edge is emitted only once");
    Check(state.cameraPressed && !state.cameraJustPressed,
          "camera edge is emitted only once");

    preferences.opacity = 2.0f;
    state = mapper.Map({}, 2400, 1080, Rectangle::Empty, preferences);
    Check(state.layout.safeArea == Rectangle(0, 0, 2400, 1080),
          "empty safe area falls back to the backbuffer");
    Check(std::abs(state.opacity - 0.85f) < 0.0001f,
          "overlay opacity is clamped");
    mapper.Reset();

    TiltSteeringFilter tilt;
    Check(!tilt.getIsCalibratedProperty(), "tilt begins uncalibrated");
    Check(tilt.Update(0.2f, 1.0f, false) == 0.0f,
          "first tilt sample establishes neutral");
    Check(tilt.getIsCalibratedProperty(), "first sample calibrates tilt");
    float tilted = 0.0f;
    for (int index = 0; index < 20; ++index)
        tilted = tilt.Update(0.65f, 1.0f, false);
    Check(tilted > 0.8f && tilted <= 1.0f,
          "tilt low-pass filter converges to positive steering");
    tilt.Calibrate(0.2f);
    for (int index = 0; index < 3; ++index)
        tilted = tilt.Update(0.65f, 1.5f, false);
    Check(tilted > 0.95f && tilted <= 1.0f,
          "aggressive tilt setting reaches full steering within three samples");
    tilt.Calibrate(0.2f);
    for (int index = 0; index < 20; ++index)
        tilted = tilt.Update(0.65f, 1.0f, true);
    Check(tilted < -0.8f && tilted >= -1.0f,
          "tilt inversion reverses steering");
    tilt.Reset();
    Check(!tilt.getIsCalibratedProperty(), "tilt reset clears calibration");

    TouchPanel::ResetForTests();
    TouchPanel::setDisplayWidthProperty(2400);
    TouchPanel::setDisplayHeightProperty(1080);
    TouchPanel::setTouchDeviceExistsProperty(true);
    auto* platformMouse = CNA::Platform::GetCurrentPlatform().GetMouse();
    Check(platformMouse != nullptr,
          "mobile integration probe has a platform mouse service");
    if (platformMouse != nullptr)
        platformMouse->SetPosition(0, 2000, 500);
    MobileInput mobileInput;
    mobileInput.SetSafeArea(safeArea);
    (void)mobileInput.Capture(true, true, 2400, 1080);
    TouchPanel::INTERNAL_setTouchState(
        20, TouchLocationState::Pressed,
        Vector2(
            static_cast<float>(layout.steering.getRightProperty() - 1),
            Centre(layout.steering).Y));
    TouchPanel::INTERNAL_setTouchState(
        21, TouchLocationState::Pressed,
        Vector2(Centre(layout.throttle).X,
                static_cast<float>(layout.throttle.getBottomProperty() - 1)));
    auto integrated = mobileInput.Capture(true, true, 2400, 1080);
    Check(integrated.car.mobileSteering > 0.98f,
          "mobile provider merges touch steering into the car snapshot");
    Check(integrated.car.mobileThrottle > 0.99f,
          "mobile provider merges touch throttle into the car snapshot");
    Check(integrated.mobile.overlayVisible,
          "connected touch hardware enables the in-race overlay");
    Check(integrated.car.mouseXMovement == 0.0f,
          "touch-owned race frame suppresses its duplicate mouse steering");

    TouchPanel::INTERNAL_setTouchState(
        20, TouchLocationState::Released, Centre(layout.steering));
    TouchPanel::INTERNAL_setTouchState(
        21, TouchLocationState::Released, Centre(layout.throttle));
    integrated = mobileInput.Capture(true, true, 2400, 1080);
    Check(integrated.car.mouseXMovement == 0.0f,
          "release frame cannot leak smoothed touch-mouse steering");

    TouchPanel::ResetForTests();
    if (platformMouse != nullptr)
        platformMouse->SetPosition(0, 2100, 500);
    integrated = mobileInput.Capture(true, true, 2400, 1080);
    Check(platformMouse == nullptr || integrated.car.mouseXMovement > 0.0f,
          "genuine mouse steering resumes after the touch-owned frames");

    TouchPanel::ResetForTests();
    TouchPanel::setDisplayWidthProperty(2400);
    TouchPanel::setDisplayHeightProperty(1080);
    TouchPanel::setTouchDeviceExistsProperty(true);
    TouchPanel::INTERNAL_setTouchState(
        30, TouchLocationState::Pressed, Vector2(810.0f, 420.0f));
    integrated = mobileInput.Capture(false, true, 2400, 1080);
    Check(integrated.mousePosition.X == 810 &&
              integrated.mousePosition.Y == 420 &&
              integrated.mouseLeftPressed &&
              integrated.mouseLeftJustPressed,
          "menu touch maps to the existing pointer and click model");
    TouchPanel::ResetForTests();

    TouchPanel::setDisplayWidthProperty(2400);
    TouchPanel::setDisplayHeightProperty(1080);
    TouchPanel::setTouchDeviceExistsProperty(true);
    MobileInput transitionInput;
    transitionInput.SetSafeArea(safeArea);
    TouchPanel::INTERNAL_setTouchState(
        31, TouchLocationState::Pressed,
        Vector2(
            static_cast<float>(layout.steering.getRightProperty() - 1),
            Centre(layout.steering).Y));
    (void)transitionInput.Capture(false, true, 2400, 1080);
    integrated = transitionInput.Capture(true, true, 2400, 1080);
    Check(integrated.car.mobileSteering == 0.0f &&
              !integrated.mobile.hasSteeringPosition,
          "touch held across GO is ignored on race entry");
    TouchPanel::INTERNAL_setTouchState(
        31, TouchLocationState::Released, Centre(layout.steering));
    integrated = transitionInput.Capture(true, true, 2400, 1080);
    Check(integrated.car.mobileSteering == 0.0f,
          "releasing the inherited GO touch keeps steering neutral");
    TouchPanel::INTERNAL_setTouchState(
        32, TouchLocationState::Pressed,
        Vector2(
            static_cast<float>(layout.steering.getRightProperty() - 1),
            Centre(layout.steering).Y));
    integrated = transitionInput.Capture(true, true, 2400, 1080);
    Check(integrated.car.mobileSteering > 0.98f,
          "a new post-entry steering touch is accepted normally");

    TouchPanel::ResetForTests();
    MobileInput releasedGoInput;
    if (platformMouse != nullptr)
        platformMouse->SetPosition(0, 300, 500);
    (void)releasedGoInput.Capture(false, true, 2400, 1080);
    if (platformMouse != nullptr)
        platformMouse->SetPosition(0, 2100, 500);
    integrated = releasedGoInput.Capture(true, true, 2400, 1080);
    Check(platformMouse == nullptr || integrated.car.mouseXMovement == 0.0f,
          "first race frame suppresses a released GO mouse delta");

    std::cout << "RESULT " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
