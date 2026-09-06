// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionBox.hpp — C++ port of BoxCollider/CollisionBox.cs (XNA 4.0 Ship Game
// Starter Kit). An axis-aligned bounding box with the ray/box/point tests the
// collision tree is built on, and a wireframe debug draw.

#include <array>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/IDisposable.hpp"

namespace BoxCollider {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::VertexDeclaration;
using Microsoft::Xna::Framework::Graphics::VertexPositionColor;

// Port of BoxCollider/CollisionBox.cs.
class CollisionBox : public System::IDisposable {
public:
    // the bounding box minimum point
    Vector3 min;
    // the bounding box maximum point
    Vector3 max;

    static constexpr float COS45 = 0.70710678f;
    static constexpr float INVSQRT3 = 0.57735027f;

    // normals for each vertex
    static const std::array<Vector3, 8>& vertexNormals() {
        static const std::array<Vector3, 8> value = {
            Vector3(-INVSQRT3, -INVSQRT3, -INVSQRT3),
            Vector3(INVSQRT3, INVSQRT3, INVSQRT3),
            Vector3(INVSQRT3, -INVSQRT3, -INVSQRT3),
            Vector3(-INVSQRT3, INVSQRT3, INVSQRT3),
            Vector3(INVSQRT3, INVSQRT3, -INVSQRT3),
            Vector3(-INVSQRT3, -INVSQRT3, INVSQRT3),
            Vector3(-INVSQRT3, INVSQRT3, -INVSQRT3),
            Vector3(INVSQRT3, -INVSQRT3, INVSQRT3),
        };
        return value;
    }

    // normals for each edge
    static const std::array<Vector3, 12>& edgeNormals() {
        static const std::array<Vector3, 12> value = {
            Vector3(-COS45, 0, -COS45), Vector3(0, COS45, -COS45),
            Vector3(COS45, 0, -COS45),  Vector3(0, -COS45, -COS45),
            Vector3(0, COS45, COS45),   Vector3(-COS45, 0, COS45),
            Vector3(0, -COS45, COS45),  Vector3(COS45, 0, COS45),
            Vector3(-COS45, -COS45, 0), Vector3(-COS45, COS45, 0),
            Vector3(COS45, COS45, 0),   Vector3(COS45, -COS45, 0),
        };
        return value;
    }

    // normals for each face
    static const std::array<Vector3, 6>& faceNormals() {
        static const std::array<Vector3, 6> value = {
            Vector3(1, 0, 0), Vector3(0, 1, 0),  Vector3(0, 0, 1),
            Vector3(-1, 0, 0), Vector3(0, -1, 0), Vector3(0, 0, -1),
        };
        return value;
    }

    // constructor from min/max floats
    CollisionBox(float minValue, float maxValue)
        : min(minValue, minValue, minValue), max(maxValue, maxValue, maxValue) {}

    // constructor from min/max vectors
    CollisionBox(Vector3 minPoint, Vector3 maxPoint) : min(minPoint), max(maxPoint) {}

    // constructor from another collision box
    CollisionBox(const CollisionBox& bb) : System::IDisposable(), min(bb.min), max(bb.max) {}

    CollisionBox& operator=(const CollisionBox& bb) {
        min = bb.min;
        max = bb.max;
        return *this;
    }

    ~CollisionBox() override { Dispose(true); }

    // add a point to the bounding box extending it if needed
    void AddPoint(Vector3 p) {
        if (p.X >= max.X)
            max.X = p.X;
        if (p.Y >= max.Y)
            max.Y = p.Y;
        if (p.Z >= max.Z)
            max.Z = p.Z;

        if (p.X <= min.X)
            min.X = p.X;
        if (p.Y <= min.Y)
            min.Y = p.Y;
        if (p.Z <= min.Z)
            min.Z = p.Z;
    }

    // get the bounding box center point
    Vector3 Center() const { return 0.5f * (min + max); }

    // check if two bounding boxes have any intersection
    bool BoxIntersect(const CollisionBox& bb) const {
        if (max.X >= bb.min.X && min.X <= bb.max.X && max.Y >= bb.min.Y && min.Y <= bb.max.Y &&
            max.Z >= bb.min.Z && min.Z <= bb.max.Z)
            return true;
        return false;
    }

    // check if a point in inside the bounding box
    bool PointInside(Vector3 p) const {
        return p.X > min.X && p.X <= max.X && p.Y > min.Y && p.Y <= max.Y && p.Z > min.Z &&
               p.Z <= max.Z;
    }

    // split in middle point creating 8 children
    std::vector<CollisionBox> GetChildren() const {
        Vector3 center = 0.5f * (min + max);

        std::vector<CollisionBox> children;
        children.reserve(8);

        children.emplace_back(min, center);
        children.emplace_back(Vector3(center.X, min.Y, min.Z), Vector3(max.X, center.Y, center.Z));
        children.emplace_back(Vector3(min.X, center.Y, min.Z), Vector3(center.X, max.Y, center.Z));
        children.emplace_back(Vector3(center.X, center.Y, min.Z), Vector3(max.X, max.Y, center.Z));
        children.emplace_back(Vector3(min.X, min.Y, center.Z), Vector3(center.X, center.Y, max.Z));
        children.emplace_back(Vector3(center.X, min.Y, center.Z), Vector3(max.X, center.Y, max.Z));
        children.emplace_back(Vector3(min.X, center.Y, center.Z), Vector3(center.X, max.Y, max.Z));
        children.emplace_back(center, max);

        return children;
    }

    // get the 8 bounding box vertices
    std::vector<Vector3> GetVertices() const {
        std::vector<Vector3> vertices(8);

        vertices[0] = min;
        vertices[1] = max;
        vertices[2] = Vector3(max.X, min.Y, min.Z);
        vertices[3] = Vector3(min.X, max.Y, max.Z);
        vertices[4] = Vector3(max.X, max.Y, min.Z);
        vertices[5] = Vector3(min.X, min.Y, max.Z);
        vertices[6] = Vector3(min.X, max.Y, min.Z);
        vertices[7] = Vector3(max.X, min.Y, max.Z);

        return vertices;
    }

    // get the 12 edges (each edge in list made of two 3D points)
    std::vector<Vector3> GetEdges() const {
        std::vector<Vector3> vertices = GetVertices();

        std::vector<Vector3> edges(24);

        edges[0] = vertices[0];  edges[1] = vertices[6];
        edges[2] = vertices[6];  edges[3] = vertices[4];
        edges[4] = vertices[4];  edges[5] = vertices[2];
        edges[6] = vertices[2];  edges[7] = vertices[0];
        edges[8] = vertices[1];  edges[9] = vertices[3];
        edges[10] = vertices[3]; edges[11] = vertices[5];
        edges[12] = vertices[5]; edges[13] = vertices[7];
        edges[14] = vertices[7]; edges[15] = vertices[1];
        edges[16] = vertices[0]; edges[17] = vertices[5];
        edges[18] = vertices[3]; edges[19] = vertices[6];
        edges[20] = vertices[4]; edges[21] = vertices[1];
        edges[22] = vertices[7]; edges[23] = vertices[2];

        return edges;
    }

    // collide ray defined by ray origin (ro) and ray direction (rd) with the box.
    // returns -1 on no collision and the face index (0 to 5) if collision is found
    // together with the distances to the collision points
    int RayIntersect(Vector3 rayOrigin, Vector3 rayDirection, float& tnear, float& tfar) const {
        float t1, t2, t;

        tnear = -std::numeric_limits<float>::max();
        tfar = std::numeric_limits<float>::max();

        int face, i = -1, j = -1;

        // intersect in X
        //
        // The parallel-ray guard reads `> -0.00001f && < -0.00001f` in the original and can
        // therefore never be true; the else branch always runs. Reproduced, not repaired.
        if (rayDirection.X > -0.00001f && rayDirection.X < -0.00001f) {
            if (rayOrigin.X < min.X || rayOrigin.X > max.X)
                return -1;
        } else {
            t = 1.0f / rayDirection.X;
            t1 = (min.X - rayOrigin.X) * t;
            t2 = (max.X - rayOrigin.X) * t;

            if (t1 > t2) {
                t = t1; t1 = t2; t2 = t;
                face = 0;
            } else
                face = 3;

            if (t1 > tnear) {
                tnear = t1;
                i = face;
            }
            if (t2 < tfar) {
                tfar = t2;
                if (face > 2)
                    j = face - 3;
                else
                    j = face + 3;
            }

            if (tnear > tfar || tfar < 0.00001f)
                return -1;
        }

        // intersect in Y
        if (rayDirection.Y > -0.00001f && rayDirection.Y < -0.00001f) {
            if (rayOrigin.Y < min.Y || rayOrigin.Y > max.Y)
                return -1;
        } else {
            t = 1.0f / rayDirection.Y;
            t1 = (min.Y - rayOrigin.Y) * t;
            t2 = (max.Y - rayOrigin.Y) * t;

            if (t1 > t2) {
                t = t1; t1 = t2; t2 = t;
                face = 1;
            } else
                face = 4;

            if (t1 > tnear) {
                tnear = t1;
                i = face;
            }
            if (t2 < tfar) {
                tfar = t2;
                if (face > 2)
                    j = face - 3;
                else
                    j = face + 3;
            }

            if (tnear > tfar || tfar < 0.00001f)
                return -1;
        }

        // intersect in Z
        if (rayDirection.Z > -0.00001f && rayDirection.Z < -0.00001f) {
            if (rayOrigin.Z < min.Z || rayOrigin.Z > max.Z)
                return -1;
        } else {
            t = 1.0f / rayDirection.Z;
            t1 = (min.Z - rayOrigin.Z) * t;
            t2 = (max.Z - rayOrigin.Z) * t;

            if (t1 > t2) {
                t = t1; t1 = t2; t2 = t;
                face = 2;
            } else
                face = 5;

            if (t1 > tnear) {
                tnear = t1;
                i = face;
            }
            if (t2 < tfar) {
                tfar = t2;
                if (face > 2)
                    j = face - 3;
                else
                    j = face + 3;
            }
        }

        if (tnear > tfar || tfar < 0.00001f)
            return -1;

        if (tnear < 0.0f)
            return j;
        else
            return i;
    }

    // render the bounding box as wireframe
    void Draw(GraphicsDevice* gd) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }

        std::vector<Vector3> edges = GetEdges();
        if (!vertexBuffer_.has_value()) {
            vertexBuffer_.emplace(*gd, VertexPositionColor::getVertexDeclarationStatic(), 24,
                                  BufferUsage::WriteOnly);
        }
        if (!vertexDecl_.has_value()) {
            vertexDecl_.emplace(VertexPositionColor::getVertexDeclarationStatic().GetVertexElements());
        }

        std::vector<VertexPositionColor> verts(24);
        for (int i = 0; i < 24; i += 2) {
            verts[i].Position = edges[i];
            verts[i].Color = Color::Red;
            verts[i + 1].Position = edges[i + 1];
            verts[i + 1].Color = Color::Red;
        }
        vertexBuffer_->SetData(verts.data(), 24);

        gd->getRasterizerStateProperty().setDepthBiasProperty(-0.1f);

        gd->SetVertexBuffer(&vertexBuffer_.value());
        gd->DrawPrimitives(PrimitiveType::LineList, 0, 12);

        gd->getRasterizerStateProperty().setDepthBiasProperty(0.0f);
    }

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override { Dispose(true); }

private:
    void Dispose(bool disposing) {
        if (disposing && !isDisposed_) {
            vertexDecl_.reset();
            vertexBuffer_.reset();
        }
    }

    bool isDisposed_ = false;

    // vertex buffer and declaration for drawing debug box
    std::optional<VertexBuffer> vertexBuffer_;
    std::optional<VertexDeclaration> vertexDecl_;
};

} // namespace BoxCollider
