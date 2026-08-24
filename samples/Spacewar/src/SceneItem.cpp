// SPDX-License-Identifier: MS-PL

#include "SceneItem.hpp"

#include <algorithm>

#include "Shape.hpp"
#include "Ship.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    SceneItem::SceneItem(Game* game)
        : game_(game)
    {
    }

    SceneItem::SceneItem(Game* game, std::unique_ptr<Shape> shape, Vector3 initialPosition)
        : shape_(std::move(shape)), position_(initialPosition), game_(game)
    {
    }

    SceneItem::SceneItem(Game* game, std::unique_ptr<Shape> shape)
        : shape_(std::move(shape)), game_(game)
    {
    }

    SceneItem::SceneItem(Game* game, Vector3 initialPosition)
        : position_(initialPosition), game_(game)
    {
    }

    SceneItem::~SceneItem() = default;

    bool SceneItem::getDeleteProperty() const { return delete_; }
    void SceneItem::setDeleteProperty(bool value) { delete_ = value; }
    float SceneItem::getRadiusProperty() const { return radius_; }
    void SceneItem::setRadiusProperty(float value) { radius_ = value; }
    Shape* SceneItem::getShapeItemProperty() const { return shape_.get(); }
    void SceneItem::setShapeItemProperty(std::unique_ptr<Shape> value) { shape_ = std::move(value); }
    bool SceneItem::getPausedProperty() const { return paused_; }
    void SceneItem::setPausedProperty(bool value) { paused_ = value; }
    const Vector3& SceneItem::getAccelerationProperty() const { return acceleration_; }
    const Vector3& SceneItem::getVelocityProperty() const { return velocity_; }
    void SceneItem::setVelocityProperty(Vector3 value) { velocity_ = value; }
    const Vector3& SceneItem::getPositionProperty() const { return position_; }
    void SceneItem::setPositionProperty(Vector3 value) { position_ = value; }
    const Vector3& SceneItem::getRotationProperty() const { return rotation_; }
    void SceneItem::setRotationProperty(Vector3 value) { rotation_ = value; }
    const Vector3& SceneItem::getCenterProperty() const { return center_; }
    void SceneItem::setCenterProperty(Vector3 value) { center_ = value; }
    const Vector3& SceneItem::getScaleProperty() const { return scale_; }
    void SceneItem::setScaleProperty(Vector3 value) { scale_ = value; }
    Game* SceneItem::getGameInstanceProperty() const { return game_; }

    SceneItem* SceneItem::Add(std::unique_ptr<SceneItem> childItem)
    {
        childItem->parent_ = this;
        childItem->root_ = root_ == nullptr ? this : root_;
        SceneItem* result = childItem.get();
        children_.push_back(std::move(childItem));
        return result;
    }

    std::size_t SceneItem::getCountProperty() const { return children_.size(); }
    SceneItem* SceneItem::operator[](std::size_t index) const { return children_.at(index).get(); }
    const std::vector<std::unique_ptr<SceneItem>>& SceneItem::getChildrenProperty() const { return children_; }
    void SceneItem::Clear() { children_.clear(); }

    void SceneItem::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (!paused_)
        {
            velocity_ += Vector3::Multiply(acceleration_, static_cast<float>(elapsedTime.getTotalSecondsProperty()));
            position_ += Vector3::Multiply(velocity_, static_cast<float>(elapsedTime.getTotalSecondsProperty()));
        }

        if (shape_)
        {
            shape_->setWorldProperty(
                Matrix::CreateTranslation(-center_) * Matrix::CreateScale(scale_) *
                Matrix::CreateRotationX(rotation_.X) * Matrix::CreateRotationY(rotation_.Y) *
                Matrix::CreateRotationZ(rotation_.Z) * Matrix::CreateTranslation(position_ + center_));
            if (!paused_)
                shape_->Update(time, elapsedTime);
        }

        for (const auto& item : children_)
            item->Update(time, elapsedTime);

        for (auto it = children_.begin(); it != children_.end();)
        {
            if ((*it)->delete_)
            {
                retainedChildren_.push_back(std::move(*it));
                it = children_.erase(it);
            }
            else
                ++it;
        }
    }

    void SceneItem::Render()
    {
        if (shape_)
            shape_->Render();
        for (const auto& item : children_)
            item->Render();
    }

    bool SceneItem::Collide(SceneItem* item)
    {
        if ((position_ - item->position_).Length() < radius_ + item->radius_)
            return true;

        if (auto* shipItem = dynamic_cast<Ship*>(item); shipItem && shipItem->getExtendedExtentProperty())
        {
            const Matrix localRotation = Matrix::CreateRotationZ(shipItem->getRotationProperty().Z);
            for (const Vector3& extent : *shipItem->getExtendedExtentProperty())
            {
                const Vector4 extendedPosition = Vector4::Transform(extent, localRotation);
                const Vector3 localPosition = shipItem->getPositionProperty() +
                    Vector3(extendedPosition.X, extendedPosition.Y, extendedPosition.Z);
                if ((getPositionProperty() - localPosition).Length() < radius_ + item->getRadiusProperty())
                    return true;
            }
        }

        if (auto* ship = dynamic_cast<Ship*>(this); ship && ship->getExtendedExtentProperty())
        {
            const Matrix localRotation = Matrix::CreateRotationZ(ship->getRotationProperty().Z);
            for (const Vector3& extent : *ship->getExtendedExtentProperty())
            {
                const Vector4 extendedPosition = Vector4::Transform(extent, localRotation);
                const Vector3 localPosition = ship->getPositionProperty() +
                    Vector3(extendedPosition.X, extendedPosition.Y, extendedPosition.Z);
                if ((localPosition - item->getPositionProperty()).Length() < radius_ + item->getRadiusProperty())
                    return true;
            }
        }
        return false;
    }

    void SceneItem::OnCreateDevice()
    {
    }
}
