// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionTreeNode.hpp — C++ port of BoxCollider/CollisionTreeNode.cs (XNA 4.0
// Ship Game Starter Kit). One octree node: eight children until the subdivision
// level runs out, then a leaf holding the elements that intersect it.

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "System/ArgumentNullException.hpp"

#include "CollisionBox.hpp"
#include "CollisionTreeElem.hpp"

namespace BoxCollider {

// Port of BoxCollider/CollisionTreeNode.cs.
class CollisionTreeNode {
public:
    // Create a new tree node
    CollisionTreeNode(const CollisionBox& collisionBox, std::uint32_t subdivLevel)
        // save node box
        : box_(collisionBox) {
        // if subdivision needed
        if (subdivLevel > 0) {
            // decrease subdivision level
            subdivLevel--;

            // create the 8 children
            children_.reserve(8);
            std::vector<CollisionBox> childrenBox = box_.GetChildren();
            for (std::uint32_t i = 0; i < 8; i++)
                children_.push_back(std::make_unique<CollisionTreeNode>(childrenBox[i], subdivLevel));
        }
    }

    // Recursive function to add an element to the tree
    void AddElement(CollisionTreeElem* e) {
        // if element do not intersect node, return
        if (e->box->BoxIntersect(box_) == false)
            return;

        // if leaf  node (no children)
        if (children_.empty()) {
            // add element to list
            elems_.push_back(e);
            e->AddToNode(this);
        } else {
            // if not a leaf recurse to all its children
            for (const auto& n : children_)
                n->AddElement(e);
        }
    }

    // Remove element from this node
    void RemoveElement(CollisionTreeElem* e) {
        const auto it = std::find(elems_.begin(), elems_.end(), e);
        if (it != elems_.end())
            elems_.erase(it);
    }

    // Recursive function to get all elements intersecting a given bounding box
    void GetElements(const CollisionBox& b, std::vector<CollisionTreeElem*>& e,
                     std::uint32_t recurseId) {
        // if selection box does not intersect node box, return
        if (b.BoxIntersect(box_) == false)
            return;

        // if any elements in this node add them to selection list
        for (CollisionTreeElem* elem : elems_) {
            // elements can be repeated in many nodes
            // only add element to selection list if not already
            // added by another node in this same recursion
            if (elem->lastRecurseId < recurseId) {
                // if selection box intersect the element box
                if (elem->box->BoxIntersect(b))
                    // add element to selection list
                    e.push_back(elem);
                // set this recuse id to prevent duplicate results
                elem->lastRecurseId = recurseId;
            }
        }

        // if not a leaf node, recurso to all children
        for (const auto& n : children_)
            n->GetElements(b, e, recurseId);
    }

private:
    // the bounding box for the node
    CollisionBox box_;

    // the node children (empty if node is a leaf)
    std::vector<std::unique_ptr<CollisionTreeNode>> children_;

    // list with elements included in the node (only filled on leaf nodes)
    std::vector<CollisionTreeElem*> elems_;
};

// Remove dynamic element from node
inline void CollisionTreeElemDynamic::RemoveFromNodes() {
    // remove element from all nodes it is included in
    for (CollisionTreeNode* n : nodes_) {
        n->RemoveElement(this);
    }
    nodes_.clear();
}

} // namespace BoxCollider
