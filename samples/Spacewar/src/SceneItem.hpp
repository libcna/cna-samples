// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/TimeSpan.hpp"

namespace Microsoft::Xna::Framework
{
    class Game;
}

namespace Spacewar
{
    class Shape;

    class SceneItem
    {
    public:
        explicit SceneItem(Microsoft::Xna::Framework::Game* game);
        SceneItem(Microsoft::Xna::Framework::Game* game, std::unique_ptr<Shape> shape,
                  Microsoft::Xna::Framework::Vector3 initialPosition);
        SceneItem(Microsoft::Xna::Framework::Game* game, std::unique_ptr<Shape> shape);
        SceneItem(Microsoft::Xna::Framework::Game* game, Microsoft::Xna::Framework::Vector3 initialPosition);
        virtual ~SceneItem();

        [[nodiscard]] bool getDeleteProperty() const;
        void setDeleteProperty(bool value);
        [[nodiscard]] float getRadiusProperty() const;
        void setRadiusProperty(float value);
        [[nodiscard]] Shape* getShapeItemProperty() const;
        void setShapeItemProperty(std::unique_ptr<Shape> value);
        [[nodiscard]] bool getPausedProperty() const;
        void setPausedProperty(bool value);
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getAccelerationProperty() const;
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getVelocityProperty() const;
        void setVelocityProperty(Microsoft::Xna::Framework::Vector3 value);
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getPositionProperty() const;
        void setPositionProperty(Microsoft::Xna::Framework::Vector3 value);
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getRotationProperty() const;
        void setRotationProperty(Microsoft::Xna::Framework::Vector3 value);
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getCenterProperty() const;
        void setCenterProperty(Microsoft::Xna::Framework::Vector3 value);
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getScaleProperty() const;
        void setScaleProperty(Microsoft::Xna::Framework::Vector3 value);

        SceneItem* Add(std::unique_ptr<SceneItem> childItem);
        [[nodiscard]] std::size_t getCountProperty() const;
        [[nodiscard]] SceneItem* operator[](std::size_t index) const;
        [[nodiscard]] const std::vector<std::unique_ptr<SceneItem>>& getChildrenProperty() const;
        virtual void Clear();
        virtual void Update(System::TimeSpan time, System::TimeSpan elapsedTime);
        virtual void Render();
        virtual bool Collide(SceneItem* item);
        virtual void OnCreateDevice();

    protected:
        [[nodiscard]] Microsoft::Xna::Framework::Game* getGameInstanceProperty() const;

        float radius_ = 0.0f;
        bool delete_ = false;
        SceneItem* root_ = nullptr;
        SceneItem* parent_ = nullptr;
        std::unique_ptr<Shape> shape_;
        Microsoft::Xna::Framework::Vector3 position_;
        Microsoft::Xna::Framework::Vector3 velocity_;
        Microsoft::Xna::Framework::Vector3 acceleration_;
        Microsoft::Xna::Framework::Vector3 rotation_;
        Microsoft::Xna::Framework::Vector3 scale_{1.0f, 1.0f, 1.0f};
        Microsoft::Xna::Framework::Vector3 center_;

    private:
        bool paused_ = false;
        Microsoft::Xna::Framework::Game* game_;
        std::vector<std::unique_ptr<SceneItem>> children_;
        std::vector<std::unique_ptr<SceneItem>> retainedChildren_;
    };
}
