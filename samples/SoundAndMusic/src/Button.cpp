// SPDX-License-Identifier: MS-PL

#include "Button.hpp"

#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "SoundAndMusicSampleGame.hpp"

namespace SoundAndMusicSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;

    Button::Button(const std::string& textureName, Game& game)
        : DrawableGameComponent(game),
          spriteBatch_(),
          isTouched_(false),
          dragStarted_(false),
          assetName_(textureName),
          texture_(),
          dragRestrictions_(),
          positionOfOrigin_(Vector2::Zero),
          positionOrigin_(Vector2::Zero),
          tintColor_(Color::White),
          tintWhenTouched_(Color::DarkGray),
          tintWhenDisabled_(Color(0.3f, 0.3f, 0.3f, 0.3f)),
          allowDrag_(false)
    {
    }

    Button::~Button() = default;

    const Rectangle& Button::getDragRestrictionsProperty() const
    {
        return dragRestrictions_;
    }

    void Button::setDragRestrictionsProperty(const Rectangle& value)
    {
        dragRestrictions_ = value;
    }

    const Vector2& Button::getPositionOfOriginProperty() const
    {
        return positionOfOrigin_;
    }

    void Button::setPositionOfOriginProperty(const Vector2& value)
    {
        if (positionOfOrigin_ != value)
        {
            positionOfOrigin_ = value;
            OnPositionChanged(System::EventArgs::Empty);
        }
    }

    const Vector2& Button::getPositionOriginProperty() const
    {
        return positionOrigin_;
    }

    void Button::setPositionOriginProperty(const Vector2& value)
    {
        positionOrigin_ = value;
    }

    Vector2 Button::getPositionForDrawProperty() const
    {
        return positionOfOrigin_ - positionOrigin_;
    }

    Vector2 Button::getTopLeftPositionProperty() const
    {
        return positionOfOrigin_ - positionOrigin_;
    }

    void Button::setTopLeftPositionProperty(const Vector2& value)
    {
        setPositionOfOriginProperty(value + positionOrigin_);
    }

    Color Button::getTintColorProperty() const
    {
        if (getEnabledProperty())
        {
            if (isTouched_ && tintWhenTouched_.has_value())
            {
                return *tintWhenTouched_;
            }
            return tintColor_;
        }

        return tintWhenDisabled_.value_or(tintColor_);
    }

    void Button::setTintColorProperty(const Color& value)
    {
        tintColor_ = value;
    }

    Vector2 Button::getTextureCenterProperty() const
    {
        return Vector2(
            static_cast<float>(texture_->getWidthProperty() / 2),
            static_cast<float>(texture_->getHeightProperty() / 2));
    }

    Rectangle Button::getScreenBoundsProperty() const
    {
        const Vector2 topLeft = getTopLeftPositionProperty();
        return Rectangle(
            static_cast<int>(topLeft.X),
            static_cast<int>(topLeft.Y),
            texture_->getWidthProperty(),
            texture_->getHeightProperty());
    }

    const std::optional<Color>& Button::getTintWhenTouchedProperty() const
    {
        return tintWhenTouched_;
    }

    void Button::setTintWhenTouchedProperty(const std::optional<Color>& value)
    {
        tintWhenTouched_ = value;
    }

    const std::optional<Color>& Button::getTintWhenDisabledProperty() const
    {
        return tintWhenDisabled_;
    }

    void Button::setTintWhenDisabledProperty(const std::optional<Color>& value)
    {
        tintWhenDisabled_ = value;
    }

    bool Button::getAllowDragProperty() const
    {
        return allowDrag_;
    }

    void Button::setAllowDragProperty(bool value)
    {
        allowDrag_ = value;
    }

    void Button::OnClick(const System::EventArgs& e)
    {
        Click.Invoke(this, e);
    }

    void Button::OnTouchDown(const System::EventArgs& e)
    {
        TouchDown.Invoke(this, e);
    }

    void Button::OnPositionChanged(const System::EventArgs& e)
    {
        PositionChanged.Invoke(this, e);
    }

    void Button::Initialize()
    {
        DrawableGameComponent::Initialize();
        setPositionOriginProperty(getTextureCenterProperty());
    }

    void Button::LoadContent()
    {
        texture_ = std::make_unique<Texture2D>(
            getGameProperty().getContentProperty().Load<Texture2D>(assetName_));
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        DrawableGameComponent::LoadContent();
    }

    void Button::Update(GameTime& gameTime)
    {
        std::optional<Rectangle> touchRect = GetTouchRect();

        if (CheckIfFirstTouchDown(touchRect))
        {
            DoOnTouchDown();
        }
        else if (isTouched_ && CheckIfTouchRelease(touchRect))
        {
            DoOnTouchRelease();
        }
        else if (dragStarted_)
        {
            DoOnDrag(touchRect.value_or(Rectangle()));
        }
        else if (!CheckIfStillTouching(touchRect))
        {
            DoOnNotTouching();
        }

        DrawableGameComponent::Update(gameTime);
    }

    void Button::Draw(const GameTime& gameTime)
    {
        spriteBatch_->Begin();
        spriteBatch_->Draw(*texture_, getPositionForDrawProperty(), getTintColorProperty());
        spriteBatch_->End();

        DrawableGameComponent::Draw(gameTime);
    }

    const std::string& Button::GetTypeName() const
    {
        static const std::string typeName = "SoundAndMusicSample.Button";
        return typeName;
    }

    std::optional<Rectangle> Button::GetTouchRect() const
    {
        const auto& game = static_cast<const SoundAndMusicSampleGame&>(getGameProperty());
        if (game.touchLocation.has_value() &&
            game.touchLocation->getStateProperty() != TouchLocationState::Invalid)
        {
            const Vector2& position = game.touchLocation->getPositionProperty();
            return Rectangle(
                static_cast<int>(position.X) - 5,
                static_cast<int>(position.Y) - 5,
                10,
                10);
        }

        return std::nullopt;
    }

    bool Button::CheckIfFirstTouchDown(const std::optional<Rectangle>& touchRect) const
    {
        if (!isTouched_ && touchRect.has_value())
        {
            return getScreenBoundsProperty().Intersects(*touchRect);
        }
        return false;
    }

    bool Button::CheckIfStillTouching(const std::optional<Rectangle>& touchRect) const
    {
        if (touchRect.has_value())
        {
            return getScreenBoundsProperty().Intersects(*touchRect);
        }
        return false;
    }

    bool Button::CheckIfTouchRelease(const std::optional<Rectangle>& touchRect) const
    {
        return isTouched_ && !touchRect.has_value();
    }

    void Button::DoOnTouchDown()
    {
        OnTouchDown(System::EventArgs::Empty);
        isTouched_ = true;

        if (allowDrag_)
        {
            dragStarted_ = true;
        }
    }

    void Button::DoOnNotTouching()
    {
        if (isTouched_)
        {
            dragStarted_ = false;
            isTouched_ = false;

            if (dragStarted_)
            {
                dragStarted_ = false;
            }
        }
    }

    void Button::DoOnTouchRelease()
    {
        if (isTouched_)
        {
            DoOnNotTouching();
            OnClick(System::EventArgs::Empty);
        }
    }

    void Button::DoOnDrag(const Rectangle& touchRect)
    {
        if (isTouched_)
        {
            const float newX = MathHelper::Clamp(
                static_cast<float>(touchRect.getCenterProperty().X),
                static_cast<float>(dragRestrictions_.getLeftProperty()),
                static_cast<float>(dragRestrictions_.getRightProperty()));
            const float newY = MathHelper::Clamp(
                static_cast<float>(touchRect.getCenterProperty().Y),
                static_cast<float>(dragRestrictions_.getTopProperty()),
                static_cast<float>(dragRestrictions_.getBottomProperty()));

            setPositionOfOriginProperty(Vector2(newX, newY));
        }
        else
        {
            dragStarted_ = false;
        }
    }
}
