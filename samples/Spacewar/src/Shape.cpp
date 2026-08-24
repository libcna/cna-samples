// SPDX-License-Identifier: MS-PL

#include "Shape.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    Shape::Shape(Game* game)
        : game_(game)
    {
    }

    const Matrix& Shape::getWorldProperty() const { return world_; }
    void Shape::setWorldProperty(const Matrix& value) { world_ = value; }
    const Vector3& Shape::getPositionProperty() const { return position_; }
    void Shape::setPositionProperty(const Vector3& value) { position_ = value; }
    Game* Shape::getGameInstanceProperty() const { return game_; }

    void Shape::Render()
    {
    }

    void Shape::Update(System::TimeSpan, System::TimeSpan)
    {
    }

    std::unique_ptr<VertexBuffer> Shape::Plane(int columns, int rows)
    {
        auto result = std::make_unique<VertexBuffer>(
            game_->getGraphicsDeviceProperty(), VertexPositionColor::getVertexDeclarationStatic(),
            columns * rows * 6, BufferUsage::WriteOnly);

        std::vector<VertexPositionColor> data(static_cast<std::size_t>(columns * rows * 6));
        int pointCount = 0;
        for (int x = 0; x < columns; ++x)
        {
            for (int y = 0; y < rows; ++y)
            {
                const float left = static_cast<float>(x) / static_cast<float>(columns);
                const float right = static_cast<float>(x + 1) / static_cast<float>(columns);
                const float top = static_cast<float>(y) / static_cast<float>(rows);
                const float bottom = static_cast<float>(y + 1) / static_cast<float>(rows);
                data[pointCount + 0] = VertexPositionColor(Vector3(left, top, 0.0f), Color::White);
                data[pointCount + 1] = VertexPositionColor(Vector3(right, top, 0.0f), Color::White);
                data[pointCount + 2] = VertexPositionColor(Vector3(right, bottom, 0.0f), Color::White);
                data[pointCount + 3] = VertexPositionColor(Vector3(left, top, 0.0f), Color::White);
                data[pointCount + 4] = VertexPositionColor(Vector3(right, bottom, 0.0f), Color::White);
                data[pointCount + 5] = VertexPositionColor(Vector3(left, bottom, 0.0f), Color::White);
                pointCount += 6;
            }
        }
        result->SetData(data.data(), static_cast<int>(data.size()));
        return result;
    }

    void Shape::OnCreateDevice()
    {
        Create();
    }

    void Shape::Dispose()
    {
        if (buffer_)
            buffer_->Dispose();
        buffer_.reset();
    }
}
