// SPDX-License-Identifier: MS-PL
#pragma once

// CollisionMesh.hpp — C++ port of BoxCollider/CollisionMesh.cs (XNA 4.0 Ship
// Game Starter Kit). Reads a level model's geometry back out of its vertex and
// index buffers and builds the collision tree over it.

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"

#include "CollisionBox.hpp"
#include "CollisionFace.hpp"
#include "CollisionTree.hpp"
#include "CollisionTreeElem.hpp"

namespace BoxCollider {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::VertexDeclaration;
using Microsoft::Xna::Framework::Graphics::VertexElement;
using Microsoft::Xna::Framework::Graphics::VertexElementFormat;
using Microsoft::Xna::Framework::Graphics::VertexElementUsage;

// The vertex NormalMappingModelProcessor writes; read back to build the collision mesh.
// Port of the CustomVertex struct in BoxCollider/CollisionMesh.cs.
struct CustomVertex {
    Vector3 Position;
    Vector4 Normal;
    Vector4 Binormal;
    Vector3 Tangent;

    CustomVertex() = default;

    CustomVertex(Vector3 position, Vector4 normal, Vector4 binormal, Vector3 tangent)
        : Position(position), Normal(normal), Binormal(binormal), Tangent(tangent) {}

    static const VertexDeclaration& getVertexDeclarationStatic() {
        static const VertexDeclaration value{
            VertexElement(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
            VertexElement(12, VertexElementFormat::Vector4, VertexElementUsage::Normal, 0),
            VertexElement(28, VertexElementFormat::Vector4, VertexElementUsage::Binormal, 0),
            VertexElement(44, VertexElementFormat::Vector3, VertexElementUsage::Tangent, 0)};
        return value;
    }
};

// Port of BoxCollider/CollisionMesh.cs.
class CollisionMesh {
public:
    CollisionMesh(Model& model, std::uint32_t subdivLevel) {
        int verticesCapacity = 0;
        int facesCapacity = 0;
        for (const auto& mesh : model.getMeshesProperty()) {
            for (const auto& part : mesh->getMeshPartsProperty()) {
                verticesCapacity += part->getVertexBufferProperty()->getVertexCountProperty();
                facesCapacity += part->getPrimitiveCountProperty();
            }
        }

        vertices_.resize(verticesCapacity);
        faces_.reserve(facesCapacity);

        int verticesLength = 0;

        std::vector<Matrix> modelTransforms(model.getBonesProperty().getCountProperty());
        model.CopyAbsoluteBoneTransformsTo(modelTransforms);
        for (const auto& mesh : model.getMeshesProperty()) {
            Matrix meshTransform = modelTransforms[mesh->getParentBoneProperty()->getIndexProperty()];

            for (const auto& part : mesh->getMeshPartsProperty()) {
                int vertexCount = part->getVertexBufferProperty()->getVertexCountProperty();
                std::vector<CustomVertex> partVertices(vertexCount);
                part->getVertexBufferProperty()->GetData(partVertices.data(), vertexCount);

                for (int i = 0; i < vertexCount; i++) {
                    vertices_[verticesLength + i] =
                        Vector3::Transform(partVertices[i].Position, meshTransform);
                }

                int indexCount = part->getIndexBufferProperty()->getIndexCountProperty();
                std::vector<std::uint16_t> rawIndices(indexCount);
                part->getIndexBufferProperty()->GetData(rawIndices.data(), indexCount);
                std::vector<short> partIndices(rawIndices.begin(), rawIndices.end());

                for (int i = 0; i < part->getPrimitiveCountProperty(); i++) {
                    faces_.push_back(std::make_unique<CollisionFace>(
                        part->getStartIndexProperty() + i * 3, partIndices,
                        verticesLength + part->getVertexOffsetProperty(), vertices_));
                }

                verticesLength += vertexCount;
            }
        }

        CollisionBox box(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (int i = 0; i < verticesCapacity; i++)
            box.AddPoint(vertices_[i]);

        if (subdivLevel > 6)
            subdivLevel = 6; // max 8^6 nodes
        tree_ = std::make_unique<CollisionTree>(box, subdivLevel);
        for (const auto& face : faces_)
            tree_->AddElement(face.get());
    }

    bool PointIntersect(Vector3 rayStart, Vector3 rayEnd, float& intersectDistance,
                        Vector3& intersectPosition, Vector3& intersectNormal) {
        return tree_->PointIntersect(rayStart, rayEnd, vertices_, intersectDistance,
                                     intersectPosition, intersectNormal);
    }

    bool BoxIntersect(const CollisionBox& box, Vector3 rayStart, Vector3 rayEnd,
                      float& intersectDistance, Vector3& intersectPosition,
                      Vector3& intersectNormal) {
        return tree_->BoxIntersect(box, rayStart, rayEnd, vertices_, intersectDistance,
                                   intersectPosition, intersectNormal);
    }

    void PointMove(Vector3 pointStart, Vector3 pointEnd, float frictionFactor, float bumpFactor,
                   std::uint32_t recurseLevel, Vector3& pointResult) {
        tree_->PointMove(pointStart, pointEnd, vertices_, frictionFactor, bumpFactor, recurseLevel,
                         pointResult);
    }

    bool BoxMove(const CollisionBox& box, Vector3 pointStart, Vector3 pointEnd,
                 float frictionFactor, float bumpFactor, std::uint32_t recurseLevel,
                 Vector3& pointResult) {
        return tree_->BoxMove(box, pointStart, pointEnd, vertices_, frictionFactor, bumpFactor,
                              recurseLevel, pointResult);
    }

    void GetElements(const CollisionBox& b, std::vector<CollisionTreeElem*>& e) {
        tree_->GetElements(b, e);
    }

    void AddElement(CollisionTreeElem* e) { tree_->AddElement(e); }

    void RemoveElement(CollisionTreeElemDynamic* e) { tree_->RemoveElement(e); }

private:
    // mesh vertices
    std::vector<Vector3> vertices_;
    // mesh faces
    std::vector<std::unique_ptr<CollisionFace>> faces_;
    // tree with meshes faces
    std::unique_ptr<CollisionTree> tree_;
};

} // namespace BoxCollider
