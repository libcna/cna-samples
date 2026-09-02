// SPDX-License-Identifier: MS-PL

#include "Tracks/TrackLine.hpp"

#include <cmath>
#include <stdexcept>

#include "Landscapes/Landscape.hpp"

namespace RacingGame::Tracks
{
    using Landscapes::Landscape;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    const std::vector<Vector3> TrackLine::LoopingPoints = {
        Vector3(0.0f, 0.0f, 0.0f),
        Vector3(0.0f, 0.353553f, 0.146447f),
        Vector3(0.0f, 0.5f, 0.5f),
        Vector3(0.0f, 0.353553f, 1.0f - 0.146447f),
        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.0f, -0.353553f, 1.0f - 0.146447f),
        Vector3(0.0f, -0.5f, 0.5f),
        Vector3(0.0f, -0.353553f, 0.146447f),
        Vector3(0.0f, 0.0f, 0.0f),
    };

    TrackLine::RoadHelperPosition::RoadHelperPosition(
        TrackData::RoadHelper::HelperType setType, int setStartNum, int setEndNum)
        : type(setType), startNum(setStartNum), endNum(setEndNum)
    {
    }

    TrackLine::TrackLine(std::vector<Vector3> inputPoints,
                         TrackData::WidthHelperList widthHelpers,
                         TrackData::RoadHelperList roadHelpers,
                         TrackData::NeutralObjectList neutralObjects,
                         Landscape* landscape)
    {
        Load(std::move(inputPoints), std::move(widthHelpers),
             std::move(roadHelpers), std::move(neutralObjects), landscape);
    }

    TrackLine::TrackLine(std::vector<Vector3> inputPoints)
        : TrackLine(std::move(inputPoints), {}, {}, {}, nullptr)
    {
    }

    TrackLine::TrackLine(TrackData inputPointsFromTrack, Landscape* landscape)
    {
        Load(std::move(inputPointsFromTrack), landscape);
    }

    const System::Collections::Generic::List<TrackVertex>&
    TrackLine::getPointsProperty() const
    {
        return points;
    }

    const System::Collections::Generic::List<TrackLine::RoadHelperPosition>&
    TrackLine::getHelperPositionsProperty() const
    {
        return helperPositions;
    }

    void TrackLine::Load(std::vector<Vector3> inputPoints,
                         TrackData::WidthHelperList widthHelpers,
                         TrackData::RoadHelperList roadHelpers,
                         TrackData::NeutralObjectList neutralObjects,
                         Landscape* landscape)
    {
        points.Clear();
        helperPositions.Clear();
        if (landscape != nullptr) landscape->KillAllLoadedObjects();

        if (inputPoints.size() < 3)
        {
            throw std::invalid_argument(
                "inputPoints is invalid, we need at least 3 valid input points to generate a TrackLine.");
        }

        if (landscape != nullptr)
        {
            for (std::size_t num = 0; num < inputPoints.size(); ++num)
            {
                const float landscapeHeight = landscape->GetMapHeight(
                    inputPoints[num].X, inputPoints[num].Y) +
                    MinimumLandscapeDistance * 2.25f;
                if (inputPoints[num].Z < landscapeHeight)
                    inputPoints[num].Z = landscapeHeight;
            }

            for (std::size_t num = 0; num < inputPoints.size(); ++num)
            {
                for (int iter = 1; iter < 25; ++iter)
                {
                    const float iterPercent = iter / 25.0f;
                    float iterHeight =
                        inputPoints[num].Z * (1.0f - iterPercent) +
                        inputPoints[(num + 1) % inputPoints.size()].Z * iterPercent;

                    for (int x = 0; x < 2; ++x)
                    {
                        for (int y = 0; y < 2; ++y)
                        {
                            const float landscapeHeight = landscape->GetMapHeight(
                                -5.0f + 10.0f * x +
                                    inputPoints[num].X * (1.0f - iterPercent) +
                                    inputPoints[(num + 1) % inputPoints.size()].X * iterPercent,
                                -5.0f + 10.0f * y +
                                    inputPoints[num].Y * (1.0f - iterPercent) +
                                    inputPoints[(num + 1) % inputPoints.size()].Y * iterPercent) +
                                MinimumLandscapeDistance * 1.6f;

                            if (iterHeight < landscapeHeight)
                            {
                                const float increaseHeight = landscapeHeight - iterHeight;
                                inputPoints[num].Z += increaseHeight;
                                inputPoints[(num + 1) % inputPoints.size()].Z += increaseHeight;
                            }
                        }
                    }
                }
            }
        }

        for (std::size_t num = 1; num + 3 < inputPoints.size(); ++num)
        {
            const Vector3 distanceVector = inputPoints[num + 1] - inputPoints[num];
            const float xyDistance = std::sqrt(distanceVector.X * distanceVector.X +
                                               distanceVector.Y * distanceVector.Y);
            const float zDistance = std::abs(distanceVector.Z);
            const Vector3 nextDistance = inputPoints[num + 2] - inputPoints[num + 1];
            if (zDistance / 2.0f > xyDistance &&
                std::abs(distanceVector.Z + nextDistance.Z) < zDistance / 2.0f)
            {
                Vector3 direction = inputPoints[num] - inputPoints[num - 1];
                direction.Normalize();
                const Vector3 upVector(0.0f, 0.0f, 1.0f);
                const Vector3 rightVector = Vector3::Cross(direction, upVector);
                const Matrix rotation(
                    rightVector.X, rightVector.Y, rightVector.Z, 0.0f,
                    direction.X, direction.Y, direction.Z, 0.0f,
                    upVector.X, upVector.Y, upVector.Z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
                const Vector3 startLoopPosition = inputPoints[num];
                const Vector3 endLoopPosition = inputPoints[num + 2];

                const std::vector<Vector3> remainingInputPoints = inputPoints;
                inputPoints.assign(remainingInputPoints.size() + 7, Vector3::Zero);
                for (std::size_t copyNum = 0;
                     copyNum < remainingInputPoints.size(); ++copyNum)
                {
                    if (copyNum < num)
                        inputPoints[copyNum] = remainingInputPoints[copyNum];
                    else
                        inputPoints[copyNum + 7] = remainingInputPoints[copyNum];
                }

                for (std::size_t loopNum = 0; loopNum < LoopingPoints.size(); ++loopNum)
                {
                    const float loopPercent =
                        loopNum / static_cast<float>(LoopingPoints.size() - 1);
                    inputPoints[num + loopNum] =
                        startLoopPosition * (1.0f - loopPercent) +
                        endLoopPosition * loopPercent +
                        zDistance * Vector3::Transform(LoopingPoints[loopNum], rotation);
                }

                Vector3 newRoadDirection = inputPoints[num + 10] - inputPoints[num + 8];
                if (newRoadDirection.Length() > zDistance * 2.0f)
                {
                    newRoadDirection.Normalize();
                    newRoadDirection *= zDistance;
                    inputPoints[num + 9] = inputPoints[num + 8] + newRoadDirection;
                }
                else
                {
                    inputPoints[num + 9] =
                        (inputPoints[num + 8] + inputPoints[num + 10]) / 2.0f;
                }
                num += 10;
            }
        }

        for (std::size_t num = 0; num < inputPoints.size(); ++num)
        {
            const Vector3 p1 = inputPoints[num == 0 ? inputPoints.size() - 1 : num - 1];
            const Vector3 p2 = inputPoints[num];
            const Vector3 p3 = inputPoints[(num + 1) % inputPoints.size()];
            const Vector3 p4 = inputPoints[(num + 2) % inputPoints.size()];
            const float distance = Vector3::Distance(p2, p3);
            int numberOfIterations = static_cast<int>(
                NumberOfIterationsPer100Meters * (distance / 100.0f));
            if (numberOfIterations <= 0) numberOfIterations = 1;
            for (int iter = 0; iter < numberOfIterations; ++iter)
            {
                points.Add(TrackVertex(Vector3::CatmullRom(
                    p1, p2, p3, p4,
                    iter / static_cast<float>(numberOfIterations))));
            }
        }

        const int pointCount = points.getCountProperty();
        std::vector<Vector3> preUpVectors;
        preUpVectors.reserve(static_cast<std::size_t>(pointCount));
        const Vector3 defaultUpVector(0.0f, 0.0f, 1.0f);
        Vector3 lastUpVector = defaultUpVector;
        for (int num = 0; num < pointCount; ++num)
        {
            TrackVertex point = points.getItem(num);
            Vector3 direction =
                points.getItem((num + 1) % pointCount).pos -
                points.getItem(num == 0 ? pointCount - 1 : num - 1).pos;
            direction.Normalize();
            const Vector3 middlePoint =
                (points.getItem((num + 1) % pointCount).pos +
                 points.getItem(num == 0 ? pointCount - 1 : num - 1).pos) / 2.0f;
            Vector3 optimalUpVector = middlePoint - point.pos;
            if (optimalUpVector.Length() < 0.0001f)
                optimalUpVector = lastUpVector;
            optimalUpVector.Normalize();
            preUpVectors.push_back(optimalUpVector);
            point.dir = direction;
            points.setItem(num, point);
            lastUpVector = optimalUpVector;
        }
        preUpVectors[0] = preUpVectors.back() + preUpVectors[1];
        // List<Vector3>'s C# indexer returns a value, so the original Normalize()
        // call operates on a temporary and does not change the stored vector.

        lastUpVector = Vector3::Lerp(
            defaultUpVector, preUpVectors[0],
            1.5f * CurveFactor * UpFactorCorrector);
        Vector3 lastUnmodifiedUpVector = lastUpVector;
        for (int num = 0; num < pointCount; ++num)
        {
            TrackVertex point = points.getItem(num);
            const Vector3 direction = point.dir;
            Vector3 upVector = Vector3::Zero;
            for (int smoothNum = -NumberOfUpSmoothValues / 2;
                 smoothNum <= NumberOfUpSmoothValues / 2; ++smoothNum)
            {
                upVector += preUpVectors[static_cast<std::size_t>(
                    (num + pointCount + smoothNum) % pointCount)];
            }
            upVector.Normalize();
            const bool upsideDown =
                upVector.Z < -0.25f && lastUnmodifiedUpVector.Z < -0.05f;
            const bool movingUp = direction.Z > 0.75f;
            const bool movingDown = direction.Z < -0.75f;
            upVector = Vector3::Lerp(lastUpVector, upVector, CurveFactor);
            upVector.Normalize();
            lastUnmodifiedUpVector = upVector;

            if (movingUp)
                lastUpVector = Vector3::Lerp(upVector, -defaultUpVector,
                                             UpFactorCorrector);
            else if (movingDown)
                lastUpVector = Vector3::Lerp(upVector, defaultUpVector,
                                             UpFactorCorrector);
            else if (upsideDown)
                lastUpVector = Vector3::Lerp(upVector, -defaultUpVector,
                                             UpFactorCorrector);
            else
                lastUpVector = Vector3::Lerp(upVector, defaultUpVector,
                                             UpFactorCorrector);

            if (landscape != nullptr)
            {
                const float landscapeHeight =
                    landscape->GetMapHeight(point.pos.X, point.pos.Y);
                if (point.pos.Z - landscapeHeight < MinimumLandscapeDistance * 4.0f)
                {
                    lastUpVector = Vector3::Lerp(
                        upVector, defaultUpVector, 1.75f * UpFactorCorrector);
                }
            }

            Vector3 rightVector = Vector3::Cross(direction, upVector);
            rightVector.Normalize();
            point.right = rightVector;
            upVector = Vector3::Cross(rightVector, direction);
            upVector.Normalize();
            point.up = upVector;
            points.setItem(num, point);
        }

        lastUpVector = points.getItem(0).up;
        for (int num = 0; num < pointCount; ++num)
            preUpVectors[static_cast<std::size_t>(num)] = points.getItem(num).up;
        for (int num = 0; num < pointCount; ++num)
        {
            TrackVertex point = points.getItem(num);
            Vector3 upVector = Vector3::Zero;
            for (int smoothNum = -NumberOfUpSmoothValues;
                 smoothNum <= NumberOfUpSmoothValues; ++smoothNum)
            {
                upVector += preUpVectors[static_cast<std::size_t>(
                    (num + pointCount + smoothNum) % pointCount)];
            }
            upVector.Normalize();
            point.up = upVector;
            point.right = Vector3::Cross(point.dir, upVector);
            points.setItem(num, point);
        }

        AdjustRoadWidths(widthHelpers);
        GenerateUTextureCoordinates();
        GenerateTunnelsAndLandscapeObjects(
            std::move(roadHelpers), neutralObjects, landscape);
    }

    void TrackLine::Load(TrackData trackData, Landscape* landscape)
    {
        Load(trackData.getTrackPointsProperty().ToArray(),
             trackData.getWidthHelpersProperty(),
             trackData.getRoadHelpersProperty(),
             trackData.getNeutralsObjectsProperty(), landscape);
    }

    void TrackLine::AdjustRoadWidths(const TrackData::WidthHelperList& widthHelpers)
    {
        float currentWidth = TrackVertex::DefaultRoadWidth;
        float widthInfluence = currentWidth;
        const int pointCount = points.getCountProperty();
        for (int num = 0; num < pointCount; ++num)
        {
            TrackVertex point = points.getItem(num);
            for (const TrackData::WidthHelper& widthHelper : widthHelpers)
            {
                const float distance = Vector3::Distance(widthHelper.pos, point.pos);
                if (distance < 25.0f)
                {
                    const float influence = 1.0f - distance / 25.0f;
                    widthInfluence = (1.0f - influence) * widthInfluence +
                                     influence * widthHelper.scale;
                }
            }
            currentWidth = currentWidth * 0.9f + widthInfluence * 0.1f;
            if (num > pointCount - 7)
            {
                // The repeated Count-2 condition is present in the original XNA source.
                const float influence =
                    num == pointCount - 1 ? 0.75f :
                    num == pointCount - 2 ? 0.5f :
                    num == pointCount - 2 ? 0.25f : 0.175f;
                currentWidth = influence * points.getItem(0).roadWidth +
                               (1.0f - influence) * currentWidth;
            }
            if (currentWidth < TrackVertex::MinRoadWidth)
                currentWidth = TrackVertex::MinRoadWidth;
            if (currentWidth > TrackVertex::MaxRoadWidth)
                currentWidth = TrackVertex::MaxRoadWidth;
            point.roadWidth = currentWidth;
            points.setItem(num, point);
        }
    }

    void TrackLine::GenerateUTextureCoordinates()
    {
        float currentRoadU = 0.0f;
        const int pointCount = points.getCountProperty();
        for (int num = 0; num < pointCount; ++num)
        {
            TrackVertex point = points.getItem(num);
            point.uv.X = currentRoadU;
            points.setItem(num, point);
            currentRoadU += RoadTextureStrechFactor *
                (points.getItem((num + 1) % pointCount).pos - point.pos).Length();
        }
        const TrackVertex& first = points.getItem(0);
        points.Add(TrackVertex(first.pos, first.right, first.up, first.dir,
                              Vector2(currentRoadU, 0.0f), first.roadWidth));
    }

    void TrackLine::GenerateTunnelsAndLandscapeObjects(
        TrackData::RoadHelperList roadHelpers,
        const TrackData::NeutralObjectList& neutralObjects,
        Landscape* landscape)
    {
        int helperStartedNum = -1;
        TrackData::RoadHelper::HelperType rememberedType =
            TrackData::RoadHelper::HelperType::Reset;
        for (int num = 0; num < points.getCountProperty(); ++num)
        {
            const Vector3 position = points.getItem(num).pos;
            for (int helperIndex = 0;
                 helperIndex < roadHelpers.getCountProperty(); ++helperIndex)
            {
                const TrackData::RoadHelper helper = roadHelpers.getItem(helperIndex);
                if (Vector3::Distance(helper.pos, position) < 25.0f)
                {
                    if (helperStartedNum >= 0)
                    {
                        helperPositions.Add(RoadHelperPosition(
                            rememberedType, helperStartedNum, num));
                        if (helper.type == TrackData::RoadHelper::HelperType::Reset)
                            helperStartedNum = -1;
                        else
                        {
                            helperStartedNum = num;
                            rememberedType = helper.type;
                        }
                    }
                    else
                    {
                        helperStartedNum = num;
                        rememberedType = helper.type;
                    }
                    roadHelpers.RemoveAt(helperIndex);
                    break;
                }
            }
        }
        if (helperStartedNum > 0)
        {
            helperPositions.Add(RoadHelperPosition(
                rememberedType, helperStartedNum, points.getCountProperty() - 3));
        }

        if (landscape != nullptr)
        {
            for (const TrackData::NeutralObject& object : neutralObjects)
            {
                landscape->AddObjectToRender(object.modelName, object.matrix, false);
            }
        }
    }
}
