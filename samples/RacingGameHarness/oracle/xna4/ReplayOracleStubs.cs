// SPDX-License-Identifier: MS-PL

using System.IO;
using System.Threading;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Storage;

namespace RacingGame.Helpers
{
    internal static class Directories
    {
        internal static string ContentDirectory =
            "__racing_replay_oracle_no_title_content__";
    }

    internal static class FileHelper
    {
        internal static readonly ManualResetEvent StorageContainerMRE =
            new ManualResetEvent(true);

        internal static StorageDevice XnaUserDevice
        {
            get { return null; }
        }

        internal static Matrix ReadMatrix(BinaryReader reader)
        {
            return new Matrix(
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle(),
                reader.ReadSingle(), reader.ReadSingle());
        }

        internal static void WriteMatrix(BinaryWriter writer, Matrix matrix)
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
}

namespace RacingGame.GameScreens
{
    internal static class Highscores
    {
        internal static float GetTopLapTime(int level)
        {
            return 75.0f * (level + 1);
        }
    }
}

namespace RacingGame.Tracks
{
    public sealed class Track
    {
        private readonly RacingTrackOracle.TrackLineProbe track;

        internal Track(RacingTrackOracle.TrackLineProbe setTrack)
        {
            track = setTrack;
            CheckpointSegmentPositions =
                new System.Collections.Generic.List<int>(
                    track.GetCheckpointSegmentPositions());
        }

        public int NumberOfSegments
        {
            get { return track.Points.Count; }
        }

        public System.Collections.Generic.List<int> CheckpointSegmentPositions
        {
            get;
            private set;
        }

        public Matrix GetTrackPositionMatrix(
            float position, out float roadWidth, out float nextRoadWidth)
        {
            return track.GetTrackPositionMatrix(
                position, out roadWidth, out nextRoadWidth);
        }
    }
}
