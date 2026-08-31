// SPDX-License-Identifier: MS-PL
#include "Transition.hpp"

#include "Controls/IControl.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace DynamicMenu::Transitions
{
    using namespace Microsoft::Xna::Framework;
    using DynamicMenu::Controls::IControl;

    Transition::Transition(
        std::optional<Point> startPosition, std::optional<Point> endPosition,
        std::optional<Point> startSize, std::optional<Point> endSize,
        std::optional<Color> startColor, std::optional<Color> endColor)
    {
        if (startPosition) setStartPositionProperty(*startPosition);
        if (endPosition) setEndPositionProperty(*endPosition);
        if (startSize) setStartSizeProperty(*startSize);
        if (endSize) setEndSizeProperty(*endSize);
        if (startColor) setStartColorProperty(*startColor);
        if (endColor) setEndColorProperty(*endColor);
    }

    IControl* Transition::getControlProperty() const { return control_; }
    void Transition::setControlProperty(IControl* value) { control_ = value; }
    Point Transition::getStartPositionProperty() const { return startPosition_; }
    void Transition::setStartPositionProperty(Point value) { startPositionSet_ = true; startPosition_ = value; }
    Point Transition::getEndPositionProperty() const { return endPosition_; }
    void Transition::setEndPositionProperty(Point value) { endPositionSet_ = true; endPosition_ = value; }
    Point Transition::getStartSizeProperty() const { return startSize_; }
    void Transition::setStartSizeProperty(Point value) { startSizeSet_ = true; startSize_ = value; }
    Point Transition::getEndSizeProperty() const { return endSize_; }
    void Transition::setEndSizeProperty(Point value) { endSizeSet_ = true; endSize_ = value; }
    Color Transition::getStartColorProperty() const { return startHue_; }
    void Transition::setStartColorProperty(Color value) { startHueSet_ = true; startHue_ = value; }
    Color Transition::getEndColorProperty() const { return endHue_; }
    void Transition::setEndColorProperty(Color value) { endHueSet_ = true; endHue_ = value; }
    float Transition::getTransitionLengthProperty() const { return transitionLength_; }
    void Transition::setTransitionLengthProperty(float value) { transitionLength_ = value; }
    bool Transition::getTransitionActiveProperty() const { return transitionActive_; }

    void Transition::StartTranstion()
    {
        transitionActive_ = true;
        transitionStartTime_ = 0.0;
        if (startPositionSet_)
        {
            control_->setLeftProperty(startPosition_.X);
            control_->setTopProperty(startPosition_.Y);
        }
        else
        {
            startPosition_ = Point(control_->getLeftProperty(), control_->getTopProperty());
        }
        if (startSizeSet_)
        {
            control_->setWidthProperty(startSize_.X);
            control_->setHeightProperty(startSize_.Y);
        }
        else
        {
            startSize_ = Point(control_->getWidthProperty(), control_->getHeightProperty());
        }
        if (startHueSet_) control_->setHueProperty(startHue_);
        else startHue_ = control_->getHueProperty();
        if (!endPositionSet_) endPosition_ = Point(control_->getLeftProperty(), control_->getTopProperty());
        if (!endSizeSet_) endSize_ = Point(control_->getWidthProperty(), control_->getHeightProperty());
        if (!endHueSet_) endHue_ = control_->getHueProperty();
    }

    void Transition::Update(const GameTime& gameTime)
    {
        if (!transitionActive_) return;
        if (transitionStartTime_ == 0.0)
            transitionStartTime_ = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();
        const float timeSinceStart = static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() - transitionStartTime_);
        const float percentComplete = timeSinceStart / transitionLength_;
        if (percentComplete > 1.0f)
        {
            control_->setLeftProperty(endPosition_.X);
            control_->setTopProperty(endPosition_.Y);
            control_->setWidthProperty(endSize_.X);
            control_->setHeightProperty(endSize_.Y);
            control_->setHueProperty(endHue_);
            transitionStartTime_ = 0.0;
            transitionActive_ = false;
            TransitionComplete.Raise(this, System::EventArgs::Empty);
        }
        else
        {
            control_->setLeftProperty(static_cast<int>(startPosition_.X +
                (endPosition_.X - startPosition_.X) * percentComplete));
            control_->setTopProperty(static_cast<int>(startPosition_.Y +
                (endPosition_.Y - startPosition_.Y) * percentComplete));
            control_->setWidthProperty(static_cast<int>(startSize_.X +
                (endSize_.X - startSize_.X) * percentComplete));
            control_->setHeightProperty(static_cast<int>(startSize_.Y +
                (endSize_.Y - startSize_.Y) * percentComplete));
            const Vector4 curHue = startHue_.ToVector4() +
                (endHue_.ToVector4() - startHue_.ToVector4()) * percentComplete;
            control_->setHueProperty(Color(curHue));
        }
    }

    std::shared_ptr<Transition> Transition::CreateFadeIn(IControl& control, std::optional<float> length)
    {
        Color startHue = control.getHueProperty();
        const Color endHue = control.getHueProperty();
        startHue.setAProperty(0);
        auto transition = std::make_shared<Transition>(std::nullopt, std::nullopt, std::nullopt,
            std::nullopt, startHue, endHue);
        transition->setControlProperty(&control);
        if (length) transition->setTransitionLengthProperty(*length);
        return transition;
    }

    std::shared_ptr<Transition> Transition::CreateFadeOut(IControl& control, std::optional<float> length)
    {
        const Color startHue = control.getHueProperty();
        Color endHue = control.getHueProperty();
        endHue.setAProperty(0);
        auto transition = std::make_shared<Transition>(std::nullopt, std::nullopt, std::nullopt,
            std::nullopt, startHue, endHue);
        transition->setControlProperty(&control);
        if (length) transition->setTransitionLengthProperty(*length);
        return transition;
    }

    std::shared_ptr<Transition> Transition::CreateFlyIn(
        IControl& control, Point startPos, std::optional<float> length)
    {
        auto transition = std::make_shared<Transition>(startPos, std::nullopt, std::nullopt,
            std::nullopt, std::nullopt, std::nullopt);
        transition->setControlProperty(&control);
        if (length) transition->setTransitionLengthProperty(*length);
        return transition;
    }

    std::shared_ptr<Transition> Transition::CreateFlyOut(
        IControl& control, Point endPos, std::optional<float> length)
    {
        auto transition = std::make_shared<Transition>(std::nullopt, endPos, std::nullopt,
            std::nullopt, std::nullopt, std::nullopt);
        transition->setControlProperty(&control);
        if (length) transition->setTransitionLengthProperty(*length);
        return transition;
    }

    const std::string& Transition::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Transitions.Transition";
        return name;
    }
}
