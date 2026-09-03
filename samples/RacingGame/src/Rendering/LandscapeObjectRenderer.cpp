// SPDX-License-Identifier: MS-PL

#include "Rendering/LandscapeObjectRenderer.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <stdexcept>

#include "Graphics/Model.hpp"
#include "Helpers/RandomHelper.hpp"
#include "Helpers/Vector3Helper.hpp"
#include "Landscapes/Landscape.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Tracks/Track.hpp"
#include "Tracks/TrackVertex.hpp"

namespace RacingGame::Rendering
{
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using namespace Microsoft::Xna::Framework::Graphics;

    namespace
    {
        constexpr std::array<const char*, 53> ModelNames = {
            "StartLight", "StartLight2", "StartLight3", "Blockade",
            "Blockade2", "Hydrant", "Kaktus", "Kaktus2", "KaktusBenny",
            "KaktusSeg", "AlphaDeadTree", "AlphaPalm", "AlphaPalm2",
            "AlphaPalm3", "AlphaPalmSmall", "Laterne", "Laterne2Sides",
            "Trashcan", "Roadsign", "Roadsign2", "Goal", "Building",
            "Building2", "Building3", "Building4", "Building5", "OilPump",
            "OilTanks", "RoadColumnSegment", "Windmill", "Ruin", "RuinHouse",
            "SandCastle", "Banner", "Banner2", "Banner3", "Banner4",
            "Banner5", "Banner6", "Sign", "Sign2", "SignWarning",
            "SignCurveLeft", "SignCurveRight", "SharpRock", "SharpRock2",
            "Stone4", "Stone5", "AlphaTrain", "GuardRailHolder", "Hotel01",
            "Hotel02", "Casino01",
        };
        constexpr std::array<const char*, 10> CombinationNames = {
            "CombiPalms", "CombiPalms2", "CombiRuins", "CombiRuins2",
            "CombiStones", "CombiStones2", "CombiOilTanks",
            "CombiSandCastle", "CombiBuildings", "CombiHotels",
        };
        constexpr std::array<const char*, 30> AutoGenerationNames = {
            "CombiPalms", "CombiPalms2", "CombiRuins", "CombiRuins2",
            "CombiStones", "CombiStones2", "Kaktus", "Kaktus2",
            "KaktusBenny", "KaktusSeg", "AlphaDeadTree", "AlphaPalm",
            "AlphaPalm2", "AlphaPalm3", "AlphaPalmSmall", "Laterne2Sides",
            "Trashcan", "OilPump", "OilTanks", "RoadColumnSegment",
            "Windmill", "Ruin", "RuinHouse", "Sign", "Sign2", "SharpRock",
            "SharpRock2", "Stone4", "Stone5", "Casino01",
        };
        constexpr float PalmAndLaternGap = 20.0f;
        constexpr float CheckpointGap = 500.0f;
        constexpr float SignGap = 24.0f;
    }

    LandscapeObjectRenderer::LandscapeObjectRenderer(
        GraphicsDevice& setDevice,
        Microsoft::Xna::Framework::Content::ContentManager& content,
        const Landscapes::Landscape& setLandscape, const Tracks::Track& track,
        const std::vector<Matrix>& leftHolders,
        const std::vector<Matrix>& rightHolders,
        const std::vector<Vector3>& columnSegments)
        : device(setDevice), landscape(setLandscape),
          normalEffect(content.Load<std::shared_ptr<Effect>>(
              "Shaders/NormalMapping")),
          reflectionTexture(std::make_unique<TextureCube>(
              content.Load<TextureCube>("Textures/SkyCubeMap")))
    {
        if (!normalEffect)
            throw std::runtime_error(
                "Authentic Racing NormalMapping effect failed to load");
        models.reserve(ModelNames.size());
        for (const char* modelName : ModelNames)
        {
            auto model = std::make_unique<Graphics::Model>(
                modelName, content, meshManager);
            modelByName.emplace(modelName, model.get());
            models.push_back(std::move(model));
        }
        combinations.reserve(CombinationNames.size());
        for (const char* combinationName : CombinationNames)
            combinations.emplace_back(combinationName);

        for (const auto& request : landscape.getLandscapeObjectRecordsProperty())
            AddObject(request.modelName, request.matrix,
                      request.isNearTrackForShadowGeneration);
        for (const Matrix& matrix : leftHolders)
            AddObject("GuardRailHolder", matrix, false);
        for (const Matrix& matrix : rightHolders)
            AddObject("GuardRailHolder", matrix, false);
        for (const Vector3& position : columnSegments)
            AddObject("RoadColumnSegment", Matrix::CreateTranslation(position),
                      false);
        GenerateObjectsForTrack(track);
    }

    LandscapeObjectRenderer::~LandscapeObjectRenderer() = default;

    int LandscapeObjectRenderer::Draw(
        const Matrix& view, const Matrix& projection,
        const float totalTimeSeconds)
    {
        const Matrix inverseView = Matrix::Invert(view);
        const Vector3 cameraPosition = inverseView.getTranslationProperty();
        const Vector3 cameraRotation = Vector3::TransformNormal(
            Vector3::UnitZ, inverseView);
        for (const LandscapeObject& object : objects)
            object.model->Render(object.matrix, cameraPosition, cameraRotation,
                                 totalTimeSeconds);
        return meshManager.Render(
            device, *normalEffect, view, projection,
            Vector3::Normalize(Vector3(8500.0f, -7250.0f, 15000.0f)),
            reflectionTexture.get());
    }

    int LandscapeObjectRenderer::GenerateShadows(
        Effect& effect, const Matrix& lightViewProjection,
        const Vector3 shadowLightPosition, const float shadowDistance,
        const float totalTimeSeconds)
    {
        int submissions = 0;
        for (const LandscapeObject& object : objects)
        {
            if (!object.isNearTrack) continue;
            submissions += object.model->GenerateShadow(
                device, object.matrix, effect, lightViewProjection,
                shadowLightPosition, shadowDistance, totalTimeSeconds);
        }
        return submissions;
    }

    int LandscapeObjectRenderer::UseShadows(
        Effect& effect, const Matrix& viewProjection,
        const Matrix& lightViewProjection, const Matrix& textureScaleBias,
        const Vector3 shadowLightPosition, const float shadowDistance,
        const float totalTimeSeconds)
    {
        int submissions = 0;
        for (const LandscapeObject& object : objects)
        {
            if (!object.isNearTrack || object.isBanner) continue;
            submissions += object.model->UseShadow(
                device, object.matrix, effect, viewProjection,
                lightViewProjection, textureScaleBias, shadowLightPosition,
                shadowDistance, totalTimeSeconds);
        }
        return submissions;
    }

    int LandscapeObjectRenderer::getLoadedModelCountProperty() const
    {
        return static_cast<int>(models.size());
    }

    int LandscapeObjectRenderer::getObjectCountProperty() const
    {
        return static_cast<int>(objects.size());
    }

    int LandscapeObjectRenderer::getNearTrackObjectCountProperty() const
    {
        return static_cast<int>(std::ranges::count_if(
            objects, [](const LandscapeObject& object)
            {
                return object.isNearTrack;
            }));
    }

    int LandscapeObjectRenderer::getRegisteredMeshCountProperty() const
    {
        return meshManager.getRegisteredMeshCountProperty();
    }

    void LandscapeObjectRenderer::ReplaceStartLightObject(int number)
    {
        if (number < 0 || number >= 3) number = 0;
        if (!startLightObjectIndex) return;
        const std::array<const char*, 3> names = {
            "StartLight", "StartLight2", "StartLight3"};
        objects[*startLightObjectIndex].model =
            modelByName.at(names[static_cast<std::size_t>(number)]);
    }

    void LandscapeObjectRenderer::AddObject(
        const std::string& requestedName, Matrix matrix, bool isNearTrack)
    {
        const std::string modelName = FixModelName(requestedName);
        const std::string lowerName = ToLower(modelName);
        if (lowerName == "windmill" || lowerName.contains("hotel") ||
            lowerName.contains("building") || lowerName.contains("casino01"))
            isNearTrack = true;

        for (const Tracks::TrackCombiModels& combination : combinations)
        {
            if (combination.getNameProperty() != modelName) continue;
            for (const auto& object : combination.getObjectsProperty())
                AddObject(object.modelName, object.matrix * matrix, false);
            return;
        }

        const auto found = modelByName.find(modelName);
        if (found == modelByName.end())
        {
            if (!modelName.contains("Track"))
                throw std::runtime_error(
                    "Unsupported original Racing landscape model: " +
                    modelName);
            return;
        }
        Graphics::Model* model = found->second;
        Vector3 position = matrix.getTranslationProperty();
        const float landscapeHeight = landscape.GetMapHeight(position.X,
                                                               position.Y);
        if (position.Z < landscapeHeight)
        {
            position.Z = landscapeHeight;
            matrix.setTranslationProperty(position);
        }

        if (!modelName.starts_with("Banner") &&
            !modelName.starts_with("Sign") &&
            !modelName.starts_with("StartLight"))
        {
            for (const LandscapeObject& object : objects)
            {
                if (Vector3::DistanceSquared(
                        object.matrix.getTranslationProperty(), position) <
                    model->getSizeProperty() * model->getSizeProperty() / 4.0f)
                    return;
            }
        }

        objects.push_back({
            model, Matrix::CreateScale(1.2f) * matrix, isNearTrack,
            lowerName.contains("banner") || lowerName.contains("sign")});
        if (modelName.starts_with("StartLight"))
            startLightObjectIndex = objects.size() - 1;
    }

    void LandscapeObjectRenderer::AddObject(
        const std::string& modelName, const float rotation,
        const Vector3 trackPosition, const Vector3 trackRight, float distance)
    {
        const float objectSize = GetObjectSize(modelName);
        if (distance > 0.0f && distance - 10.0f < objectSize)
            distance += objectSize;
        if (distance < 0.0f && distance + 10.0f > -objectSize)
            distance -= objectSize;
        AddObject(
            modelName,
            Matrix::CreateRotationZ(rotation) *
                Matrix::CreateTranslation(
                    trackPosition + trackRight * distance +
                    Vector3(0.0f, 0.0f, -100.0f)),
            false);
    }

    float LandscapeObjectRenderer::GetObjectSize(
        const std::string& requestedName) const
    {
        const std::string modelName = FixModelName(requestedName);
        for (const Tracks::TrackCombiModels& combination : combinations)
        {
            if (combination.getNameProperty() == modelName)
                return combination.getSizeProperty();
        }
        const auto found = modelByName.find(modelName);
        return found == modelByName.end() ? 1.0f
                                          : found->second->getSizeProperty();
    }

    void LandscapeObjectRenderer::GenerateObjectsForTrack(
        const Tracks::Track& track)
    {
        using Helpers::RandomHelper;
        using Helpers::Vector3Helper;

        const auto& points = track.getPointsProperty();
        const auto& helpers = track.getHelperPositionsProperty();
        const int pointCount = points.getCountProperty();

        float lastGap = 0.0f;
        int generatedNum = 0;
        for (int num = 0; num < pointCount; ++num)
        {
            bool palms = false;
            bool laterns = false;
            for (const Tracks::TrackLine::RoadHelperPosition& helper : helpers)
            {
                if (num < helper.startNum || num > helper.endNum) continue;
                if (helper.type ==
                    Tracks::TrackData::RoadHelper::HelperType::Palms)
                    palms = true;
                else if (helper.type ==
                         Tracks::TrackData::RoadHelper::HelperType::Laterns)
                    laterns = true;
            }
            if (!palms && !laterns) continue;

            const Tracks::TrackVertex& point = points.getItem(num);
            const float distance = Vector3::Distance(
                points.getItem((num + 1) % pointCount).pos, point.pos);
            if (lastGap - distance <= 0.0f)
            {
                const bool upsideDown = point.up.Z < 0.05f;
                const bool movingUp = point.dir.Z > 0.65f;
                const bool movingDown = point.dir.Z < -0.65f;
                if (upsideDown || movingUp || movingDown) continue;

                Matrix pointSpace = Matrix::getIdentityProperty();
                pointSpace.setRightProperty(point.right);
                pointSpace.setUpProperty(point.dir);
                pointSpace.setForwardProperty(-point.up);
                const Vector3 objectPoint = Vector3::CatmullRom(
                    points.getItem(num == 0 ? pointCount - 1 : num - 1).pos,
                    point.pos, points.getItem((num + 1) % pointCount).pos,
                    points.getItem((num + 2) % pointCount).pos,
                    lastGap / distance);
                ++generatedNum;

                if (palms)
                {
                    if (objectPoint.Z - landscape.GetMapHeight(
                                            objectPoint.X, objectPoint.Y) <
                        11.0f)
                    {
                        int randomNum = RandomHelper::GetRandomInt(4);
                        if (randomNum == 3)
                            randomNum = RandomHelper::GetRandomInt(4);
                        const std::array<const char*, 4> palmNames = {
                            "AlphaPalm", "AlphaPalm2", "AlphaPalm3",
                            "AlphaPalmSmall"};
                        AddObject(
                            palmNames[static_cast<std::size_t>(randomNum)],
                            Matrix::CreateScale(1.25f) *
                                Matrix::CreateRotationZ(
                                    RandomHelper::GetRandomFloat(
                                        0.0f, MathHelper::Pi * 2.0f)) *
                                Matrix::CreateTranslation(
                                    point.right *
                                    (generatedNum % 2 == 0 ? 0.6f : -0.6f) *
                                    point.roadWidth *
                                    Tracks::TrackVertex::RoadWidthScale) *
                                Matrix::CreateTranslation(
                                    Vector3(0.0f, 0.0f, -50.0f)) *
                                Matrix::CreateTranslation(objectPoint),
                            true);
                    }
                }
                else
                {
                    AddObject(
                        "Laterne",
                        Matrix::CreateRotationZ(
                            generatedNum % 2 == 0 ? MathHelper::Pi : 0.0f) *
                            Matrix::CreateTranslation(
                                (generatedNum % 2 == 0 ? 0.5f : -0.5f) *
                                        point.roadWidth *
                                        Tracks::TrackVertex::RoadWidthScale -
                                    0.35f,
                                0.0f, -0.2f) *
                            pointSpace * Matrix::CreateTranslation(objectPoint),
                        true);
                }
                lastGap += PalmAndLaternGap;
            }
            lastGap -= distance;
        }

        const Tracks::TrackVertex& start = points.getItem(0);
        Matrix startPointSpace = Matrix::getIdentityProperty();
        startPointSpace.setRightProperty(start.right);
        startPointSpace.setUpProperty(start.dir);
        startPointSpace.setForwardProperty(-start.up);
        AddObject(
            "Banner6",
            Matrix::CreateScale(start.roadWidth) * Matrix::CreateScale(1.051f) *
                Matrix::CreateTranslation(0.0f, -5.1f, 0.0f) *
                startPointSpace * Matrix::CreateTranslation(start.pos),
            true);
        AddObject(
            "StartLight3",
            Matrix::CreateScale(1.1f) *
                Matrix::CreateTranslation(
                    start.roadWidth * Tracks::TrackVertex::RoadWidthScale *
                            0.5f -
                        0.3f,
                    6.0f, -0.2f) *
                startPointSpace * Matrix::CreateTranslation(start.pos),
            true);

        lastGap = CheckpointGap;
        float signGap = SignGap;
        for (int num = 0; num < pointCount - 24; ++num)
        {
            const Tracks::TrackVertex& point = points.getItem(num);
            const float distance = Vector3::Distance(
                points.getItem((num + 1) % pointCount).pos, point.pos);
            const bool upsideDown = point.up.Z < 0.05f;
            const bool movingUp = point.dir.Z > 0.65f;
            const bool movingDown = point.dir.Z < -0.65f;
            if (upsideDown || movingUp || movingDown) continue;

            Matrix pointSpace = Matrix::getIdentityProperty();
            pointSpace.setRightProperty(point.right);
            pointSpace.setUpProperty(point.dir);
            pointSpace.setForwardProperty(-point.up);
            const Vector3 p1 =
                points.getItem(num == 0 ? pointCount - 1 : num - 1).pos;
            const Vector3 p2 = point.pos;
            const Vector3 p3 = points.getItem((num + 1) % pointCount).pos;
            const Vector3 p4 = points.getItem((num + 2) % pointCount).pos;

            if (lastGap - distance <= 0.0f)
            {
                const Vector3 objectPoint = Vector3::CatmullRom(
                    p1, p2, p3, p4, lastGap / distance);
                const int randomNum = RandomHelper::GetRandomInt(6);
                const std::array<const char*, 6> bannerNames = {
                    "Banner", "Banner2", "Banner3", "Banner4", "Banner5",
                    "Banner6"};
                AddObject(
                    bannerNames[static_cast<std::size_t>(randomNum)],
                    Matrix::CreateScale(point.roadWidth) *
                        Matrix::CreateTranslation(0.0f, 0.0f, -0.1f) *
                        pointSpace * Matrix::CreateTranslation(objectPoint),
                    true);
                lastGap += CheckpointGap;
            }
            else if (signGap - distance <= 0.0f && num >= 25)
            {
                const Vector3 objectPoint = Vector3::CatmullRom(
                    p1, p2, p3, p4, signGap / distance);
                const Vector3 backPosition = points.getItem(num - 25).pos;
                const bool loopingAhead =
                    points.getItem((num + 60) % pointCount).up.Z < 0.15f;
                const Vector3 angleVector =
                    Vector3::Normalize(backPosition - point.pos);
                float roadAngle = Vector3Helper::GetAngleBetweenVectors(
                    angleVector, Vector3::Normalize(-point.dir));
                if (Vector3::Distance(point.right, angleVector) <
                    Vector3::Distance(-point.right, angleVector))
                    roadAngle = -roadAngle;

                if (loopingAhead)
                {
                    AddObject(
                        "SignWarning",
                        Matrix::CreateTranslation(
                            point.roadWidth *
                                    Tracks::TrackVertex::RoadWidthScale * 0.5f -
                                0.1f,
                            0.0f, -0.25f) *
                            pointSpace * Matrix::CreateTranslation(objectPoint),
                        true);
                }
                else if (roadAngle < -MathHelper::Pi / 7.5f)
                {
                    AddObject(
                        "SignCurveRight",
                        Matrix::CreateRotationZ(MathHelper::Pi / 2.0f) *
                            Matrix::CreateTranslation(
                                -point.roadWidth *
                                        Tracks::TrackVertex::RoadWidthScale *
                                        0.5f -
                                    0.15f,
                                0.0f, -0.25f) *
                            pointSpace * Matrix::CreateTranslation(objectPoint),
                        true);
                }
                else if (roadAngle > MathHelper::Pi / 7.5f)
                {
                    AddObject(
                        "SignCurveLeft",
                        Matrix::CreateRotationZ(-MathHelper::Pi / 2.0f) *
                            Matrix::CreateTranslation(
                                point.roadWidth *
                                        Tracks::TrackVertex::RoadWidthScale *
                                        0.5f -
                                    0.15f,
                                0.0f, -0.25f) *
                            pointSpace * Matrix::CreateTranslation(objectPoint),
                        true);
                }
                else if (roadAngle < -MathHelper::Pi / 10.0f ||
                         roadAngle > MathHelper::Pi / 10.0f ||
                         RandomHelper::GetRandomInt(9) == 4)
                {
                    int randomValue = RandomHelper::GetRandomInt(3);
                    if (randomValue == 0 &&
                        std::abs(roadAngle) < MathHelper::Pi / 24.0f)
                        randomValue = RandomHelper::GetRandomInt(3);
                    else if (std::abs(roadAngle) < MathHelper::Pi / 20.0f &&
                             RandomHelper::GetRandomInt(2) == 1)
                        roadAngle *= -1.0f;
                    const std::string signName =
                        randomValue == 0
                            ? (roadAngle > 0.0f ? "SignCurveLeft"
                                              : "SignCurveRight")
                            : (randomValue == 1 ? "Sign" : "Sign2");
                    AddObject(
                        signName,
                        Matrix::CreateRotationZ(
                            (roadAngle > 0.0f ? -1.0f : 1.0f) *
                            MathHelper::Pi / 2.0f) *
                            Matrix::CreateTranslation(
                                (roadAngle > 0.0f ? 1.0f : -1.0f) *
                                        point.roadWidth *
                                        Tracks::TrackVertex::RoadWidthScale *
                                        0.5f -
                                    (randomValue == 0 ? 0.15f : 0.005f),
                                0.0f, -0.25f) *
                            pointSpace * Matrix::CreateTranslation(objectPoint),
                        true);
                }
                signGap += SignGap;
            }
            lastGap -= distance;
            signGap -= distance;
        }

        for (int num = 0; num < pointCount; num += 2)
        {
            const Tracks::TrackVertex& point = points.getItem(num);
            const float landscapeHeight =
                landscape.GetMapHeight(point.pos.X, point.pos.Y);
            if (point.pos.Z - landscapeHeight > 60.0f) continue;
            const bool upsideDown = point.up.Z < 0.05f;
            const bool movingUp = point.dir.Z > 0.65f;
            const bool movingDown = point.dir.Z < -0.65f;
            if (upsideDown || movingUp || movingDown) continue;

            constexpr int randomMaxProbability = 5;
            if (RandomHelper::GetRandomInt(randomMaxProbability) != 0) continue;
            int randomObject = RandomHelper::GetRandomInt(
                static_cast<int>(AutoGenerationNames.size()));
            if (randomObject >= 6)
                randomObject = RandomHelper::GetRandomInt(
                    static_cast<int>(AutoGenerationNames.size()));
            if (randomObject ==
                    static_cast<int>(AutoGenerationNames.size()) - 1 &&
                RandomHelper::GetRandomInt(3) < 2)
                randomObject = RandomHelper::GetRandomInt(
                    static_cast<int>(AutoGenerationNames.size()));
            float objectDistance = RandomHelper::GetRandomFloat(26.0f, 88.0f);
            if (randomObject ==
                static_cast<int>(AutoGenerationNames.size()) - 1)
                objectDistance += 20.0f;
            const bool side = RandomHelper::GetRandomInt(2) == 0;
            const float rotation = RandomHelper::GetRandomFloat(
                0.0f, MathHelper::Pi * 2.0f);
            AddObject(AutoGenerationNames[static_cast<std::size_t>(randomObject)],
                      rotation, point.pos, point.right,
                      objectDistance * (side ? 1.0f : -1.0f));
        }
    }

    std::string LandscapeObjectRenderer::FixModelName(std::string modelName)
    {
        if (modelName == "OilWell") return "OilPump";
        if (modelName == "PalmSmall" || modelName == "AlphaPalm4")
            return "AlphaPalmSmall";
        if (modelName == "Palm") return "AlphaPalm";
        if (modelName == "Casino") return "Casino01";
        if (modelName == "Combi") return "CombiPalms";
        return modelName;
    }

    std::string LandscapeObjectRenderer::ToLower(std::string value)
    {
        std::ranges::transform(value, value.begin(), [](const unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }
}
