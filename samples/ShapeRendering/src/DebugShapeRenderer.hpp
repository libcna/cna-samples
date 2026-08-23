// SPDX-License-Identifier: MS-PL

#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "System/InvalidOperationException.hpp"

namespace ShapeRenderingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /** @brief Batches and renders temporary line-based debug shapes. */
    class DebugShapeRenderer
    {
        struct DebugShape
        {
            std::vector<VertexPositionColor> Vertices;
            int   LineCount = 0;
            float Lifetime  = 0.0f;
        };

        inline static std::vector<std::shared_ptr<DebugShape>> cachedShapes_;
        inline static std::vector<std::shared_ptr<DebugShape>> activeShapes_;
        inline static std::vector<VertexPositionColor> verts_ =
            std::vector<VertexPositionColor>(64);
        inline static GraphicsDevice* graphics_ = nullptr;
        inline static std::unique_ptr<BasicEffect> effect_;
        inline static std::vector<Vector3> corners_ = std::vector<Vector3>(8);

        static constexpr int sphereResolution = 30;
        static constexpr int sphereLineCount  = (sphereResolution + 1) * 3;
        inline static std::vector<Vector3> unitSphere_;

        static void InitializeSphere()
        {
            unitSphere_.resize(sphereLineCount * 2);
            float step = MathHelper::TwoPi / sphereResolution;
            int index = 0;

            for (float a = 0.0f; a < MathHelper::TwoPi; a += step) {
                unitSphere_[index++] = Vector3(
                    static_cast<float>(std::cos(static_cast<double>(a))),
                    static_cast<float>(std::sin(static_cast<double>(a))), 0.0f);
                unitSphere_[index++] = Vector3(
                    static_cast<float>(std::cos(static_cast<double>(a + step))),
                    static_cast<float>(std::sin(static_cast<double>(a + step))), 0.0f);
            }
            for (float a = 0.0f; a < MathHelper::TwoPi; a += step) {
                unitSphere_[index++] = Vector3(
                    static_cast<float>(std::cos(static_cast<double>(a))), 0.0f,
                    static_cast<float>(std::sin(static_cast<double>(a))));
                unitSphere_[index++] = Vector3(
                    static_cast<float>(std::cos(static_cast<double>(a + step))), 0.0f,
                    static_cast<float>(std::sin(static_cast<double>(a + step))));
            }
            for (float a = 0.0f; a < MathHelper::TwoPi; a += step) {
                unitSphere_[index++] = Vector3(
                    0.0f, static_cast<float>(std::cos(static_cast<double>(a))),
                    static_cast<float>(std::sin(static_cast<double>(a))));
                unitSphere_[index++] = Vector3(
                    0.0f, static_cast<float>(std::cos(static_cast<double>(a + step))),
                    static_cast<float>(std::sin(static_cast<double>(a + step))));
            }
        }

        static std::shared_ptr<DebugShape> GetShapeForLines(int lineCount, float life)
        {
            std::shared_ptr<DebugShape> shape;
            int vertCount = lineCount * 2;

            for (int i = 0; i < static_cast<int>(cachedShapes_.size()); i++) {
                if (static_cast<int>(cachedShapes_[i]->Vertices.size()) >= vertCount) {
                    shape = cachedShapes_[i];
                    cachedShapes_.erase(cachedShapes_.begin() + i);
                    activeShapes_.push_back(shape);
                    break;
                }
            }

            if (!shape) {
                shape = std::make_shared<DebugShape>();
                shape->Vertices.resize(vertCount);
                activeShapes_.push_back(shape);
            }

            shape->LineCount = lineCount;
            shape->Lifetime  = life;
            return shape;
        }

    public:
        /**
         * @brief Initializes the renderer.
         *
         * @param graphicsDevice Graphics device used for rendering.
         */
        static void Initialize(GraphicsDevice& graphicsDevice)
        {
            if (graphics_ != nullptr)
                throw System::InvalidOperationException("Initialize can only be called once.");

            graphics_ = &graphicsDevice;

            effect_ = std::make_unique<BasicEffect>(graphicsDevice);
            effect_->VertexColorEnabled = true;
            effect_->setTextureEnabledProperty(false);
            effect_->setDiffuseColorProperty(Vector3::One);
            effect_->setWorldProperty(Matrix::getIdentityProperty());

            InitializeSphere();
        }

        /**
         * @brief Adds a line for one frame.
         *
         * @param a First endpoint.
         * @param b Second endpoint.
         * @param color Line color.
         */
        static void AddLine(Vector3 a, Vector3 b, Color color)
        {
            AddLine(a, b, color, 0.0f);
        }

        /**
         * @brief Adds a line for a specified lifetime.
         *
         * @param a First endpoint.
         * @param b Second endpoint.
         * @param color Line color.
         * @param life Lifetime in seconds.
         */
        static void AddLine(Vector3 a, Vector3 b, Color color, float life)
        {
            auto shape = GetShapeForLines(1, life);
            shape->Vertices[0] = VertexPositionColor(a, color);
            shape->Vertices[1] = VertexPositionColor(b, color);
        }

        /**
         * @brief Adds a triangle for one frame.
         *
         * @param a First vertex.
         * @param b Second vertex.
         * @param c Third vertex.
         * @param color Triangle color.
         */
        static void AddTriangle(Vector3 a, Vector3 b, Vector3 c, Color color)
        {
            AddTriangle(a, b, c, color, 0.0f);
        }

        /**
         * @brief Adds a triangle for a specified lifetime.
         *
         * @param a First vertex.
         * @param b Second vertex.
         * @param c Third vertex.
         * @param color Triangle color.
         * @param life Lifetime in seconds.
         */
        static void AddTriangle(Vector3 a, Vector3 b, Vector3 c, Color color, float life)
        {
            auto shape = GetShapeForLines(3, life);
            shape->Vertices[0] = VertexPositionColor(a, color);
            shape->Vertices[1] = VertexPositionColor(b, color);
            shape->Vertices[2] = VertexPositionColor(b, color);
            shape->Vertices[3] = VertexPositionColor(c, color);
            shape->Vertices[4] = VertexPositionColor(c, color);
            shape->Vertices[5] = VertexPositionColor(a, color);
        }

        /**
         * @brief Adds a bounding frustum for one frame.
         *
         * @param frustum Frustum to render.
         * @param color Frustum color.
         */
        static void AddBoundingFrustum(const BoundingFrustum& frustum, Color color)
        {
            AddBoundingFrustum(frustum, color, 0.0f);
        }

        /**
         * @brief Adds a bounding frustum for a specified lifetime.
         *
         * @param frustum Frustum to render.
         * @param color Frustum color.
         * @param life Lifetime in seconds.
         */
        static void AddBoundingFrustum(const BoundingFrustum& frustum, Color color, float life)
        {
            auto shape = GetShapeForLines(12, life);
            frustum.GetCorners(corners_);

            shape->Vertices[0]  = VertexPositionColor(corners_[0], color);
            shape->Vertices[1]  = VertexPositionColor(corners_[1], color);
            shape->Vertices[2]  = VertexPositionColor(corners_[1], color);
            shape->Vertices[3]  = VertexPositionColor(corners_[2], color);
            shape->Vertices[4]  = VertexPositionColor(corners_[2], color);
            shape->Vertices[5]  = VertexPositionColor(corners_[3], color);
            shape->Vertices[6]  = VertexPositionColor(corners_[3], color);
            shape->Vertices[7]  = VertexPositionColor(corners_[0], color);
            shape->Vertices[8]  = VertexPositionColor(corners_[4], color);
            shape->Vertices[9]  = VertexPositionColor(corners_[5], color);
            shape->Vertices[10] = VertexPositionColor(corners_[5], color);
            shape->Vertices[11] = VertexPositionColor(corners_[6], color);
            shape->Vertices[12] = VertexPositionColor(corners_[6], color);
            shape->Vertices[13] = VertexPositionColor(corners_[7], color);
            shape->Vertices[14] = VertexPositionColor(corners_[7], color);
            shape->Vertices[15] = VertexPositionColor(corners_[4], color);
            shape->Vertices[16] = VertexPositionColor(corners_[0], color);
            shape->Vertices[17] = VertexPositionColor(corners_[4], color);
            shape->Vertices[18] = VertexPositionColor(corners_[1], color);
            shape->Vertices[19] = VertexPositionColor(corners_[5], color);
            shape->Vertices[20] = VertexPositionColor(corners_[2], color);
            shape->Vertices[21] = VertexPositionColor(corners_[6], color);
            shape->Vertices[22] = VertexPositionColor(corners_[3], color);
            shape->Vertices[23] = VertexPositionColor(corners_[7], color);
        }

        /**
         * @brief Adds a bounding box for one frame.
         *
         * @param box Bounding box to render.
         * @param color Box color.
         */
        static void AddBoundingBox(const BoundingBox& box, Color color)
        {
            AddBoundingBox(box, color, 0.0f);
        }

        /**
         * @brief Adds a bounding box for a specified lifetime.
         *
         * @param box Bounding box to render.
         * @param color Box color.
         * @param life Lifetime in seconds.
         */
        static void AddBoundingBox(const BoundingBox& box, Color color, float life)
        {
            auto shape = GetShapeForLines(12, life);
            box.GetCorners(corners_);

            shape->Vertices[0]  = VertexPositionColor(corners_[0], color);
            shape->Vertices[1]  = VertexPositionColor(corners_[1], color);
            shape->Vertices[2]  = VertexPositionColor(corners_[1], color);
            shape->Vertices[3]  = VertexPositionColor(corners_[2], color);
            shape->Vertices[4]  = VertexPositionColor(corners_[2], color);
            shape->Vertices[5]  = VertexPositionColor(corners_[3], color);
            shape->Vertices[6]  = VertexPositionColor(corners_[3], color);
            shape->Vertices[7]  = VertexPositionColor(corners_[0], color);
            shape->Vertices[8]  = VertexPositionColor(corners_[4], color);
            shape->Vertices[9]  = VertexPositionColor(corners_[5], color);
            shape->Vertices[10] = VertexPositionColor(corners_[5], color);
            shape->Vertices[11] = VertexPositionColor(corners_[6], color);
            shape->Vertices[12] = VertexPositionColor(corners_[6], color);
            shape->Vertices[13] = VertexPositionColor(corners_[7], color);
            shape->Vertices[14] = VertexPositionColor(corners_[7], color);
            shape->Vertices[15] = VertexPositionColor(corners_[4], color);
            shape->Vertices[16] = VertexPositionColor(corners_[0], color);
            shape->Vertices[17] = VertexPositionColor(corners_[4], color);
            shape->Vertices[18] = VertexPositionColor(corners_[1], color);
            shape->Vertices[19] = VertexPositionColor(corners_[5], color);
            shape->Vertices[20] = VertexPositionColor(corners_[2], color);
            shape->Vertices[21] = VertexPositionColor(corners_[6], color);
            shape->Vertices[22] = VertexPositionColor(corners_[3], color);
            shape->Vertices[23] = VertexPositionColor(corners_[7], color);
        }

        /**
         * @brief Adds a bounding sphere for one frame.
         *
         * @param sphere Bounding sphere to render.
         * @param color Sphere color.
         */
        static void AddBoundingSphere(const BoundingSphere& sphere, Color color)
        {
            AddBoundingSphere(sphere, color, 0.0f);
        }

        /**
         * @brief Adds a bounding sphere for a specified lifetime.
         *
         * @param sphere Bounding sphere to render.
         * @param color Sphere color.
         * @param life Lifetime in seconds.
         */
        static void AddBoundingSphere(const BoundingSphere& sphere, Color color, float life)
        {
            auto shape = GetShapeForLines(sphereLineCount, life);
            for (int i = 0; i < static_cast<int>(unitSphere_.size()); i++) {
                Vector3 vertPos = unitSphere_[i] * sphere.Radius + sphere.Center;
                shape->Vertices[i] = VertexPositionColor(vertPos, color);
            }
        }

        /**
         * @brief Draws all active shapes and advances their lifetimes.
         *
         * @param gameTime Current timing snapshot.
         * @param view View matrix.
         * @param projection Projection matrix.
         */
        static void Draw(const GameTime& gameTime, Matrix view, Matrix projection)
        {
            effect_->setViewProperty(view);
            effect_->setProjectionProperty(projection);

            int vertexCount = 0;
            for (auto& shape : activeShapes_)
                vertexCount += shape->LineCount * 2;

            if (vertexCount > 0) {
                if (static_cast<int>(verts_.size()) < vertexCount)
                    verts_.resize(vertexCount * 2);

                int lineCount = 0;
                int vertIndex = 0;
                for (auto& shape : activeShapes_) {
                    lineCount += shape->LineCount;
                    int shapeVerts = shape->LineCount * 2;
                    for (int i = 0; i < shapeVerts; i++)
                        verts_[vertIndex++] = shape->Vertices[i];
                }

                effect_->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

                int vertexOffset = 0;
                while (lineCount > 0) {
                    int linesToDraw = std::min(lineCount, 65535);
                    graphics_->DrawUserPrimitives(PrimitiveType::LineList,
                        verts_.data(), vertexOffset, linesToDraw);
                    vertexOffset += linesToDraw * 2;
                    lineCount    -= linesToDraw;
                }
            }

            bool resort = false;
            for (int i = static_cast<int>(activeShapes_.size()) - 1; i >= 0; i--) {
                activeShapes_[i]->Lifetime -= static_cast<float>(
                    gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
                if (activeShapes_[i]->Lifetime <= 0.0f) {
                    cachedShapes_.push_back(activeShapes_[i]);
                    activeShapes_.erase(activeShapes_.begin() + i);
                    resort = true;
                }
            }

            if (resort) {
                std::sort(cachedShapes_.begin(), cachedShapes_.end(),
                    [](const auto& a, const auto& b) {
                        return a->Vertices.size() < b->Vertices.size();
                    });
            }
        }
    };

} // namespace ShapeRenderingSample
