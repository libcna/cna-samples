// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionTreeElem.hpp — C++ port of BoxCollider/CollisionTreeElem.cs (XNA 4.0
// Ship Game Starter Kit). The base element a collision tree node holds, plus the
// dynamic variant that remembers which nodes it was added to.

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "CollisionBox.hpp"

namespace BoxCollider {

using Microsoft::Xna::Framework::Vector3;

class CollisionTreeNode;

// Port of BoxCollider/CollisionTreeElem.cs.
class CollisionTreeElem {
public:
    // bounding box for tree element
    std::optional<CollisionBox> box;

    // recurse id used to compute selections without element duplicates
    std::uint32_t lastRecurseId = 0;

    // Create a new tree element
    CollisionTreeElem() = default;
    virtual ~CollisionTreeElem() = default;

    // Virtual function to intersect a point with the element
    virtual bool PointIntersect(Vector3 rayOrigin, Vector3 rayDirection,
                                const std::vector<Vector3>& vertices, float& intersectDistance,
                                Vector3& intersectPosition, Vector3& intersectNormal) {
        (void)rayOrigin;
        (void)rayDirection;
        (void)vertices;
        intersectDistance = 0;
        intersectPosition = Vector3::Zero;
        intersectNormal = Vector3::Zero;
        return false;
    }

    // Virtual function to intersect a box with the element
    virtual bool BoxIntersect(const CollisionBox& rayBox, Vector3 rayOrigin, Vector3 rayDirection,
                              const std::vector<Vector3>& vertices, float& intersectDistance,
                              Vector3& intersectPosition, Vector3& intersectNormal) {
        (void)rayBox;
        (void)rayOrigin;
        (void)rayDirection;
        (void)vertices;
        intersectDistance = 0;
        intersectPosition = Vector3::Zero;
        intersectNormal = Vector3::Zero;
        return false;
    }

    // Virtual function when adding the element to a node
    virtual void AddToNode(CollisionTreeNode* n) { (void)n; }
};

// Port of BoxCollider/CollisionTreeElem.cs's CollisionTreeElemDynamic.
class CollisionTreeElemDynamic : public CollisionTreeElem {
public:
    // Create a new dynamic tree element
    // (can change position moving around the tree at any time)
    CollisionTreeElemDynamic() = default;

    // Add the dynamic element to the node
    void AddToNode(CollisionTreeNode* n) override { nodes_.push_back(n); }

    // Remove dynamic element from node — defined out-of-line, once CollisionTreeNode is complete.
    void RemoveFromNodes();

private:
    // all tree nodes the dynamic element is included
    std::vector<CollisionTreeNode*> nodes_;
};

} // namespace BoxCollider
