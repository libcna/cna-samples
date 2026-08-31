// SPDX-License-Identifier: MS-PL
#include "Container.hpp"

#include <algorithm>
#include <utility>

namespace DynamicMenu::Controls
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;

    std::vector<std::shared_ptr<IControl>>& Container::getControlsProperty() { return controls_; }
    const std::vector<std::shared_ptr<IControl>>& Container::getControlsProperty() const { return controls_; }

    void Container::AddControl(std::shared_ptr<IControl> control)
    {
        controls_.push_back(control);
        control->setParentProperty(this);
    }

    void Container::RemoveControl(const std::shared_ptr<IControl>& control)
    {
        std::erase(controls_, control);
    }

    void Container::MarkForAdd(std::shared_ptr<IControl> control)
    {
        markedForAdd_.push_back(std::move(control));
    }

    void Container::MarkForRemove(std::shared_ptr<IControl> control)
    {
        markedForRemove_.push_back(std::move(control));
    }

    void Container::Initialize()
    {
        for (const auto& control : controls_)
        {
            control->setParentProperty(this);
            dynamic_cast<Control&>(*control).Initialize();
        }
    }

    void Container::LoadContent(GraphicsDevice& graphics, ContentManager& content)
    {
        Control::LoadContent(graphics, content);
        for (const auto& control : controls_)
            dynamic_cast<Control&>(*control).LoadContent(graphics, content);
    }

    void Container::Update(const GameTime& gameTime, const std::vector<GestureSample>& gestures)
    {
        Control::Update(gameTime, gestures);
        if (getVisibleProperty())
        {
            for (const auto& control : controls_)
            {
                auto& concrete = dynamic_cast<Control&>(*control);
                if (concrete.getVisibleProperty()) concrete.Update(gameTime, gestures);
            }
        }
        for (const auto& control : markedForRemove_) RemoveControl(control);
        markedForRemove_.clear();
        for (const auto& control : markedForAdd_) AddControl(control);
        markedForAdd_.clear();
    }

    void Container::Draw(const GameTime& gameTime, SpriteBatch& spriteBatch)
    {
        Control::Draw(gameTime, spriteBatch);
        if (getVisibleProperty())
        {
            for (const auto& control : controls_)
            {
                auto& concrete = dynamic_cast<Control&>(*control);
                if (concrete.getVisibleProperty()) concrete.Draw(gameTime, spriteBatch);
            }
        }
    }

    std::shared_ptr<IControl> Container::FindControlByName(const std::string& name) const
    {
        for (const auto& control : controls_)
            if (control->getNameProperty() == name) return control;
        return nullptr;
    }

    const std::string& Container::GetTypeName() const
    {
        static const std::string name = "DynamicMenu.Controls.Container";
        return name;
    }
}
