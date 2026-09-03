// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/ICloneable.hpp"

namespace RacingGame::Tracks
{
    class Track;
}

namespace System::IO
{
    class BinaryReader;
    class BinaryWriter;
}

namespace RacingGame::GameLogic
{
    /** @brief Stores one recorded or generated lap for ghost-car playback. */
    class Replay final : public System::ICloneable
    {
    public:
        /** @brief Time in seconds between recorded car matrices. */
        static constexpr float TrackMatrixIntervals = 0.2f;

        /**
         * @brief Creates, loads or generates a replay for one track.
         * @param setTrackNum Zero-based track number.
         * @param createNew True to create an empty recording.
         * @param track Active generated track used by the default replay.
         * @param topLapTime Current top highscore time in seconds.
         * @param contentDirectory Title-content directory containing optional replay files.
         */
        Replay(int setTrackNum, bool createNew, const Tracks::Track& track,
               float topLapTime, std::string contentDirectory = "Content");

        /**
         * @brief Creates a deep copy of this replay.
         * @return Independently owned replay clone.
         */
        [[nodiscard]] std::shared_ptr<System::ICloneable> Clone() const override;

        /** @brief Gets the zero-based track number. */
        [[nodiscard]] int getTrackNumberProperty() const;
        /** @brief Gets the replay lap time in seconds. */
        [[nodiscard]] float getLapTimeProperty() const;
        /** @brief Sets the replay lap time in seconds. */
        void setLapTimeProperty(float value);
        /** @brief Gets the number of recorded car matrices. */
        [[nodiscard]] int getNumberOfTrackMatricesProperty() const;
        /** @brief Gets mutable checkpoint times in seconds. */
        [[nodiscard]] std::vector<float>& getCheckpointTimesProperty();
        /** @brief Gets checkpoint times in seconds. */
        [[nodiscard]] const std::vector<float>&
        getCheckpointTimesProperty() const;

        /**
         * @brief Gets the interpolated ghost-car matrix at a lap time.
         * @param trackTime Time since lap start in seconds.
         * @return Recorded/interpolated matrix, or identity if fewer than two exist.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        GetCarMatrixAtTime(float trackTime) const;

        /** @brief Saves the replay to the RacingGame storage container. */
        void Save() const;

        /**
         * @brief Appends a recorded car matrix.
         * @param matrix Car coordinate matrix at the next 0.2-second interval.
         */
        void AddCarMatrix(Microsoft::Xna::Framework::Matrix matrix);

    private:
        static std::mutex storageMutex;

        int trackNum = 0;
        float lapTime = 0.0f;
        std::vector<Microsoft::Xna::Framework::Matrix> trackMatrixValues;
        std::vector<float> checkpointTimes;
        std::string contentDirectory;

        [[nodiscard]] static const std::string& ReplayFilename(int track);
        void LoadFromStorage();
        void LoadFromTitle();
        void GenerateDefault(const Tracks::Track& track, float topLapTime);
        void Read(System::IO::BinaryReader& reader);
        void Write(System::IO::BinaryWriter& writer) const;
        [[nodiscard]] static Microsoft::Xna::Framework::Matrix
        ReadMatrix(System::IO::BinaryReader& reader);
        static void WriteMatrix(
            System::IO::BinaryWriter& writer,
            const Microsoft::Xna::Framework::Matrix& matrix);
    };
}
