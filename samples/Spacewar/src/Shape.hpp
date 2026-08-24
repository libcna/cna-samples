// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "System/TimeSpan.hpp"

namespace Microsoft::Xna::Framework
{
    class Game;
}

namespace Spacewar
{
    class Shape
    {
    public:
        explicit Shape(Microsoft::Xna::Framework::Game* game);
        virtual ~Shape() = default;

        [[nodiscard]] const Microsoft::Xna::Framework::Matrix& getWorldProperty() const;
        void setWorldProperty(const Microsoft::Xna::Framework::Matrix& value);
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getPositionProperty() const;
        void setPositionProperty(const Microsoft::Xna::Framework::Vector3& value);

        virtual void Create() = 0;
        virtual void Render();
        virtual void Update(System::TimeSpan time, System::TimeSpan elapsedTime);
        [[nodiscard]] std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> Plane(int columns, int rows);
        virtual void OnCreateDevice();
        virtual void Dispose();

    protected:
        [[nodiscard]] Microsoft::Xna::Framework::Game* getGameInstanceProperty() const;

        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> buffer_;
        Microsoft::Xna::Framework::Matrix world_;
        Microsoft::Xna::Framework::Vector3 position_;

    private:
        Microsoft::Xna::Framework::Game* game_;
    };
}
