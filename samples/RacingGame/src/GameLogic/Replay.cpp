// SPDX-License-Identifier: MS-PL

#include "GameLogic/Replay.hpp"

#include <array>
#include <filesystem>
#include <stdexcept>
#include <utility>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "Microsoft/Xna/Framework/TitleContainer.hpp"
#include "System/IO/BinaryReader.hpp"
#include "System/IO/BinaryWriter.hpp"
#include "System/IO/FileAccess.hpp"
#include "System/IO/FileMode.hpp"
#include "System/IO/FileShare.hpp"
#include "Tracks/Track.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::TitleContainer;
    using namespace Microsoft::Xna::Framework::Storage;
    using namespace System::IO;

    std::mutex Replay::storageMutex;

    Replay::Replay(
        const int setTrackNum, const bool createNew,
        const Tracks::Track& track, const float topLapTime,
        std::string setContentDirectory)
        : trackNum(setTrackNum),
          contentDirectory(std::move(setContentDirectory))
    {
        (void)ReplayFilename(trackNum);
        if (createNew)
            return;

        // The original sample never sets replayFileFound after either successful
        // read. Preserve that observable quirk: title/default data is appended too.
        constexpr bool replayFileFound = false;
        LoadFromStorage();
        if (!replayFileFound)
            LoadFromTitle();
        if (!replayFileFound)
            GenerateDefault(track, topLapTime);
    }

    std::shared_ptr<System::ICloneable> Replay::Clone() const
    {
        return std::make_shared<Replay>(*this);
    }

    int Replay::getTrackNumberProperty() const
    {
        return trackNum;
    }

    float Replay::getLapTimeProperty() const
    {
        return lapTime;
    }

    void Replay::setLapTimeProperty(const float value)
    {
        lapTime = value;
    }

    int Replay::getNumberOfTrackMatricesProperty() const
    {
        return static_cast<int>(trackMatrixValues.size());
    }

    std::vector<float>& Replay::getCheckpointTimesProperty()
    {
        return checkpointTimes;
    }

    const std::vector<float>& Replay::getCheckpointTimesProperty() const
    {
        return checkpointTimes;
    }

    Matrix Replay::GetCarMatrixAtTime(const float trackTime) const
    {
        if (trackMatrixValues.size() < 2)
            return Matrix::getIdentityProperty();
        if (trackTime <= 0.0f)
            return trackMatrixValues.front();

        int interval = static_cast<int>(trackTime / TrackMatrixIntervals);
        const float percent =
            (trackTime - interval * TrackMatrixIntervals) /
            TrackMatrixIntervals;
        if (interval < 0)
            interval = 0;
        if (interval > static_cast<int>(trackMatrixValues.size()) - 2)
            return trackMatrixValues.front();
        return Matrix::Lerp(
            trackMatrixValues[static_cast<std::size_t>(interval)],
            trackMatrixValues[static_cast<std::size_t>(interval + 1)],
            percent);
    }

    void Replay::Save() const
    {
        const std::scoped_lock lock(storageMutex);
        try
        {
            auto selector = StorageDevice::BeginShowSelector(
                PlayerIndex::One, nullptr, nullptr);
            auto device = StorageDevice::EndShowSelector(selector.get());
            if (!device || !device->getIsConnectedProperty())
                return;
            auto open = device->BeginOpenContainer(
                "RacingGame", nullptr, nullptr);
            auto container = device->EndOpenContainer(open.get());
            auto stream = container->CreateFile(ReplayFilename(trackNum));
            BinaryWriter writer(stream.get(), true);
            Write(writer);
            writer.Close();
            stream->Close();
            container->Dispose();
        }
        catch (...)
        {
            // The original game treats replay persistence as best effort.
        }
    }

    void Replay::AddCarMatrix(const Matrix matrix)
    {
        trackMatrixValues.push_back(matrix);
    }

    const std::string& Replay::ReplayFilename(const int track)
    {
        static const std::array<std::string, 3> filenames{
            "TrackBeginner.Replay",
            "TrackAdvanced.Replay",
            "TrackExpert.Replay",
        };
        return filenames.at(static_cast<std::size_t>(track));
    }

    void Replay::LoadFromStorage()
    {
        const std::scoped_lock lock(storageMutex);
        try
        {
            auto selector = StorageDevice::BeginShowSelector(
                PlayerIndex::One, nullptr, nullptr);
            auto device = StorageDevice::EndShowSelector(selector.get());
            if (!device || !device->getIsConnectedProperty())
                return;
            auto open = device->BeginOpenContainer(
                "RacingGame", nullptr, nullptr);
            auto container = device->EndOpenContainer(open.get());
            if (container->FileExists(ReplayFilename(trackNum)))
            {
                auto stream = container->OpenFile(
                    ReplayFilename(trackNum), FileMode::Open,
                    FileAccess::Read, FileShare::ReadWrite);
                BinaryReader reader(stream.get(), true);
                Read(reader);
                reader.Close();
                stream->Close();
            }
            container->Dispose();
        }
        catch (...)
        {
            // The original game ignores unavailable or malformed saved replays.
        }
    }

    void Replay::LoadFromTitle()
    {
        const std::filesystem::path path =
            std::filesystem::path(contentDirectory) /
            ReplayFilename(trackNum);
        if (!std::filesystem::is_regular_file(path))
            return;
        auto stream = TitleContainer::OpenStream(path.string());
        BinaryReader reader(stream.get(), true);
        Read(reader);
        reader.Close();
        stream->Close();
    }

    void Replay::GenerateDefault(
        const Tracks::Track& track, const float topLapTime)
    {
        lapTime = topLapTime;
        const int matrixCount =
            1 + static_cast<int>(lapTime / TrackMatrixIntervals);
        float lastTrackPosition = 0.0f;
        int oldTrackSegment = 0;
        const auto& checkpoints =
            track.getCheckpointSegmentPositionsProperty();

        for (int number = 0; number < matrixCount * 2; ++number)
        {
            float carTrackPosition = 0.00001f +
                static_cast<float>(number) /
                    static_cast<float>(matrixCount - 1);
            const float difference =
                carTrackPosition - lastTrackPosition;
            carTrackPosition =
                lastTrackPosition + difference * 0.1f;
            lastTrackPosition = carTrackPosition;

            float roadWidth = 0.0f;
            float nextRoadWidth = 0.0f;
            trackMatrixValues.push_back(track.GetTrackPositionMatrix(
                carTrackPosition, roadWidth, nextRoadWidth));

            const int trackSegment = static_cast<int>(
                carTrackPosition * track.getNumberOfSegmentsProperty());
            if (trackSegment != oldTrackSegment)
            {
                for (std::size_t checkpoint = 0;
                     checkpoint < checkpoints.size(); ++checkpoint)
                {
                    if (checkpoints[checkpoint] > oldTrackSegment &&
                        checkpoints[checkpoint] <= trackSegment)
                    {
                        checkpointTimes.push_back(
                            lapTime * static_cast<float>(number) /
                            static_cast<float>(matrixCount - 1));
                        break;
                    }
                }
            }
            oldTrackSegment = trackSegment;
            if (carTrackPosition >= 1.0f)
                break;
        }
        checkpointTimes.push_back(lapTime);
    }

    void Replay::Read(BinaryReader& reader)
    {
        lapTime = reader.ReadSingle();
        const int matrixCount = reader.ReadInt32();
        for (int number = 0; number < matrixCount; ++number)
            trackMatrixValues.push_back(ReadMatrix(reader));
        const int checkpointCount = reader.ReadInt32();
        for (int number = 0; number < checkpointCount; ++number)
            checkpointTimes.push_back(reader.ReadSingle());
    }

    void Replay::Write(BinaryWriter& writer) const
    {
        writer.Write(lapTime);
        writer.Write(static_cast<int>(trackMatrixValues.size()));
        for (const Matrix& matrix : trackMatrixValues)
            WriteMatrix(writer, matrix);
        writer.Write(static_cast<int>(checkpointTimes.size()));
        for (const float checkpoint : checkpointTimes)
            writer.Write(checkpoint);
    }

    Matrix Replay::ReadMatrix(BinaryReader& reader)
    {
        return {
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
            reader.ReadSingle(), reader.ReadSingle(),
        };
    }

    void Replay::WriteMatrix(
        BinaryWriter& writer, const Matrix& matrix)
    {
        writer.Write(matrix.M11); writer.Write(matrix.M12);
        writer.Write(matrix.M13); writer.Write(matrix.M14);
        writer.Write(matrix.M21); writer.Write(matrix.M22);
        writer.Write(matrix.M23); writer.Write(matrix.M24);
        writer.Write(matrix.M31); writer.Write(matrix.M32);
        writer.Write(matrix.M33); writer.Write(matrix.M34);
        writer.Write(matrix.M41); writer.Write(matrix.M42);
        writer.Write(matrix.M43); writer.Write(matrix.M44);
    }
}
