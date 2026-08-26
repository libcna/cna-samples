// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SampleGrid.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "SampleGrid.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPassCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace VertexLightingSample
{
    SampleGrid::SampleGrid()
        : gridSize(16)
        , gridScale(32.0f)
        , gridColor(0xFF, 0xFF, 0xFF, 0xFF)
    {
        world = Matrix::getIdentityProperty();
        view = Matrix::getIdentityProperty();
        projection = Matrix::getIdentityProperty();
    }

    SampleGrid::~SampleGrid()
    {
        // The original's finalizer calls Dispose(false); a C++ destructor runs at the same
        // point the object stops being reachable, so this is the whole of that pattern.
        Dispose(false);
    }

    Color SampleGrid::getGridColorProperty() const { return gridColor; }

    void SampleGrid::setGridColorProperty(Color value) { gridColor = value; }

    int SampleGrid::getGridSizeProperty() const { return gridSize; }

    void SampleGrid::setGridSizeProperty(int value) { gridSize = value; }

    float SampleGrid::getGridScaleProperty() const { return gridScale; }

    void SampleGrid::setGridScaleProperty(float value) { gridScale = value; }

    Matrix SampleGrid::getProjectionMatrixProperty() const { return projection; }

    void SampleGrid::setProjectionMatrixProperty(const Matrix& value) { projection = value; }

    Matrix SampleGrid::getWorldMatrixProperty() const { return world; }

    void SampleGrid::setWorldMatrixProperty(const Matrix& value) { world = value; }

    Matrix SampleGrid::getViewMatrixProperty() const { return view; }

    void SampleGrid::setViewMatrixProperty(const Matrix& value) { view = value; }

    void SampleGrid::UnloadGraphicsContent()
    {
        vertexBuffer.reset();
        effect.reset();
    }

    void SampleGrid::LoadGraphicsContent(GraphicsDevice& graphicsDevice)
    {
        device = &graphicsDevice;

        effect = std::make_unique<BasicEffect>(*device);
        int gridSize1 = this->gridSize + 1;
        this->primitiveCount = gridSize1 * 2;
        this->vertexCount = this->primitiveCount * 2;

        std::vector<VertexPositionColor> vertices(static_cast<std::size_t>(this->vertexCount));

        float length = (float)gridSize * gridScale;
        float halfLength = length * 0.5f;

        int index = 0;

        for (int i = 0; i < gridSize1; ++i)
        {
            vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(Vector3(
                -halfLength, 0.0f, i * this->gridScale - halfLength), this->gridColor);
            vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(Vector3(
                halfLength, 0.0f, i * this->gridScale - halfLength), this->gridColor);
            vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(Vector3(
                i * this->gridScale - halfLength, 0.0f, -halfLength), this->gridColor);
            vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(Vector3(
                i * this->gridScale - halfLength, 0.0f, halfLength), this->gridColor);
        }

        this->vertexBuffer = std::make_unique<VertexBuffer>(
            *device, VertexPositionColor::getVertexDeclarationStatic(),
            this->vertexCount, BufferUsage::WriteOnly);
        this->vertexBuffer->SetData(vertices.data(), this->vertexCount);
    }

    void SampleGrid::Dispose()
    {
        Dispose(true);
    }

    void SampleGrid::Dispose(bool disposing)
    {
        if (!isDisposed)
        {
            if (disposing)
            {
                // if we're manually disposing,
                // then managed content should be unloaded
                UnloadGraphicsContent();
            }
            isDisposed = true;
        }
    }

    void SampleGrid::Draw()
    {
        effect->setWorldProperty(world);
        effect->setViewProperty(view);
        effect->setProjectionProperty(projection);
        effect->VertexColorEnabled = true;
        effect->setLightingEnabledProperty(false);

        device->SetVertexBuffer(this->vertexBuffer.get());

        EffectPassCollection& passes = effect->getCurrentTechniqueProperty()->getPassesProperty();
        for (int i = 0; i < passes.getCountProperty(); ++i)
        {
            passes[i].Apply();
            device->DrawPrimitives(PrimitiveType::LineList, 0, this->primitiveCount);
        }
    }
}
