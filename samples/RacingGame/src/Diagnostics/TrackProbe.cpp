// SPDX-License-Identifier: MS-PL

#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Landscapes/Landscape.hpp"
#include "Tracks/GuardRailGeometry.hpp"
#include "Tracks/TrackCombiModels.hpp"
#include "Tracks/TrackColumnsGeometry.hpp"
#include "Tracks/TrackData.hpp"
#include "Tracks/TrackGeometry.hpp"
#include "Tracks/TrackLine.hpp"
#include "Tracks/Track.hpp"

namespace
{
    using SharpRuntime::String;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using RacingGame::Landscapes::Landscape;
    using RacingGame::Tracks::GuardRailGeometry;
    using RacingGame::Tracks::TrackData;
    using RacingGame::Tracks::TrackCombiModels;
    using RacingGame::Tracks::TrackColumnsGeometry;
    using RacingGame::Tracks::TrackLine;
    using RacingGame::Tracks::Track;
    using RacingGame::Tracks::TrackGeometry;
    using RacingGame::Tracks::TrackVertex;

    std::uint64_t HashByte(std::uint64_t hash, std::uint8_t value)
    {
        return (hash ^ value) * UINT64_C(1099511628211);
    }

    std::uint64_t HashInt32(std::uint64_t hash, std::uint32_t value)
    {
        hash = HashByte(hash, static_cast<std::uint8_t>(value));
        hash = HashByte(hash, static_cast<std::uint8_t>(value >> 8));
        hash = HashByte(hash, static_cast<std::uint8_t>(value >> 16));
        return HashByte(hash, static_cast<std::uint8_t>(value >> 24));
    }

    std::uint64_t HashSingle(std::uint64_t hash, float value)
    {
        return HashInt32(hash, std::bit_cast<std::uint32_t>(value));
    }

    std::uint64_t HashString(std::uint64_t hash, const String& value)
    {
        for (const unsigned char part : value)
            hash = HashByte(hash, part);
        return HashByte(hash, 0);
    }

    std::uint64_t HashVector3(std::uint64_t hash, const Vector3& value)
    {
        hash = HashSingle(hash, value.X);
        hash = HashSingle(hash, value.Y);
        return HashSingle(hash, value.Z);
    }

    std::uint64_t HashMatrix(std::uint64_t hash, const Matrix& value)
    {
        hash = HashSingle(hash, value.M11); hash = HashSingle(hash, value.M12);
        hash = HashSingle(hash, value.M13); hash = HashSingle(hash, value.M14);
        hash = HashSingle(hash, value.M21); hash = HashSingle(hash, value.M22);
        hash = HashSingle(hash, value.M23); hash = HashSingle(hash, value.M24);
        hash = HashSingle(hash, value.M31); hash = HashSingle(hash, value.M32);
        hash = HashSingle(hash, value.M33); hash = HashSingle(hash, value.M34);
        hash = HashSingle(hash, value.M41); hash = HashSingle(hash, value.M42);
        hash = HashSingle(hash, value.M43); return HashSingle(hash, value.M44);
    }

    std::uint64_t HashTrack(const TrackLine& track)
    {
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (const TrackVertex& point : track.getPointsProperty())
        {
            hash = HashVector3(hash, point.pos);
            hash = HashVector3(hash, point.right);
            hash = HashVector3(hash, point.up);
            hash = HashVector3(hash, point.dir);
            hash = HashSingle(hash, point.uv.X);
            hash = HashSingle(hash, point.uv.Y);
            hash = HashSingle(hash, point.roadWidth);
        }
        return hash;
    }

    std::uint64_t HashTangentVertices(
        const std::vector<RacingGame::Graphics::TangentVertex>& vertices)
    {
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (const RacingGame::Graphics::TangentVertex& vertex : vertices)
        {
            hash = HashVector3(hash, vertex.pos);
            hash = HashSingle(hash, vertex.uv.X);
            hash = HashSingle(hash, vertex.uv.Y);
            hash = HashVector3(hash, vertex.normal);
            hash = HashVector3(hash, vertex.tangent);
        }
        return hash;
    }

    std::uint64_t HashIndices(const std::vector<std::int32_t>& indices)
    {
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (const std::int32_t index : indices)
            hash = HashInt32(hash, static_cast<std::uint32_t>(index));
        return hash;
    }

    std::uint64_t HashObjects(const Landscape& landscape)
    {
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (const Landscape::LandscapeObjectRecord& object :
             landscape.getLandscapeObjectRecordsProperty())
        {
            hash = HashString(hash, object.modelName);
            hash = HashMatrix(hash, object.matrix);
            hash = HashByte(hash, object.isNearTrackForShadowGeneration ? 1 : 0);
        }
        return hash;
    }

    bool ProbeLandscape(std::ostream& output)
    {
        Landscape landscape;
        std::uint64_t vertexHash = UINT64_C(14695981039346656037);
        for (const RacingGame::Graphics::TangentVertex& vertex :
             landscape.getVerticesProperty())
        {
            vertexHash = HashVector3(vertexHash, vertex.pos);
            vertexHash = HashSingle(vertexHash, vertex.uv.X);
            vertexHash = HashSingle(vertexHash, vertex.uv.Y);
            vertexHash = HashVector3(vertexHash, vertex.normal);
            vertexHash = HashVector3(vertexHash, vertex.tangent);
        }
        std::uint64_t indexHash = UINT64_C(14695981039346656037);
        for (const std::uint32_t index : landscape.getIndicesProperty())
            indexHash = HashInt32(indexHash, index);
        const bool passed =
            landscape.getVerticesProperty().size() == 66049 &&
            landscape.getIndicesProperty().size() == 393216 &&
            vertexHash == UINT64_C(0x60c95bf995da8abe) &&
            indexHash == UINT64_C(0x9741d6c3a11b82f4);
        output << "LANDSCAPE vertices=" << landscape.getVerticesProperty().size()
               << " indices=" << landscape.getIndicesProperty().size()
               << " vertexHash=" << std::hex << std::setw(16)
               << std::setfill('0') << vertexHash
               << " indexHash=" << std::setw(16) << indexHash << std::dec
               << " result=" << (passed ? "PASS" : "FAIL") << '\n';
        return passed;
    }

    void WriteVector3(std::ostream& output, const Vector3& value)
    {
        output << std::setprecision(9) << value.X << ',' << value.Y << ',' << value.Z;
    }

    const char* HelperTypeName(TrackData::RoadHelper::HelperType type)
    {
        using HelperType = TrackData::RoadHelper::HelperType;
        switch (type)
        {
        case HelperType::Tunnel: return "Tunnel";
        case HelperType::Palms: return "Palms";
        case HelperType::Laterns: return "Laterns";
        case HelperType::Reset: return "Reset";
        }
        return "Unknown";
    }

    void WriteOrientationPhases(std::ostream& output, const char* name,
                                const TrackLine& track, const Landscape& landscape)
    {
        const auto& points = track.getPointsProperty();
        const int count = points.getCountProperty() - 1;
        std::vector<Vector3> directions(static_cast<std::size_t>(count));
        std::vector<Vector3> preUps(static_cast<std::size_t>(count));
        Vector3 lastUp(0.0f, 0.0f, 1.0f);
        for (int index = 0; index < count; ++index)
        {
            Vector3 direction = points.getItem((index + 1) % count).pos -
                points.getItem(index == 0 ? count - 1 : index - 1).pos;
            direction.Normalize();
            const Vector3 middle =
                (points.getItem((index + 1) % count).pos +
                 points.getItem(index == 0 ? count - 1 : index - 1).pos) / 2.0f;
            Vector3 optimal = middle - points.getItem(index).pos;
            if (optimal.Length() < 0.0001f) optimal = lastUp;
            optimal.Normalize();
            directions[static_cast<std::size_t>(index)] = direction;
            preUps[static_cast<std::size_t>(index)] = optimal;
            lastUp = optimal;
        }
        preUps[0] = preUps.back() + preUps[1];
        // Match the original List<Vector3> indexer call, which normalizes a copy.

        lastUp = Vector3::Lerp(Vector3(0.0f, 0.0f, 1.0f), preUps[0],
                               1.5f * 0.25f * 0.6f);
        Vector3 up = Vector3::Zero;
        for (int smooth = -5; smooth <= 5; ++smooth)
        {
            up += preUps[static_cast<std::size_t>((count + smooth) % count)];
        }
        up.Normalize();
        up = Vector3::Lerp(lastUp, up, 0.25f);
        up.Normalize();
        Vector3 right = Vector3::Cross(directions[0], up);
        right.Normalize();
        Vector3 orthogonalUp = Vector3::Cross(right, directions[0]);
        orthogonalUp.Normalize();
        output << "PHASE name=" << name << " preUp0=";
        WriteVector3(output, preUps[0]);
        output << " secondRight0=";
        WriteVector3(output, right);
        output << " secondUp0=";
        WriteVector3(output, orthogonalUp);
        output << '\n';

        std::vector<Vector3> secondUps(static_cast<std::size_t>(count));
        std::vector<Vector3> secondRights(static_cast<std::size_t>(count));
        Vector3 lastUnmodified = lastUp;
        std::uint64_t groundHash = UINT64_C(14695981039346656037);
        int groundCount = 0;
        for (int index = 0; index < count; ++index)
        {
            up = Vector3::Zero;
            for (int smooth = -5; smooth <= 5; ++smooth)
            {
                up += preUps[static_cast<std::size_t>(
                    (index + count + smooth) % count)];
            }
            up.Normalize();
            const bool upsideDown = up.Z < -0.25f && lastUnmodified.Z < -0.05f;
            const bool movingUp = directions[static_cast<std::size_t>(index)].Z > 0.75f;
            const bool movingDown = directions[static_cast<std::size_t>(index)].Z < -0.75f;
            up = Vector3::Lerp(lastUp, up, 0.25f);
            up.Normalize();
            lastUnmodified = up;
            if (movingUp)
                lastUp = Vector3::Lerp(up, Vector3(0.0f, 0.0f, -1.0f), 0.6f);
            else if (movingDown)
                lastUp = Vector3::Lerp(up, Vector3(0.0f, 0.0f, 1.0f), 0.6f);
            else if (upsideDown)
                lastUp = Vector3::Lerp(up, Vector3(0.0f, 0.0f, -1.0f), 0.6f);
            else
                lastUp = Vector3::Lerp(up, Vector3(0.0f, 0.0f, 1.0f), 0.6f);
            const bool nearGround =
                points.getItem(index).pos.Z - landscape.GetMapHeight(
                    points.getItem(index).pos.X, points.getItem(index).pos.Y) < 8.0f;
            groundHash = HashByte(groundHash, nearGround ? 1 : 0);
            if (nearGround)
            {
                ++groundCount;
                lastUp = Vector3::Lerp(
                    up, Vector3(0.0f, 0.0f, 1.0f), 1.75f * 0.6f);
            }
            right = Vector3::Cross(directions[static_cast<std::size_t>(index)], up);
            right.Normalize();
            orthogonalUp = Vector3::Cross(
                right, directions[static_cast<std::size_t>(index)]);
            orthogonalUp.Normalize();
            secondRights[static_cast<std::size_t>(index)] = right;
            secondUps[static_cast<std::size_t>(index)] = orthogonalUp;
        }
        std::uint64_t preHash = UINT64_C(14695981039346656037);
        std::uint64_t secondHash = UINT64_C(14695981039346656037);
        std::uint64_t finalHash = UINT64_C(14695981039346656037);
        Vector3 finalRight0 = Vector3::Zero;
        Vector3 finalUp0 = Vector3::Zero;
        for (int index = 0; index < count; ++index)
        {
            preHash = HashVector3(preHash, preUps[static_cast<std::size_t>(index)]);
            secondHash = HashVector3(
                secondHash, secondRights[static_cast<std::size_t>(index)]);
            secondHash = HashVector3(
                secondHash, secondUps[static_cast<std::size_t>(index)]);
            up = Vector3::Zero;
            for (int smooth = -10; smooth <= 10; ++smooth)
            {
                up += secondUps[static_cast<std::size_t>(
                    (index + count + smooth) % count)];
            }
            up.Normalize();
            const Vector3 finalRight =
                Vector3::Cross(directions[static_cast<std::size_t>(index)], up);
            if (index == 0)
            {
                finalRight0 = finalRight;
                finalUp0 = up;
            }
            finalHash = HashVector3(
                finalHash, finalRight);
            finalHash = HashVector3(finalHash, up);
        }
        output << "PHASEHASH name=" << name
               << " pre=" << std::hex << std::setw(16) << std::setfill('0') << preHash
               << " second=" << std::setw(16) << secondHash
               << " final=" << std::setw(16) << finalHash
               << " ground=" << std::setw(16) << groundHash << std::dec
               << " groundCount=" << groundCount << '\n';
        output << "PHASEFINAL name=" << name << " right0=";
        WriteVector3(output, finalRight0);
        output << " up0=";
        WriteVector3(output, finalUp0);
        output << '\n';
    }

    void WriteTrackFieldHashes(std::ostream& output, const char* name,
                               const TrackLine& track)
    {
        std::uint64_t position = UINT64_C(14695981039346656037);
        std::uint64_t direction = UINT64_C(14695981039346656037);
        std::uint64_t right = UINT64_C(14695981039346656037);
        std::uint64_t up = UINT64_C(14695981039346656037);
        std::uint64_t uvWidth = UINT64_C(14695981039346656037);
        for (const TrackVertex& point : track.getPointsProperty())
        {
            position = HashVector3(position, point.pos);
            direction = HashVector3(direction, point.dir);
            right = HashVector3(right, point.right);
            up = HashVector3(up, point.up);
            uvWidth = HashSingle(uvWidth, point.uv.X);
            uvWidth = HashSingle(uvWidth, point.uv.Y);
            uvWidth = HashSingle(uvWidth, point.roadWidth);
        }
        output << "FIELDHASH name=" << name
               << " position=" << std::hex << std::setw(16) << std::setfill('0') << position
               << " direction=" << std::setw(16) << direction
               << " right=" << std::setw(16) << right
               << " up=" << std::setw(16) << up
               << " uvWidth=" << std::setw(16) << uvWidth << std::dec << '\n';
    }

    struct ExpectedGuardRail
    {
        std::size_t points;
        std::size_t vertices;
        std::size_t indices;
        std::uint64_t vertexHash;
        std::uint64_t indexHash;
        std::size_t holders;
        std::uint64_t holderHash;
    };

    struct ExpectedColumns
    {
        std::size_t columns;
        std::uint64_t positionHash;
        std::size_t vertices;
        std::size_t indices;
        std::uint64_t vertexHash;
        std::uint64_t indexHash;
        std::size_t objects;
        std::uint64_t objectHash;
    };

    struct ExpectedTrack
    {
        const char* name;
        int generatedPoints;
        int helperRanges;
        std::uint64_t hash;
        std::uint64_t objectHash;
        std::size_t topVertices;
        std::size_t topIndices;
        std::uint64_t topVertexHash;
        std::uint64_t topIndexHash;
        std::size_t backVertices;
        std::size_t backIndices;
        std::uint64_t backVertexHash;
        std::uint64_t backIndexHash;
        std::size_t tunnelVertices;
        std::size_t tunnelIndices;
        std::uint64_t tunnelVertexHash;
        std::uint64_t tunnelIndexHash;
        ExpectedGuardRail leftGuard;
        ExpectedGuardRail rightGuard;
        ExpectedColumns columns;
    };

    bool ProbeRoadGeometry(std::ostream& output, const ExpectedTrack& expected,
                           const TrackLine& track)
    {
        const TrackGeometry geometry(track);
        const auto& topVertices = geometry.getRoadVerticesProperty();
        const auto& topIndices = geometry.getRoadIndicesProperty();
        const auto& backVertices = geometry.getRoadBackVerticesProperty();
        const auto& backIndices = geometry.getRoadBackIndicesProperty();
        const auto& tunnelVertices = geometry.getRoadTunnelVerticesProperty();
        const auto& tunnelIndices = geometry.getRoadTunnelIndicesProperty();
        const std::uint64_t topVertexHash = HashTangentVertices(topVertices);
        const std::uint64_t topIndexHash = HashIndices(topIndices);
        const std::uint64_t backVertexHash = HashTangentVertices(backVertices);
        const std::uint64_t backIndexHash = HashIndices(backIndices);
        const std::uint64_t tunnelVertexHash = HashTangentVertices(tunnelVertices);
        const std::uint64_t tunnelIndexHash = HashIndices(tunnelIndices);
        const bool passed =
            topVertices.size() == expected.topVertices &&
            topIndices.size() == expected.topIndices &&
            topVertexHash == expected.topVertexHash &&
            topIndexHash == expected.topIndexHash &&
            backVertices.size() == expected.backVertices &&
            backIndices.size() == expected.backIndices &&
            backVertexHash == expected.backVertexHash &&
            backIndexHash == expected.backIndexHash &&
            tunnelVertices.size() == expected.tunnelVertices &&
            tunnelIndices.size() == expected.tunnelIndices &&
            tunnelVertexHash == expected.tunnelVertexHash &&
            tunnelIndexHash == expected.tunnelIndexHash;
        output << "ROAD name=" << expected.name
               << " topVertices=" << topVertices.size()
               << " topIndices=" << topIndices.size()
               << " topVertexHash=" << std::hex << std::setw(16)
               << std::setfill('0') << topVertexHash
               << " topIndexHash=" << std::setw(16) << topIndexHash << std::dec
               << " backVertices=" << backVertices.size()
               << " backIndices=" << backIndices.size()
               << " backVertexHash=" << std::hex << std::setw(16)
               << backVertexHash
               << " backIndexHash=" << std::setw(16) << backIndexHash << std::dec
               << " tunnelVertices=" << tunnelVertices.size()
               << " tunnelIndices=" << tunnelIndices.size()
               << " tunnelVertexHash=" << std::hex << std::setw(16)
               << tunnelVertexHash
               << " tunnelIndexHash=" << std::setw(16) << tunnelIndexHash
               << std::dec << " result=" << (passed ? "PASS" : "FAIL") << '\n';
        return passed;
    }

    std::uint64_t HashGuardRailHolders(
        const std::vector<Matrix>& holderMatrices)
    {
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (const Matrix& matrix : holderMatrices)
        {
            hash = HashString(hash, "GuardRailHolder");
            hash = HashMatrix(hash, matrix);
            hash = HashByte(hash, 0);
        }
        return hash;
    }

    bool ProbeGuardRail(std::ostream& output, const char* name,
                        const TrackLine& track, GuardRailGeometry::Mode mode,
                        const ExpectedGuardRail& expected)
    {
        const GuardRailGeometry geometry(track, mode);
        const std::uint64_t vertexHash =
            HashTangentVertices(geometry.getVerticesProperty());
        const std::uint64_t indexHash = HashIndices(geometry.getIndicesProperty());
        const std::uint64_t holderHash =
            HashGuardRailHolders(geometry.getHolderMatricesProperty());
        const bool passed =
            geometry.getRailPointCountProperty() == expected.points &&
            geometry.getVerticesProperty().size() == expected.vertices &&
            geometry.getIndicesProperty().size() == expected.indices &&
            vertexHash == expected.vertexHash && indexHash == expected.indexHash &&
            geometry.getHolderMatricesProperty().size() == expected.holders &&
            holderHash == expected.holderHash;
        output << "GUARD name=" << name
               << " mode="
               << (mode == GuardRailGeometry::Mode::Left ? "Left" : "Right")
               << " points=" << geometry.getRailPointCountProperty()
               << " vertices=" << geometry.getVerticesProperty().size()
               << " indices=" << geometry.getIndicesProperty().size()
               << " vertexHash=" << std::hex << std::setw(16)
               << std::setfill('0') << vertexHash
               << " indexHash=" << std::setw(16) << indexHash << std::dec
               << " holders=" << geometry.getHolderMatricesProperty().size()
               << " holderHash=" << std::hex << std::setw(16) << holderHash
               << std::dec << " result=" << (passed ? "PASS" : "FAIL") << '\n';
        return passed;
    }

    std::uint64_t HashColumnObjects(const std::vector<Vector3>& positions)
    {
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (const Vector3& position : positions)
        {
            hash = HashString(hash, "RoadColumnSegment");
            hash = HashMatrix(hash, Matrix::CreateTranslation(position));
            hash = HashByte(hash, 0);
        }
        return hash;
    }

    bool ProbeColumns(std::ostream& output, const char* name,
                      const TrackLine& track, const Landscape& landscape,
                      const ExpectedColumns& expected)
    {
        const TrackColumnsGeometry geometry(track, landscape);
        std::uint64_t positionHash = UINT64_C(14695981039346656037);
        for (const Vector3& position : geometry.getColumnPositionsProperty())
            positionHash = HashVector3(positionHash, position);
        const std::uint64_t vertexHash =
            HashTangentVertices(geometry.getVerticesProperty());
        const std::uint64_t indexHash = HashIndices(geometry.getIndicesProperty());
        const std::uint64_t objectHash =
            HashColumnObjects(geometry.getSegmentPositionsProperty());
        const bool passed =
            geometry.getColumnPositionsProperty().size() == expected.columns &&
            positionHash == expected.positionHash &&
            geometry.getVerticesProperty().size() == expected.vertices &&
            geometry.getIndicesProperty().size() == expected.indices &&
            vertexHash == expected.vertexHash && indexHash == expected.indexHash &&
            geometry.getSegmentPositionsProperty().size() == expected.objects &&
            objectHash == expected.objectHash;
        output << "COLUMN name=" << name
               << " columns=" << geometry.getColumnPositionsProperty().size()
               << " positionHash=" << std::hex << std::setw(16)
               << std::setfill('0') << positionHash << std::dec
               << " vertices=" << geometry.getVerticesProperty().size()
               << " indices=" << geometry.getIndicesProperty().size()
               << " vertexHash=" << std::hex << std::setw(16) << vertexHash
               << " indexHash=" << std::setw(16) << indexHash << std::dec
               << " objects=" << geometry.getSegmentPositionsProperty().size()
               << " objectHash=" << std::hex << std::setw(16) << objectHash
               << std::dec << " result=" << (passed ? "PASS" : "FAIL") << '\n';
        return passed;
    }

    void WriteKinematics(std::ostream& output, const char* name,
                         const Track& track)
    {
        std::uint64_t startHash = UINT64_C(14695981039346656037);
        startHash = HashVector3(startHash, track.getStartPositionProperty());
        startHash = HashVector3(startHash, track.getStartDirectionProperty());
        startHash = HashVector3(startHash, track.getStartUpVectorProperty());
        startHash = HashSingle(startHash, track.getLengthProperty());
        startHash = HashInt32(startHash, static_cast<std::uint32_t>(
            track.getNumberOfSegmentsProperty()));

        std::uint64_t percentHash = UINT64_C(14695981039346656037);
        constexpr std::array percents{
            -0.125f, 0.0f, 0.0001f, 0.249f,
            0.5f, 0.999f, 1.0f, 1.125f};
        for (const float percent : percents)
        {
            float roadWidth = 0.0f;
            float nextRoadWidth = 0.0f;
            const Matrix matrix = track.GetTrackPositionMatrix(
                percent, roadWidth, nextRoadWidth);
            percentHash = HashMatrix(percentHash, matrix);
            percentHash = HashSingle(percentHash, roadWidth);
            percentHash = HashSingle(percentHash, nextRoadWidth);
        }

        std::uint64_t segmentHash = UINT64_C(14695981039346656037);
        const int pointCount = track.getNumberOfSegmentsProperty();
        const std::array segments{
            0, 1, pointCount / 2, pointCount - 2, pointCount + 3};
        constexpr std::array segmentPercents{
            -0.5f, 0.0f, 0.37f, 1.0f, 1.5f};
        for (std::size_t index = 0; index < segments.size(); ++index)
        {
            float roadWidth = 0.0f;
            float nextRoadWidth = 0.0f;
            const Matrix matrix = track.GetTrackPositionMatrix(
                segments[index], segmentPercents[index],
                roadWidth, nextRoadWidth);
            segmentHash = HashMatrix(segmentHash, matrix);
            segmentHash = HashSingle(segmentHash, roadWidth);
            segmentHash = HashSingle(segmentHash, nextRoadWidth);
        }

        std::uint64_t updateHash = UINT64_C(14695981039346656037);
        const std::array targets{
            0, 7, pointCount / 3, pointCount - 6, 2};
        constexpr std::array targetPercents{
            0.2f, 0.75f, 0.41f, 0.9f, 0.05f};
        const std::array guesses{
            0, 4, pointCount / 3 - 3, pointCount - 10, pointCount - 2};
        constexpr std::array lateral{
            0.0f, 1.5f, -2.0f, 0.75f, -0.25f};
        for (std::size_t index = 0; index < targets.size(); ++index)
        {
            float roadWidth = 0.0f;
            float nextRoadWidth = 0.0f;
            const Matrix matrix = track.GetTrackPositionMatrix(
                targets[index], targetPercents[index],
                roadWidth, nextRoadWidth);
            const Vector3 carPosition = matrix.getTranslationProperty() +
                matrix.getRightProperty() * lateral[index] +
                matrix.getUpProperty() * 0.25f;
            int segment = guesses[index];
            float segmentPercent = -1.0f;
            track.UpdateCarTrackPosition(
                carPosition, segment, segmentPercent);
            updateHash = HashInt32(
                updateHash, static_cast<std::uint32_t>(segment));
            updateHash = HashSingle(updateHash, segmentPercent);
        }

        std::uint64_t tunnelHash = UINT64_C(14695981039346656037);
        for (int segment = 0; segment < pointCount; ++segment)
            tunnelHash = HashByte(
                tunnelHash, track.IsTunnel(segment) ? 1 : 0);

        output << "KINEMATIC name=" << name
               << " startHash=" << std::hex << std::setw(16)
               << std::setfill('0') << startHash
               << " percentHash=" << std::setw(16) << percentHash
               << " segmentHash=" << std::setw(16) << segmentHash
               << " updateHash=" << std::setw(16) << updateHash
               << " tunnelHash=" << std::setw(16) << tunnelHash
               << std::dec << '\n';
    }

    bool ProbeTrack(std::ostream& output, const ExpectedTrack& expected)
    {
        TrackData data = TrackData::Load(expected.name);
        const int inputPoints = data.getTrackPointsProperty().getCountProperty();
        const int widthHelpers = data.getWidthHelpersProperty().getCountProperty();
        const int roadHelpers = data.getRoadHelpersProperty().getCountProperty();
        const int neutralObjects = data.getNeutralsObjectsProperty().getCountProperty();
        Landscape landscape;
        Track track(expected.name, landscape);
        const std::uint64_t hash = HashTrack(track);
        const std::uint64_t objectHash = HashObjects(landscape);
        const bool passed =
            track.getPointsProperty().getCountProperty() == expected.generatedPoints &&
            track.getHelperPositionsProperty().getCountProperty() == expected.helperRanges &&
            landscape.getLandscapeObjectRecordsProperty().size() ==
                static_cast<std::size_t>(neutralObjects) &&
            hash == expected.hash && objectHash == expected.objectHash;

        output << "TRACK name=" << expected.name
               << " inputPoints=" << inputPoints
               << " widthHelpers=" << widthHelpers
               << " roadHelpers=" << roadHelpers
               << " neutralObjects=" << neutralObjects
               << " generatedPoints=" << track.getPointsProperty().getCountProperty()
               << " helperRanges=" << track.getHelperPositionsProperty().getCountProperty()
               << " emittedNeutralObjects="
               << landscape.getLandscapeObjectRecordsProperty().size()
               << " hash=" << std::hex << std::setw(16) << std::setfill('0') << hash
               << " objectHash=" << std::setw(16) << objectHash << std::dec
               << " result=" << (passed ? "PASS" : "FAIL") << '\n';

        const TrackVertex& first = track.getPointsProperty().getItem(0);
        const TrackVertex& finalUnique = track.getPointsProperty().getItem(
            track.getPointsProperty().getCountProperty() - 2);
        const TrackVertex& duplicate = track.getPointsProperty().getItem(
            track.getPointsProperty().getCountProperty() - 1);
        output << "FIRST name=" << expected.name << " pos=";
        WriteVector3(output, first.pos);
        output << " right=";
        WriteVector3(output, first.right);
        output << " up=";
        WriteVector3(output, first.up);
        output << " dir=";
        WriteVector3(output, first.dir);
        output << " width=" << first.roadWidth << " u=" << first.uv.X << '\n';
        output << "LAST name=" << expected.name << " pos=";
        WriteVector3(output, finalUnique.pos);
        output << " duplicatePos=";
        WriteVector3(output, duplicate.pos);
        output << " duplicateU=" << duplicate.uv.X << '\n';
        WriteTrackFieldHashes(output, expected.name, track);
        WriteKinematics(output, expected.name, track);
        WriteOrientationPhases(output, expected.name, track, landscape);
        const bool geometryPassed = ProbeRoadGeometry(output, expected, track);
        const bool leftGuardPassed = ProbeGuardRail(
            output, expected.name, track, GuardRailGeometry::Mode::Left,
            expected.leftGuard);
        const bool rightGuardPassed = ProbeGuardRail(
            output, expected.name, track, GuardRailGeometry::Mode::Right,
            expected.rightGuard);
        const bool columnsPassed = ProbeColumns(
            output, expected.name, track, landscape, expected.columns);
        for (const TrackLine::RoadHelperPosition& helper :
             track.getHelperPositionsProperty())
        {
            output << "HELPER name=" << expected.name
                   << " type=" << HelperTypeName(helper.type)
                   << " start=" << helper.startNum
                   << " end=" << helper.endNum << '\n';
        }
        return passed && geometryPassed && leftGuardPassed && rightGuardPassed &&
               columnsPassed;
    }

    struct ExpectedCombi
    {
        const char* name;
        float size;
        std::size_t objects;
        std::uint64_t hash;
    };

    bool ProbeCombi(std::ostream& output, const ExpectedCombi& expected)
    {
        Landscape landscape;
        TrackCombiModels combi(expected.name);
        combi.AddAllModels(landscape, Matrix::getIdentityProperty());
        const std::uint64_t hash = HashObjects(landscape);
        const bool passed =
            combi.getNameProperty() == expected.name &&
            combi.getSizeProperty() == expected.size &&
            landscape.getLandscapeObjectRecordsProperty().size() == expected.objects &&
            hash == expected.hash;
        output << "COMBI name=" << expected.name
               << " size=" << combi.getSizeProperty()
               << " objects=" << landscape.getLandscapeObjectRecordsProperty().size()
               << " hash=" << std::hex << std::setw(16) << std::setfill('0') << hash
               << std::dec << " result=" << (passed ? "PASS" : "FAIL") << '\n';
        return passed;
    }
}

int main(int argc, char** argv)
{
    std::ofstream file;
    std::ostream* output = &std::cout;
    if (argc == 2)
    {
        file.open(argv[1], std::ios::trunc);
        output = &file;
    }
    else if (argc != 1)
    {
        std::fprintf(stderr, "usage: RacingGameTrackProbe [report path]\n");
        return 2;
    }

    *output << "FORMAT racing-cna-track-oracle-v1\n";
    const ExpectedTrack expected[] = {
        {"TrackBeginner", 1332, 4, UINT64_C(0x16a26ce8b782756e),
         UINT64_C(0xdaf3e1af34c79af6),
         6660, 31944, UINT64_C(0x336ad55ab705c0a7),
         UINT64_C(0x5c7323d46f704f78),
         5328, 23958, UINT64_C(0xae4626744f7cf92f),
         UINT64_C(0x132472b7d52824ee),
         404, 1800, UINT64_C(0x815692bdd3e085f7),
         UINT64_C(0x80a1e0f12363a572),
         {667, 11339, 63936, UINT64_C(0xdbde88019a2d59b5),
          UINT64_C(0xa23b4d39677d07af), 224, UINT64_C(0x1498fbc398f4f361)},
         {667, 11339, 63936, UINT64_C(0x3b9af7d331bfbfc8),
          UINT64_C(0xa23b4d39677d07af), 231, UINT64_C(0x10584fbf9b69040f)},
         {101, UINT64_C(0xfddcb1bad328177b), 1414, 3636,
          UINT64_C(0x1394ed0aa1595d75), UINT64_C(0x93d42756ef7322ea),
          101, UINT64_C(0xb788b515f18b1424)}},
        {"TrackAdvanced", 2172, 8, UINT64_C(0x1f63b964ffb2df54),
         UINT64_C(0xa05e0bf61f7dccd5),
         10860, 52104, UINT64_C(0xea70784870f80332),
         UINT64_C(0x983861c5fa8b1857),
         8688, 39078, UINT64_C(0x0d863aaba48bdcec),
         UINT64_C(0x159b10d5037a5165),
         860, 3834, UINT64_C(0x7c4d521e7252affc),
         UINT64_C(0x5bd260dd6dbbfee7),
         {1087, 18479, 104256, UINT64_C(0x4182297e1f1eb5b5),
          UINT64_C(0xf50ad28905572751), 369, UINT64_C(0x1b51a6b47038c5d4)},
         {1087, 18479, 104256, UINT64_C(0x038dfe57b9c7d590),
          UINT64_C(0xf50ad28905572751), 374, UINT64_C(0xca68f162253d8a79)},
         {160, UINT64_C(0x787be2626ebf3d68), 2240, 5760,
          UINT64_C(0xf5eea4fa461bbe2b), UINT64_C(0x17addfba63965572),
          160, UINT64_C(0x1224f4be7b985207)}},
        {"TrackExpert", 4466, 20, UINT64_C(0xf7ae13e7762f5b8f),
         UINT64_C(0xcb278caf8b90b8d3),
         22330, 107160, UINT64_C(0x8b2d97f77f908deb),
         UINT64_C(0x005cc303d40fa803),
         17864, 80370, UINT64_C(0xe7733419a825e9b9),
         UINT64_C(0xec9f4d520e9575c1),
         896, 3852, UINT64_C(0x9c2a761df64aa772),
         UINT64_C(0xc3b61cace7b82f77),
         {2234, 37978, 214368, UINT64_C(0x752c02598043ed51),
          UINT64_C(0x35b754cbc2c213b1), 765, UINT64_C(0x94962432c02d6105)},
         {2234, 37978, 214368, UINT64_C(0x81f0e89e4e49cd70),
          UINT64_C(0x35b754cbc2c213b1), 764, UINT64_C(0x859c61168c610d8f)},
         {337, UINT64_C(0x64be083741863ba8), 4718, 12132,
          UINT64_C(0xa78c00e99e7a3cfc), UINT64_C(0x1768841d777d2c51),
          337, UINT64_C(0x184093ef54760b6a)}},
    };

    bool passed = ProbeLandscape(*output);
    for (const ExpectedTrack& track : expected)
        passed = ProbeTrack(*output, track) && passed;
    const ExpectedCombi expectedCombis[] = {
        {"CombiBuildings", 50.0f, 8, UINT64_C(0x37c3b0974a44ebf7)},
        {"CombiHotels", 50.0f, 6, UINT64_C(0xe2a28eb4ed7d5738)},
        {"CombiOilTanks", 50.0f, 5, UINT64_C(0x205df53566403859)},
        {"CombiPalms", 10.0f, 3, UINT64_C(0x4a6dbb07008c5b57)},
        {"CombiPalms2", 10.0f, 5, UINT64_C(0x77e7a0554a60426a)},
        {"CombiRuins", 10.0f, 11, UINT64_C(0xb6e9f3dda25e38e6)},
        {"CombiRuins2", 10.0f, 8, UINT64_C(0x198164c394ddaa0d)},
        {"CombiSandCastle", 50.0f, 10, UINT64_C(0xdbe2bebb6fa22075)},
        {"CombiStones", 10.0f, 7, UINT64_C(0x474560198b2dfbf3)},
        {"CombiStones2", 10.0f, 16, UINT64_C(0xdd8f0126d783328d)},
    };
    for (const ExpectedCombi& combi : expectedCombis)
        passed = ProbeCombi(*output, combi) && passed;
    *output << "RESULT " << (passed ? "PASS" : "FAIL") << '\n';
    return passed ? 0 : 1;
}
