// SPDX-License-Identifier: MS-PL

#include "Tracks/Track.hpp"

#include "Helpers/Vector3Helper.hpp"
#include "Landscapes/Landscape.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Tracks/TrackData.hpp"

namespace RacingGame::Tracks
{
    using Helpers::Vector3Helper;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    Track::Track(const SharpRuntime::String& setTrackName,
                 Landscapes::Landscape& landscape)
        : TrackLine(TrackData::Load(setTrackName), &landscape)
    {
        GenerateCheckpointSegmentPositions();
    }

    Vector3 Track::getStartPositionProperty() const
    {
        return points.getItem(0).pos;
    }

    Vector3 Track::getStartDirectionProperty() const
    {
        return points.getItem(0).dir;
    }

    Vector3 Track::getStartUpVectorProperty() const
    {
        return points.getItem(0).up;
    }

    float Track::getLengthProperty() const
    {
        return points.getCountProperty() * 100.0f /
               static_cast<float>(NumberOfIterationsPer100Meters);
    }

    int Track::getNumberOfSegmentsProperty() const
    {
        return points.getCountProperty();
    }

    const std::vector<int>&
    Track::getCheckpointSegmentPositionsProperty() const
    {
        return checkpointSegmentPositions;
    }

    void Track::GenerateCheckpointSegmentPositions()
    {
        checkpointSegmentPositions.clear();
        float lastGap = CheckpointGap;
        const int pointCount = points.getCountProperty();
        for (int num = 0; num < pointCount - 24; ++num)
        {
            const float distance = Vector3::Distance(
                points.getItem((num + 1) % pointCount).pos,
                points.getItem(num).pos);
            const Vector3 direction = points.getItem(num).dir;
            const Vector3 up = points.getItem(num).up;
            const bool upsideDown = up.Z < 0.05f;
            const bool movingUp = direction.Z > 0.65f;
            const bool movingDown = direction.Z < -0.65f;
            if (upsideDown || movingUp || movingDown)
                continue;

            if (lastGap - distance <= 0.0f)
            {
                checkpointSegmentPositions.push_back(num);
                lastGap += CheckpointGap;
            }
            lastGap -= distance;
        }
    }

    Matrix Track::GetTrackPositionMatrix(
        float trackPositionPercent, float& roadWidth,
        float& nextRoadWidth) const
    {
        while (trackPositionPercent < 0.0f)
            trackPositionPercent += 1.0f;
        while (trackPositionPercent > 1.0f)
            trackPositionPercent -= 1.0f;
        const int pointCount = points.getCountProperty();
        const int num = static_cast<int>(trackPositionPercent * pointCount) %
                        pointCount;

        const TrackVertex p1 = points.getItem(
            num - 1 < 0 ? pointCount - 1 : num - 1);
        const TrackVertex p2 = points.getItem(num);
        const TrackVertex p3 = points.getItem((num + 1) % pointCount);
        const TrackVertex p4 = points.getItem((num + 2) % pointCount);

        const float eachPointPercent = 1.0f / static_cast<float>(pointCount);
        const float pointPercent =
            (trackPositionPercent - num * eachPointPercent) /
            eachPointPercent;
        const Vector3 interpolatedPos = Vector3::CatmullRom(
            p1.pos, p2.pos, p3.pos, p4.pos, pointPercent);
        const Vector3 interpolatedDir = Vector3::CatmullRom(
            p1.dir, p2.dir, p3.dir, p4.dir, pointPercent);
        const Vector3 interpolatedRight = Vector3::CatmullRom(
            p1.right, p2.right, p3.right, p4.right, pointPercent);
        const Vector3 interpolatedUp = Vector3::CatmullRom(
            p1.up, p2.up, p3.up, p4.up, pointPercent);

        Matrix mat = Matrix::getIdentityProperty();
        mat.setRightProperty(interpolatedRight);
        mat.setUpProperty(interpolatedUp);
        mat.setForwardProperty(interpolatedDir);
        mat.setTranslationProperty(interpolatedPos);

        roadWidth = MathHelper::Lerp(
                        p2.roadWidth, p3.roadWidth, pointPercent) *
                    TrackVertex::RoadWidthScale;
        nextRoadWidth = p4.roadWidth * TrackVertex::RoadWidthScale;
        return mat;
    }

    Matrix Track::GetTrackPositionMatrix(
        const int trackSegmentNum, float trackSegmentPercent,
        float& roadWidth, float& nextRoadWidth) const
    {
        if (trackSegmentPercent < 0.0f)
            trackSegmentPercent = 0.0f;
        if (trackSegmentPercent > 1.0f)
            trackSegmentPercent = 1.0f;
        const float pointPercent = trackSegmentPercent;
        const int pointCount = points.getCountProperty();
        const int num = trackSegmentNum % pointCount;

        const TrackVertex p1 = points.getItem(
            num - 1 < 0 ? pointCount - 1 : num - 1);
        const TrackVertex p2 = points.getItem(num);
        const TrackVertex p3 = points.getItem((num + 1) % pointCount);
        const TrackVertex p4 = points.getItem((num + 2) % pointCount);

        const Vector3 interpolatedPos = Vector3::CatmullRom(
            p1.pos, p2.pos, p3.pos, p4.pos, pointPercent);
        const Vector3 interpolatedDir = Vector3::CatmullRom(
            p1.dir, p2.dir, p3.dir, p4.dir, pointPercent);
        const Vector3 interpolatedRight = Vector3::CatmullRom(
            p1.right, p2.right, p3.right, p4.right, pointPercent);
        const Vector3 interpolatedUp = Vector3::CatmullRom(
            p1.up, p2.up, p3.up, p4.up, pointPercent);

        Matrix mat = Matrix::getIdentityProperty();
        mat.setRightProperty(interpolatedRight);
        mat.setUpProperty(interpolatedUp);
        mat.setForwardProperty(interpolatedDir);
        mat.setTranslationProperty(interpolatedPos);

        roadWidth = MathHelper::Lerp(
                        p2.roadWidth, p3.roadWidth, pointPercent) *
                    TrackVertex::RoadWidthScale;
        nextRoadWidth = MathHelper::Lerp(
                            p3.roadWidth, p4.roadWidth, pointPercent) *
                        TrackVertex::RoadWidthScale;
        return mat;
    }

    void Track::UpdateCarTrackPosition(
        const Vector3 carPos, int& trackSegmentNumber,
        float& trackSegmentPercent) const
    {
        int num = trackSegmentNumber;
        bool gotCarInThisSegment = false;
        float thisPointDist = 0.0f;
        float nextPointDist = 1.0f;
        int maxNumberOfIterations = 100;
        do
        {
            const TrackVertex thisPoint = points.getItem(num);
            const TrackVertex nextPoint = points.getItem(
                (num + 1) % points.getCountProperty());
            thisPointDist = Vector3Helper::SignedDistanceToPlane(
                carPos, thisPoint.pos, -thisPoint.dir);
            nextPointDist = Vector3Helper::SignedDistanceToPlane(
                carPos, nextPoint.pos, nextPoint.dir);
            if (thisPointDist < 0.0f)
                --num;
            else if (nextPointDist < 0.0f)
                ++num;
            else
                gotCarInThisSegment = true;

            if (num < 0)
                num = points.getCountProperty() - 1;
            if (num >= points.getCountProperty())
                num = 0;
            if (maxNumberOfIterations-- < 0)
                return;
        } while (!gotCarInThisSegment);

        trackSegmentNumber = num;
        const float segmentLength = thisPointDist + nextPointDist;
        if (segmentLength == 0.0f)
            trackSegmentPercent = 0.0f;
        else
            trackSegmentPercent = thisPointDist / segmentLength;
    }

    bool Track::IsTunnel(const int trackSegment) const
    {
        for (const RoadHelperPosition& helper : helperPositions)
        {
            if (helper.type == TrackData::RoadHelper::HelperType::Tunnel &&
                trackSegment >= helper.startNum &&
                trackSegment <= helper.endNum)
            {
                return true;
            }
        }
        return false;
    }
}
