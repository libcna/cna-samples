// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionTree.hpp — C++ port of BoxCollider/CollisionTree.cs (XNA 4.0 Ship
// Game Starter Kit). The octree over a mesh's faces, and the swept point/box
// movement that slides and bounces along whatever it hits.

#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "CollisionBox.hpp"
#include "CollisionTreeElem.hpp"
#include "CollisionTreeNode.hpp"

namespace BoxCollider {

using Microsoft::Xna::Framework::Vector3;

// Port of BoxCollider/CollisionTree.cs.
class CollisionTree {
public:
    CollisionTree(const CollisionBox& box, std::uint32_t subdivLevel)
        : root_(std::make_unique<CollisionTreeNode>(box, subdivLevel)), recurseId_(0) {}

    void AddElement(CollisionTreeElem* elem) { root_->AddElement(elem); }

    void RemoveElement(CollisionTreeElemDynamic* dynamicElem) {
        if (dynamicElem != nullptr) {
            dynamicElem->RemoveFromNodes();
        }
    }

    void GetElements(const CollisionBox& collisionBox, std::vector<CollisionTreeElem*>& elements) {
        root_->GetElements(collisionBox, elements, ++recurseId_);
    }

    bool PointMove(Vector3 pointStart, Vector3 pointEnd, const std::vector<Vector3>& vertices,
                   float frictionFactor, float bumpFactor, std::uint32_t recurseLevel,
                   Vector3& pointResult) {
        pointResult = pointStart;

        Vector3 delta = pointEnd - pointStart;
        float delta_len = delta.Length();
        if (delta_len < 0.00001f)
            return false;

        float total_dist = delta_len;
        delta *= 1.0f / delta_len;

        float bias = 0.01f;

        pointEnd += delta * bias;

        bool collision_hit = false;

        while (recurseLevel > 0) {
            float dist;
            Vector3 pos, norm;
            if (false == PointIntersect(pointStart, pointEnd, vertices, dist, pos, norm)) {
                pointStart = pointEnd - delta * bias;
                break;
            }

            collision_hit = true;

            dist -= bias / std::abs(Vector3::Dot(delta, norm));
            if (dist > 0) {
                pointStart += delta * dist;
                total_dist -= dist;
            }

            Vector3 reflect_dir = Vector3::Normalize(Vector3::Reflect(delta, norm));

            Vector3 n = norm * Vector3::Dot(reflect_dir, norm);
            Vector3 t = reflect_dir - n;

            reflect_dir = frictionFactor * t + bumpFactor * n;

            pointEnd = pointStart + reflect_dir * total_dist;

            delta = pointEnd - pointStart;
            delta_len = delta.Length();
            if (delta_len < 0.00001f)
                break;
            delta *= 1.0f / delta_len;

            pointEnd += delta * bias;

            recurseLevel--;
        }

        pointResult = pointStart;
        return collision_hit;
    }

    bool BoxMove(const CollisionBox& box, Vector3 pointStart, Vector3 pointEnd,
                 const std::vector<Vector3>& vertices, float frictionFactor, float bumpFactor,
                 std::uint32_t recurseLevel, Vector3& pointResult) {
        pointResult = pointStart;

        Vector3 delta = pointEnd - pointStart;
        float deltaLength = delta.Length();
        if (deltaLength < 0.00001f)
            return false;

        float totalDistance = deltaLength;
        delta *= 1.0f / deltaLength;

        float bias = 0.01f;

        pointEnd += delta * bias;

        bool collisionHit = false;

        while (recurseLevel > 0) {
            float dist;
            Vector3 pos, norm;
            if (false == BoxIntersect(box, pointStart, pointEnd, vertices, dist, pos, norm)) {
                pointStart = pointEnd - delta * bias;
                break;
            }

            collisionHit = true;

            dist -= bias / std::abs(Vector3::Dot(delta, norm));
            if (dist > 0) {
                pointStart += delta * dist;
                totalDistance -= dist;
            }

            Vector3 reflectDirection = Vector3::Normalize(Vector3::Reflect(delta, norm));

            Vector3 n = norm * Vector3::Dot(reflectDirection, norm);
            Vector3 t = reflectDirection - n;

            reflectDirection = frictionFactor * t + bumpFactor * n;

            pointEnd = pointStart + reflectDirection * totalDistance;

            delta = pointEnd - pointStart;
            deltaLength = delta.Length();
            if (deltaLength < 0.00001f)
                break;
            delta *= 1.0f / deltaLength;

            pointEnd += delta * bias;

            recurseLevel--;
        }

        pointResult = pointStart;
        return collisionHit;
    }

    bool PointIntersect(Vector3 rayStart, Vector3 rayEnd, const std::vector<Vector3>& vertices,
                        float& intersectDistance, Vector3& intersectPosition,
                        Vector3& intersectNormal) {
        intersectDistance = 0.0f;
        intersectPosition = rayStart;
        intersectNormal = Vector3::Zero;

        Vector3 rayDirection = rayEnd - rayStart;
        float rayLength = rayDirection.Length();
        if (rayLength == 0)
            return false;

        CollisionBox rayBox(std::numeric_limits<float>::max(),
                            -std::numeric_limits<float>::max());
        rayBox.AddPoint(rayStart);
        rayBox.AddPoint(rayEnd);
        Vector3 inflate(0.001f, 0.001f, 0.001f);
        rayBox.min -= inflate;
        rayBox.max += inflate;

        std::vector<CollisionTreeElem*> elems;
        root_->GetElements(rayBox, elems, ++recurseId_);

        rayDirection *= 1.0f / rayLength;
        intersectDistance = rayLength;

        bool intersected = false;

        for (CollisionTreeElem* e : elems) {
            float distance;
            Vector3 position;
            Vector3 normal;
            if (true == e->PointIntersect(rayStart, rayDirection, vertices, distance, position,
                                          normal)) {
                if (distance < intersectDistance) {
                    intersectDistance = distance;
                    intersectPosition = position;
                    intersectNormal = normal;
                    intersected = true;
                }
            }
        }

        return intersected;
    }

    bool BoxIntersect(const CollisionBox& box, Vector3 rayStart, Vector3 rayEnd,
                      const std::vector<Vector3>& vertices, float& intersectDistance,
                      Vector3& intersectPosition, Vector3& intersectNormal) {
        intersectDistance = 0.0f;
        intersectPosition = rayStart;
        intersectNormal = Vector3::Zero;

        Vector3 rayDirection = rayEnd - rayStart;
        float rayLength = rayDirection.Length();
        if (rayLength == 0)
            return false;

        CollisionBox rayBox(box.min + rayStart, box.max + rayStart);
        rayBox.AddPoint(rayBox.min + rayDirection);
        rayBox.AddPoint(rayBox.max + rayDirection);
        Vector3 inflate(0.001f, 0.001f, 0.001f);
        rayBox.min -= inflate;
        rayBox.max += inflate;

        std::vector<CollisionTreeElem*> elems;
        root_->GetElements(rayBox, elems, ++recurseId_);

        rayDirection *= 1.0f / rayLength;
        intersectDistance = rayLength;

        bool intersected = false;

        for (CollisionTreeElem* e : elems) {
            float distance;
            Vector3 position;
            Vector3 normal;
            if (true == e->BoxIntersect(box, rayStart, rayDirection, vertices, distance, position,
                                        normal)) {
                if (distance < intersectDistance) {
                    intersectDistance = distance;
                    intersectPosition = position;
                    intersectNormal = normal;
                    intersected = true;
                }
            }
        }

        return intersected;
    }

private:
    // the tree root node
    std::unique_ptr<CollisionTreeNode> root_;

    // the last recurse id used (for selections without duplicates)
    std::uint32_t recurseId_;
};

} // namespace BoxCollider
