// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionFace.hpp — C++ port of BoxCollider/CollisionFace.cs (XNA 4.0 Ship
// Game Starter Kit). One triangle of the collision mesh, with the ray/triangle
// and swept-box tests the whole collider rests on.

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/IDisposable.hpp"

#include "CollisionBox.hpp"
#include "CollisionTreeElem.hpp"

namespace BoxCollider {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Port of BoxCollider/CollisionFace.cs.
class CollisionFace : public CollisionTreeElem, public System::IDisposable {
public:
    // face constructor
    CollisionFace(int offset, const std::vector<int>& indexBuffer, int vertexOffset,
                  const std::vector<Vector3>& vertexBuffer) {
        box.emplace(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (int i = 0; i < 3; i++) {
            indices_[i] = indexBuffer[i + offset] + vertexOffset;
            box->AddPoint(vertexBuffer[indices_[i]]);
        }
    }

    CollisionFace(int offset, const std::vector<short>& indexBuffer, int vertexOffset,
                  const std::vector<Vector3>& vertexBuffer) {
        box.emplace(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (int i = 0; i < 3; i++) {
            indices_[i] = (int)indexBuffer[i + offset] + vertexOffset;
            box->AddPoint(vertexBuffer[indices_[i]]);
        }
    }

    ~CollisionFace() override { Dispose(true); }

    // remove vector component (vector3 to vector2)
    static Vector2 Vector3RemoveComponent(Vector3 vec, std::uint32_t i) {
        switch (i) {
            case 0: return Vector2(vec.Y, vec.Z);
            case 1: return Vector2(vec.X, vec.Z);
            case 2: return Vector2(vec.X, vec.Y);
            default: return Vector2::Zero;
        }
    }

    // intersect edge (p1,p2) moving in direction (dir) colliding with edge (p3,p4)
    // return true on a collision with collision distance (dist)
    // and intersection point (ip)
    static bool EdgeIntersect(Vector3 p1, Vector3 p2, Vector3 dir, Vector3 p3, Vector3 p4,
                              float& dist, Vector3& ip) {
        dist = 0;
        ip = Vector3::Zero;

        // edge vectors
        Vector3 v1 = p2 - p1;
        Vector3 v2 = p4 - p3;

        // build plane based on edge (p1,p2) and move direction (dir)
        Vector3 planeDir;
        float planeW;
        planeDir = Vector3::Cross(v1, dir);
        planeDir.Normalize();
        planeW = Vector3::Dot(planeDir, p1);

        // if colliding edge (p3,p4) does not cross plane return no collision
        // same as if p3 and p4 on same side of plane return 0
        float temp = (Vector3::Dot(planeDir, p3) - planeW) * (Vector3::Dot(planeDir, p4) - planeW);
        if (temp > 0)
            return false;

        // if colliding edge (p3,p4) and plane are paralell return no collision
        v2.Normalize();
        temp = Vector3::Dot(planeDir, v2);
        if (temp == 0)
            return false;

        // compute intersection point of plane and colliding edge (p3,p4)
        ip = p3 + v2 * ((planeW - Vector3::Dot(planeDir, p3)) / temp);

        // get largest 2D plane projection
        planeDir.X = std::abs(planeDir.X);
        planeDir.Y = std::abs(planeDir.Y);
        planeDir.Z = std::abs(planeDir.Z);
        std::uint32_t i;
        if (planeDir.X > planeDir.Y) {
            i = 0;
            if (planeDir.X < planeDir.Z)
                i = 2;
        } else {
            i = 1;
            if (planeDir.Y < planeDir.Z)
                i = 2;
        }

        // remove component with largest absolute value
        Vector2 p12d = CollisionFace::Vector3RemoveComponent(p1, i);
        Vector2 v12d = CollisionFace::Vector3RemoveComponent(v1, i);
        Vector2 ip2d = CollisionFace::Vector3RemoveComponent(ip, i);
        Vector2 dir2d = CollisionFace::Vector3RemoveComponent(dir, i);

        // compute distance of intersection from line (ip,-dir) to line (p1,p2)
        dist = (v12d.X * (ip2d.Y - p12d.Y) - v12d.Y * (ip2d.X - p12d.X)) /
               (v12d.X * dir2d.Y - v12d.Y * dir2d.X);
        if (dist < 0)
            return false;

        // compute intesection point on edge (p1,p2)
        ip -= dist * dir;

        // check if intersection point (ip) is between egde (p1,p2) vertices
        temp = Vector3::Dot(p1 - ip, p2 - ip);
        if (temp >= 0)
            return false; // no collision

        return true; // collision found!
    }

    // triangle intersect from http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
    static bool RayTriangleIntersect(Vector3 rayOrigin, Vector3 rayDirection, Vector3 vert0,
                                     Vector3 vert1, Vector3 vert2, float& t, float& u, float& v) {
        t = 0; u = 0; v = 0;

        Vector3 edge1 = vert1 - vert0;
        Vector3 edge2 = vert2 - vert0;

        Vector3 tvec, pvec, qvec;
        float det, inv_det;

        pvec = Vector3::Cross(rayDirection, edge2);

        det = Vector3::Dot(edge1, pvec);

        if (det > -0.00001f)
            return false;

        inv_det = 1.0f / det;

        tvec = rayOrigin - vert0;

        u = Vector3::Dot(tvec, pvec) * inv_det;
        if (u < -0.0001f || u > 1.0001f)
            return false;

        qvec = Vector3::Cross(tvec, edge1);

        v = Vector3::Dot(rayDirection, qvec) * inv_det;
        if (v < -0.0001f || u + v > 1.0001f)
            return false;

        t = Vector3::Dot(edge2, qvec) * inv_det;

        if (t <= 0)
            return false;

        return true;
    }

    // ray intersect face and return intersection distance, point and normal
    bool PointIntersect(Vector3 rayOrigin, Vector3 rayDirection,
                        const std::vector<Vector3>& vertices, float& intersectDistance,
                        Vector3& intersectPosition, Vector3& intersectNormal) override {
        intersectDistance = 0.0f;
        intersectPosition = rayOrigin;
        intersectNormal = Vector3::Zero;

        Vector3 v1 = vertices[indices_[0]];
        Vector3 v2 = vertices[indices_[1]];
        Vector3 v3 = vertices[indices_[2]];

        Vector3 uvt;
        if (CollisionFace::RayTriangleIntersect(rayOrigin, rayDirection, v1, v2, v3, uvt.Z, uvt.X,
                                                uvt.Y)) {
            intersectDistance = uvt.Z;
            intersectPosition = (1.0f - uvt.X - uvt.Y) * v1 + uvt.X * v2 + uvt.Y * v3;
            intersectNormal = Vector3::Normalize(Vector3::Cross(v3 - v1, v2 - v1));
            return true;
        }
        return false;
    }

    // box intersect face and return intersection distance, point and normal
    bool BoxIntersect(const CollisionBox& rayBox, Vector3 rayOrigin, Vector3 rayDirection,
                      const std::vector<Vector3>& vertices, float& intersectDistance,
                      Vector3& intersectPosition, Vector3& intersectNormal) override {
        intersectDistance = std::numeric_limits<float>::max();
        intersectPosition = rayOrigin;
        intersectNormal = Vector3::Zero;

        bool intersected = false;
        Vector3 p1, p2, p3, p4;
        std::uint32_t i, j;

        CollisionBox worldBox(rayBox.min + rayOrigin, rayBox.max + rayOrigin);

        std::vector<Vector3> boxVerts = worldBox.GetVertices();
        std::vector<Vector3> boxEdges = worldBox.GetEdges();

        float distance;
        Vector3 position;

        // intersect box edges to face edges
        for (i = 0; i < 12; i++) {
            // cull edges with normal more than 135 degree from moving direction
            float dot = Vector3::Dot(CollisionBox::edgeNormals()[i], rayDirection);
            if (dot < -0.70710678f)
                continue;

            p1 = boxEdges[i * 2];
            p2 = boxEdges[i * 2 + 1];
            p4 = vertices[indices_[0]];
            for (j = 0; j < indices_.size(); j++) {
                p3 = p4;
                p4 = vertices[indices_[(j + 1) % indices_.size()]];

                if (CollisionFace::EdgeIntersect(p1, p2, rayDirection, p3, p4, distance,
                                                 position)) {
                    if (distance < intersectDistance) {
                        intersectDistance = distance;
                        intersectPosition = position;
                        intersectNormal = Vector3::Cross(p2 - p1, p3 - p4);
                        intersectNormal = Vector3::Normalize(intersectNormal);
                        if (Vector3::Dot(rayDirection, intersectNormal) > 0)
                            intersectNormal = Vector3::Negate(intersectNormal);
                        intersected = true;
                    }
                }
            }
        }

        // intersect from face vertices to box
        for (i = 0; i < 3; i++) {
            float tnear, tfar;
            p1 = vertices[indices_[i]];
            int box_face_id = worldBox.RayIntersect(p1, -rayDirection, tnear, tfar);
            if (box_face_id > -1) {
                if (tnear < intersectDistance) {
                    intersectDistance = tnear;
                    intersectPosition = p1;
                    intersectNormal = -CollisionBox::faceNormals()[box_face_id];
                    intersected = true;
                }
            }
        }

        // intersect from box vertices to face polygon
        Vector3 v1 = vertices[indices_[0]];
        Vector3 v2 = vertices[indices_[1]];
        Vector3 v3 = vertices[indices_[2]];
        Vector3 uvt;
        for (i = 0; i < 8; i++) {
            // cull vertices with normal more than 135 degree from moving direction
            float dot = Vector3::Dot(CollisionBox::vertexNormals()[i], rayDirection);
            if (dot < -0.70710678f)
                continue;

            if (CollisionFace::RayTriangleIntersect(boxVerts[i], rayDirection, v1, v2, v3, uvt.Z,
                                                    uvt.X, uvt.Y)) {
                if (uvt.Z < intersectDistance) {
                    intersectDistance = uvt.Z;
                    intersectPosition = (1.0f - uvt.X - uvt.Y) * v1 + uvt.X * v2 + uvt.Y * v3;
                    intersectNormal = Vector3::Cross(v3 - v1, v2 - v1);
                    intersectNormal = Vector3::Normalize(intersectNormal);
                    intersected = true;
                }
            }
        }

        return intersected;
    }

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override { Dispose(true); }

private:
    void Dispose(bool disposing) {
        if (disposing && !isDisposed_) {
            box.reset();
        }
    }

    bool isDisposed_ = false;

    // indices for the three face vertices
    std::array<int, 3> indices_{};
};

} // namespace BoxCollider
