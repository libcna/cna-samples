// SPDX-License-Identifier: MS-PL
#include "Control.hpp"

#include <algorithm>
#include <utility>

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Transitions/Transition.hpp"

namespace DynamicMenu::Controls
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;
    using DynamicMenu::Transitions::Transition;

    Control::Control() = default;
    Control::~Control() = default;
    int Control::getLeftProperty() const { return left_; }
    void Control::setLeftProperty(int value) { left_ = value; }
    int Control::getTopProperty() const { return top_; }
    void Control::setTopProperty(int value) { top_ = value; }
    int Control::getWidthProperty() const { return width_; }
    void Control::setWidthProperty(int value) { width_ = value; }
    int Control::getHeightProperty() const { return height_; }
    void Control::setHeightProperty(int value) { height_ = value; }
    int Control::getBottomProperty() const { return top_ + height_; }
    int Control::getRightProperty() const { return left_ + width_; }
    const std::string& Control::getNameProperty() const { return name_; }
    void Control::setNameProperty(std::string value) { name_ = std::move(value); }
    const std::string& Control::getBackTextureNameProperty() const { return backTextureName_; }
    void Control::setBackTextureNameProperty(std::string value) { backTextureName_ = std::move(value); }
    Texture2D* Control::getBackTextureProperty() { return backTexture_ ? &*backTexture_ : nullptr; }
    bool Control::getVisibleProperty() const { return visible_; }
    void Control::setVisibleProperty(bool value) { visible_ = value; }
    Color Control::getHueProperty() const { return hue_; }
    void Control::setHueProperty(Color value) { hue_ = value; }
    IControl* Control::getParentProperty() const { return parent_; }
    void Control::setParentProperty(IControl* value) { parent_ = value; }
    std::any& Control::getTagProperty() { return tag_; }
    const std::any& Control::getTagProperty() const { return tag_; }
    void Control::setTagProperty(std::any value) { tag_ = std::move(value); }

    void Control::Initialize() {}

    void Control::LoadContent(GraphicsDevice&, ContentManager& content)
    {
        if (!backTextureName_.empty()) backTexture_.emplace(content.Load<Texture2D>(backTextureName_));
    }

    void Control::Update(const GameTime& gameTime, const std::vector<GestureSample>&)
    {
        std::vector<std::shared_ptr<Transition>> toRemove;
        const auto curTransitions = activeTransitions_;
        for (const auto& transition : curTransitions)
        {
            transition->Update(gameTime);
            if (!transition->getTransitionActiveProperty()) toRemove.push_back(transition);
        }
        for (const auto& transition : toRemove) std::erase(activeTransitions_, transition);
    }

    void Control::Draw(const GameTime&, SpriteBatch& spriteBatch)
    {
        Texture2D* currTexture = GetCurrTexture();
        if (currTexture != nullptr)
        {
            spriteBatch.Draw(*currTexture, GetAbsoluteRect(), std::nullopt, hue_, 0.0f,
                             Vector2::Zero, SpriteEffects::None, 0.0f);
        }
    }

    Texture2D* Control::GetCurrTexture() { return getBackTextureProperty(); }

    Point Control::GetAbsoluteTopLeft() const
    {
        Point absoluteTopLeft(left_, top_);
        if (parent_ != nullptr)
        {
            const Point parentTopLeft = parent_->GetAbsoluteTopLeft();
            absoluteTopLeft.X += parentTopLeft.X;
            absoluteTopLeft.Y += parentTopLeft.Y;
        }
        return absoluteTopLeft;
    }

    Rectangle Control::GetAbsoluteRect() const
    {
        const Point topLeft = GetAbsoluteTopLeft();
        return Rectangle(topLeft.X, topLeft.Y, width_, height_);
    }

    void Control::ApplyTransition(std::shared_ptr<Transition> transition)
    {
        activeTransitions_.push_back(transition);
        transition->setControlProperty(this);
        transition->StartTranstion();
    }

    void Control::DrawCenteredText(
        SpriteBatch& spriteBatch, SpriteFont* font, Rectangle rect,
        const std::string& text, Color color)
    {
        if (font == nullptr || text.empty()) return;
        const Vector2 midPoint(static_cast<float>(rect.X + rect.Width / 2),
                               static_cast<float>(rect.Y + rect.Height / 2));
        const Vector2 stringSize = font->MeasureString(text);
        const Vector2 fontPos(midPoint.X - stringSize.X * 0.5f,
                              midPoint.Y - stringSize.Y * 0.5f);
        spriteBatch.DrawString(*font, text, fontPos, color);
    }

    bool Control::ContainsPos(Vector2 pos) const
    {
        return GetAbsoluteRect().Contains(static_cast<int>(pos.X), static_cast<int>(pos.Y));
    }
}
