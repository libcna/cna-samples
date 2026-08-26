// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SampleGrid.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "System/IDisposable.hpp"

namespace PerPixelLightingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief A reference grid drawn with BasicEffect, to make the scene's orientation readable.
     */
    class SampleGrid : public System::IDisposable
    {
    public:
        /** @brief Constructs a grid with the default size, scale and colour. */
        SampleGrid();

        /** @brief Releases the grid's graphics resources. */
        ~SampleGrid() override;

        /**
         * @brief Get the colour every grid line is built with.
         * @return The grid colour.
         */
        [[nodiscard]] Color getGridColorProperty() const;

        /**
         * @brief Set the colour every grid line is built with.
         * @param value The grid colour.
         */
        void setGridColorProperty(Color value);

        /**
         * @brief Get the number of grid divisions per axis.
         * @return The grid size.
         */
        [[nodiscard]] int getGridSizeProperty() const;

        /**
         * @brief Set the number of grid divisions per axis.
         * @param value The grid size.
         */
        void setGridSizeProperty(int value);

        /**
         * @brief Get the world-space length of one grid division.
         * @return The grid scale.
         */
        [[nodiscard]] float getGridScaleProperty() const;

        /**
         * @brief Set the world-space length of one grid division.
         * @param value The grid scale.
         */
        void setGridScaleProperty(float value);

        /**
         * @brief Get the projection matrix the grid is drawn with.
         * @return The projection matrix.
         */
        [[nodiscard]] Matrix getProjectionMatrixProperty() const;

        /**
         * @brief Set the projection matrix the grid is drawn with.
         * @param value The projection matrix.
         */
        void setProjectionMatrixProperty(const Matrix& value);

        /**
         * @brief Get the world matrix the grid is drawn with.
         * @return The world matrix.
         */
        [[nodiscard]] Matrix getWorldMatrixProperty() const;

        /**
         * @brief Set the world matrix the grid is drawn with.
         * @param value The world matrix.
         */
        void setWorldMatrixProperty(const Matrix& value);

        /**
         * @brief Get the view matrix the grid is drawn with.
         * @return The view matrix.
         */
        [[nodiscard]] Matrix getViewMatrixProperty() const;

        /**
         * @brief Set the view matrix the grid is drawn with.
         * @param value The view matrix.
         */
        void setViewMatrixProperty(const Matrix& value);

        /** @brief Releases the vertex buffer and effect this grid owns. */
        void UnloadGraphicsContent();

        /**
         * @brief Builds the grid's line geometry and its BasicEffect.
         * @param graphicsDevice The device the resources are created on.
         */
        void LoadGraphicsContent(GraphicsDevice& graphicsDevice);

        /** @brief Releases the grid's graphics resources. */
        void Dispose() override;

        /** @brief Draws the grid with its current world, view and projection matrices. */
        void Draw();

    private:
        void Dispose(bool disposing);

        int gridSize;
        float gridScale;
        Color gridColor;
        bool isDisposed = false;

        // Rendering
        std::unique_ptr<VertexBuffer> vertexBuffer;
        int vertexCount = 0;
        int primitiveCount = 0;
        std::unique_ptr<BasicEffect> effect;
        Matrix projection, view, world;
        GraphicsDevice* device = nullptr;
    };
}
