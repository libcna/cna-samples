// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PrimitiveBatch.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "System/IDisposable.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/NotSupportedException.hpp"

namespace PathDrawing
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief PrimitiveBatch is a class that handles efficient rendering automatically for
     *        its users, in a similar way to SpriteBatch.
     *
     * PrimitiveBatch can render lines, points, and triangles to the screen.
     */
    class PrimitiveBatch : public System::IDisposable
    {
        // this constant controls how large the vertices buffer is. Larger buffers will
        // require flushing less often, which can increase performance. However, having
        // buffer that is unnecessarily large will waste memory.
        static constexpr int DefaultBufferSize = 500;

        // a block of vertices that calling AddVertex will fill. Flush will draw using
        // this array, and will determine how many primitives to draw from
        // positionInBuffer.
        std::vector<VertexPositionColor> vertices =
            std::vector<VertexPositionColor>(DefaultBufferSize);

        // keeps track of how many vertices have been added. this value increases until
        // we run out of space in the buffer, at which time Flush is automatically
        // called.
        int positionInBuffer = 0;

        // the vertex declaration that will be set on the device for drawing. this is
        // created automatically using VertexPositionColor's vertex elements.
        VertexDeclaration vertexDeclaration;

        // a basic effect, which contains the shaders that we will use to draw our
        // primitives.
        BasicEffect basicEffect;

        // the device that we will issue draw calls to.
        GraphicsDevice& device;

        // this value is set by Begin, and is the type of primitives that we are
        // drawing.
        PrimitiveType primitiveType = PrimitiveType::LineList;

        // how many verts does each of these primitives take up? points are 1,
        // lines are 2, and triangles are 3.
        int numVertsPerPrimitive = 2;

        // hasBegun is flipped to true once Begin is called, and is used to make
        // sure users don't call End before Begin is called.
        bool hasBegun = false;

        bool isDisposed = false;

    public:
        /**
         * @brief Creates a new PrimitiveBatch and sets up all of the internals it will need.
         * @param graphicsDevice The device draw calls are issued to.
         */
        explicit PrimitiveBatch(GraphicsDevice& graphicsDevice)
            : vertexDeclaration(VertexPositionColor::getVertexDeclarationStatic()),
              basicEffect(graphicsDevice),
              device(graphicsDevice)
        {
            // set up a new basic effect, and enable vertex colors.
            basicEffect.VertexColorEnabled = true;

            // projection uses CreateOrthographicOffCenter to create 2d projection
            // matrix with 0,0 in the upper left.
            basicEffect.setProjectionProperty(Matrix::CreateOrthographicOffCenter(
                0.0f, (float)graphicsDevice.getViewportProperty().getWidthProperty(),
                (float)graphicsDevice.getViewportProperty().getHeightProperty(), 0.0f,
                0.0f, 1.0f));
        }

        /**
         * @brief Releases the effect and vertex declaration this batch owns.
         */
        void Dispose() override
        {
            this->Dispose(true);
        }

    protected:
        /**
         * @brief Releases the resources this batch owns.
         * @param disposing True when called from Dispose() rather than a finalizer.
         */
        virtual void Dispose(bool disposing)
        {
            if (disposing && !isDisposed)
            {
                basicEffect.Dispose();

                isDisposed = true;
            }
        }

    public:
        /**
         * @brief Tells the PrimitiveBatch what kind of primitives will be drawn, and prepares
         *        the graphics card to render those primitives.
         *
         * @param primitiveType The kind of primitive that AddVertex will build.
         * @throws System::InvalidOperationException if Begin is called twice without an End.
         * @throws System::NotSupportedException for the strip primitive types.
         */
        void Begin(PrimitiveType primitiveType)
        {
            if (hasBegun)
            {
                throw System::InvalidOperationException(
                    "End must be called before Begin can be called again.");
            }

            // these three types reuse vertices, so we can't flush properly without more
            // complex logic. Since that's a bit too complicated for this sample, we'll
            // simply disallow them.
            if (primitiveType == PrimitiveType::LineStrip ||
                primitiveType == PrimitiveType::TriangleStrip)
            {
                throw System::NotSupportedException(
                    "The specified primitiveType is not supported by PrimitiveBatch.");
            }

            this->primitiveType = primitiveType;

            // how many verts will each of these primitives require?
            this->numVertsPerPrimitive = NumVertsPerPrimitive(primitiveType);

            // prepare the graphics device for drawing by setting the vertex declaration
            // and telling our basic effect to begin.
            basicEffect.getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

            // flip the error checking boolean. It's now ok to call AddVertex, Flush,
            // and End.
            hasBegun = true;
        }

        /**
         * @brief Adds another vertex to be rendered.
         *
         * To draw a point, AddVertex must be called once; for lines, twice; and for
         * triangles three times. This function can only be called once Begin has been
         * called. If there is not enough room in the vertices buffer, Flush is called
         * automatically.
         *
         * @param vertex Position of the vertex, in screen space.
         * @param color Color of the vertex.
         * @throws System::InvalidOperationException if Begin has not been called.
         */
        void AddVertex(Vector2 vertex, Color color)
        {
            if (!hasBegun)
            {
                throw System::InvalidOperationException(
                    "Begin must be called before AddVertex can be called.");
            }

            // are we starting a new primitive? if so, and there will not be enough room
            // for a whole primitive, flush.
            bool newPrimitive = ((positionInBuffer % numVertsPerPrimitive) == 0);

            if (newPrimitive &&
                (positionInBuffer + numVertsPerPrimitive) >= (int)vertices.size())
            {
                Flush();
            }

            // once we know there's enough room, set the vertex in the buffer,
            // and increase position.
            vertices[positionInBuffer].Position = Vector3(vertex, 0.0f);
            vertices[positionInBuffer].Color = color;

            positionInBuffer++;
        }

        /**
         * @brief Called once all the primitives have been drawn using AddVertex.
         *
         * It will call Flush to actually submit the draw call to the graphics card, and
         * then tell the basic effect to end.
         *
         * @throws System::InvalidOperationException if Begin has not been called.
         */
        void End()
        {
            if (!hasBegun)
            {
                throw System::InvalidOperationException(
                    "Begin must be called before End can be called.");
            }

            // Draw whatever the user wanted us to draw
            Flush();

            hasBegun = false;
        }

    private:
        // Flush is called to issue the draw call to the graphics card. Once the draw
        // call is made, positionInBuffer is reset, so that AddVertex can start over
        // at the beginning. End will call this to draw the primitives that the user
        // requested, and AddVertex will call this if there is not enough room in the
        // buffer.
        void Flush()
        {
            if (!hasBegun)
            {
                throw System::InvalidOperationException(
                    "Begin must be called before Flush can be called.");
            }

            // how many primitives will we draw?
            int primitiveCount = positionInBuffer / numVertsPerPrimitive;

            // no work to do
            if (primitiveCount == 0)
            {
                return;
            }

            // submit the draw call to the graphics card
            // The C# generic overload takes the layout from VertexPositionColor.VertexDeclaration;
            // CNA's built-in vertex structures are not raw GPU streams, so the declaration this
            // batch already holds is passed explicitly to the matching typed overload.
            device.DrawUserPrimitives(primitiveType, vertices.data(), 0,
                                      primitiveCount, vertexDeclaration);

            // now that we've drawn, it's ok to reset positionInBuffer back to zero,
            // and write over any vertices that may have been set previously.
            positionInBuffer = 0;
        }

        // NumVertsPerPrimitive is a boring helper function that tells how many vertices
        // it will take to draw each kind of primitive.
        static int NumVertsPerPrimitive(PrimitiveType primitive)
        {
            int numVertsPerPrimitive;
            switch (primitive)
            {
                case PrimitiveType::LineList:
                    numVertsPerPrimitive = 2;
                    break;
                case PrimitiveType::TriangleList:
                    numVertsPerPrimitive = 3;
                    break;
                default:
                    throw System::InvalidOperationException("primitive is not valid");
            }
            return numVertsPerPrimitive;
        }
    };
}
