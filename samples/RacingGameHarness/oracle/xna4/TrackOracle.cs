// SPDX-License-Identifier: MS-PL

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using Microsoft.Xna.Framework;
using RacingGame.Graphics;
using RacingGame.Tracks;

namespace RacingGame.Helpers
{
}

namespace RacingGame.Landscapes
{
    public sealed class Landscape
    {
        private const int GridWidth = 257;
        private const int GridHeight = 257;
        private const float MapWidthFactor = 10.0f;
        private const float MapHeightFactor = 10.0f;
        private const float MapZScale = 300.0f;
        private readonly float[,] mapHeights = new float[GridWidth, GridHeight];

        public readonly TangentVertex[] Vertices =
            new TangentVertex[GridWidth * GridHeight];
        public readonly uint[] Indices =
            new uint[(GridWidth - 1) * (GridHeight - 1) * 6];

        public readonly List<Tuple<string, Matrix, bool>> Objects =
            new List<Tuple<string, Matrix, bool>>();

        public Landscape(string heightPath)
        {
            byte[] heights = File.ReadAllBytes(heightPath);
            if (heights.Length != GridWidth * GridHeight)
                throw new InvalidDataException("LandscapeHeights.data must contain 257*257 bytes");

            for (int x = 0; x < GridWidth; ++x)
                for (int y = 0; y < GridHeight; ++y)
                    mapHeights[x, y] = heights[x + y * GridWidth] / 255.0f * MapZScale;

            for (int x = 0; x < GridWidth; ++x)
                for (int y = 0; y < GridHeight; ++y)
                {
                    int index = x + y * GridWidth;
                    Vector3 position = CalcLandscapePos(x, y, heights);
                    Vertices[index].pos = position;
                    Vector3 edge1 = position - CalcLandscapePos(x, y + 1, heights);
                    Vector3 edge2 = position - CalcLandscapePos(x + 1, y, heights);
                    Vector3 edge3 = position - CalcLandscapePos(x - 1, y + 1, heights);
                    Vector3 edge4 = position - CalcLandscapePos(x + 1, y + 1, heights);
                    Vector3 edge5 = position - CalcLandscapePos(x - 1, y - 1, heights);
                    Vertices[index].normal = Vector3.Normalize(
                        Vector3.Cross(edge2, edge1) +
                        Vector3.Cross(edge4, edge3) +
                        Vector3.Cross(edge3, edge5));
                    Vertices[index].tangent = Vector3.Normalize(edge1);
                    Vertices[index].uv = new Vector2(
                        y / (float)(GridHeight - 1),
                        x / (float)(GridWidth - 1));
                }

            var normalsForSmoothing = new Vector3[GridWidth, GridHeight];
            for (int x = 0; x < GridWidth; ++x)
                for (int y = 0; y < GridHeight; ++y)
                    normalsForSmoothing[x, y] = Vertices[x + y * GridWidth].normal;

            for (int x = 1; x < GridWidth - 1; ++x)
                for (int y = 1; y < GridHeight - 1; ++y)
                {
                    int index = x + y * GridWidth;
                    Vector3 normal = Vertices[index].normal * 4.0f;
                    for (int xAdd = -1; xAdd <= 1; ++xAdd)
                        for (int yAdd = -1; yAdd <= 1; ++yAdd)
                            normal += normalsForSmoothing[x + xAdd, y + yAdd];
                    Vertices[index].normal = Vector3.Normalize(normal);
                    Vector3 helper = Vector3.Cross(
                        Vertices[index].normal, Vertices[index].tangent);
                    Vertices[index].tangent = Vector3.Cross(
                        helper, Vertices[index].normal);
                }

            int current = 0;
            for (int x = 0; x < GridWidth - 1; ++x)
                for (int y = 0; y < GridHeight - 1; ++y)
                {
                    Indices[current + 0] = (uint)(x * GridHeight + y);
                    Indices[current + 2] =
                        (uint)((x + 1) * GridHeight + (y + 1));
                    Indices[current + 1] = (uint)((x + 1) * GridHeight + y);
                    Indices[current + 3] =
                        (uint)((x + 1) * GridHeight + (y + 1));
                    Indices[current + 5] = (uint)(x * GridHeight + y);
                    Indices[current + 4] = (uint)(x * GridHeight + (y + 1));
                    current += 6;
                }
        }

        public void KillAllLoadedObjects()
        {
            Objects.Clear();
        }

        public void AddObjectToRender(string modelName, Matrix matrix, bool isNearTrack)
        {
            Objects.Add(Tuple.Create(modelName, matrix, isNearTrack));
        }

        private static int ModulateValueInRange(float value, int maximum)
        {
            return value < 0.0f
                ? (maximum - 1) - ((int)(-value) % maximum)
                : (int)value % maximum;
        }

        public float GetMapHeight(float x, float y)
        {
            x /= MapWidthFactor;
            y /= MapHeightFactor;
            int ix = ModulateValueInRange(x, GridWidth - 1);
            int iy = ModulateValueInRange(y, GridHeight - 1);
            float fx = x - (int)x;
            float fy = y - (int)y;
            int ix2 = (ix + 1) % (GridWidth - 1);
            int iy2 = (iy + 1) % (GridHeight - 1);

            if (fx + fy < 1.0f)
                return mapHeights[ix, iy] +
                    fx * (mapHeights[ix2, iy] - mapHeights[ix, iy]) +
                    fy * (mapHeights[ix, iy2] - mapHeights[ix, iy]);

            return mapHeights[ix2, iy2] +
                (1.0f - fy) * (mapHeights[ix2, iy] - mapHeights[ix2, iy2]) +
                (1.0f - fx) * (mapHeights[ix, iy2] - mapHeights[ix2, iy2]);
        }

        private static Vector3 CalcLandscapePos(int x, int y, byte[] heights)
        {
            int mapX = x < 0 ? 0 : x >= GridWidth ? GridWidth - 1 : x;
            int mapY = y < 0 ? 0 : y >= GridHeight ? GridHeight - 1 : y;
            float heightPercent = heights[mapX + mapY * GridWidth] / 255.0f;
            return new Vector3(
                x * MapWidthFactor, y * MapHeightFactor,
                heightPercent * MapZScale);
        }
    }
}

namespace RacingTrackOracle
{
    using RacingGame.Landscapes;

    internal sealed class TrackLineProbe : TrackLine
    {
        internal TrackLineProbe(TrackData data, Landscape landscape)
            : base(data, landscape)
        {
        }

        internal IList<TrackVertex> Points { get { return points; } }
        internal IList<RoadHelperPosition> Helpers { get { return helperPositions; } }
    }

    internal static class Program
    {
        private static ulong HashByte(ulong hash, byte value)
        {
            return (hash ^ value) * 1099511628211UL;
        }

        private static ulong HashInt32(ulong hash, int value)
        {
            unchecked
            {
                hash = HashByte(hash, (byte)value);
                hash = HashByte(hash, (byte)(value >> 8));
                hash = HashByte(hash, (byte)(value >> 16));
                return HashByte(hash, (byte)(value >> 24));
            }
        }

        private static ulong HashSingle(ulong hash, float value)
        {
            return HashInt32(hash, BitConverter.SingleToInt32Bits(value));
        }

        private static ulong HashString(ulong hash, string value)
        {
            foreach (byte part in Encoding.UTF8.GetBytes(value)) hash = HashByte(hash, part);
            return HashByte(hash, 0);
        }

        private static ulong HashVector3(ulong hash, Vector3 value)
        {
            hash = HashSingle(hash, value.X);
            hash = HashSingle(hash, value.Y);
            return HashSingle(hash, value.Z);
        }

        private static ulong HashMatrix(ulong hash, Matrix value)
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

        private static ulong HashTrack(TrackLineProbe track)
        {
            ulong hash = 14695981039346656037UL;
            foreach (TrackVertex point in track.Points)
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

        private static string TrackFieldHashes(TrackLineProbe track)
        {
            ulong position = 14695981039346656037UL;
            ulong direction = 14695981039346656037UL;
            ulong right = 14695981039346656037UL;
            ulong up = 14695981039346656037UL;
            ulong uvWidth = 14695981039346656037UL;
            foreach (TrackVertex point in track.Points)
            {
                position = HashVector3(position, point.pos);
                direction = HashVector3(direction, point.dir);
                right = HashVector3(right, point.right);
                up = HashVector3(up, point.up);
                uvWidth = HashSingle(uvWidth, point.uv.X);
                uvWidth = HashSingle(uvWidth, point.uv.Y);
                uvWidth = HashSingle(uvWidth, point.roadWidth);
            }
            return string.Format(CultureInfo.InvariantCulture,
                "position={0:x16} direction={1:x16} right={2:x16} up={3:x16} uvWidth={4:x16}",
                position, direction, right, up, uvWidth);
        }

        private static ulong HashObjects(IEnumerable<Tuple<string, Matrix, bool>> objects)
        {
            ulong hash = 14695981039346656037UL;
            foreach (Tuple<string, Matrix, bool> value in objects)
            {
                hash = HashString(hash, value.Item1);
                hash = HashMatrix(hash, value.Item2);
                hash = HashByte(hash, value.Item3 ? (byte)1 : (byte)0);
            }
            return hash;
        }

        private static ulong HashTangentVertices(IEnumerable<TangentVertex> vertices)
        {
            ulong hash = 14695981039346656037UL;
            foreach (TangentVertex vertex in vertices)
            {
                hash = HashVector3(hash, vertex.pos);
                hash = HashSingle(hash, vertex.uv.X);
                hash = HashSingle(hash, vertex.uv.Y);
                hash = HashVector3(hash, vertex.normal);
                hash = HashVector3(hash, vertex.tangent);
            }
            return hash;
        }

        private static ulong HashIndices(IEnumerable<int> indices)
        {
            ulong hash = 14695981039346656037UL;
            foreach (int index in indices) hash = HashInt32(hash, index);
            return hash;
        }

        private static void WriteLandscape(
            StreamWriter report, Landscape landscape)
        {
            ulong vertexHash = HashTangentVertices(landscape.Vertices);
            ulong indexHash = 14695981039346656037UL;
            foreach (uint index in landscape.Indices)
                indexHash = HashInt32(indexHash, unchecked((int)index));
            report.WriteLine(
                "LANDSCAPE vertices={0} indices={1} vertexHash={2:x16} indexHash={3:x16}",
                landscape.Vertices.Length, landscape.Indices.Length,
                vertexHash, indexHash);
        }

        private static void WriteRoadGeometry(
            StreamWriter report, string name, TrackLineProbe track)
        {
            int count = track.Points.Count;
            var roadVertices = new TangentVertex[count * 5];
            for (int num = 0; num < count; ++num)
            {
                roadVertices[num * 5 + 0] = track.Points[num].RightTangentVertex;
                roadVertices[num * 5 + 1] = track.Points[num].MiddleRightTangentVertex;
                roadVertices[num * 5 + 2] = track.Points[num].MiddleTangentVertex;
                roadVertices[num * 5 + 3] = track.Points[num].MiddleLeftTangentVertex;
                roadVertices[num * 5 + 4] = track.Points[num].LeftTangentVertex;
            }
            var roadIndices = new int[(count - 1) * 24];
            int vertexIndex = 0;
            for (int num = 0; num < count - 1; ++num)
            {
                for (int side = 0; side < 4; ++side)
                {
                    roadIndices[num * 24 + 6 * side + 0] = vertexIndex + side;
                    roadIndices[num * 24 + 6 * side + 1] = vertexIndex + 6 + side;
                    roadIndices[num * 24 + 6 * side + 2] = vertexIndex + 5 + side;
                    roadIndices[num * 24 + 6 * side + 3] = vertexIndex + 6 + side;
                    roadIndices[num * 24 + 6 * side + 4] = vertexIndex + side;
                    roadIndices[num * 24 + 6 * side + 5] = vertexIndex + 1 + side;
                }
                vertexIndex += 5;
            }

            var backVertices = new TangentVertex[count * 4];
            for (int num = 0; num < count; ++num)
            {
                backVertices[num * 4 + 0] = track.Points[num].LeftTangentVertex;
                backVertices[num * 4 + 0].uv = new Vector2(
                    backVertices[num * 4 + 0].U, 0.0f);
                backVertices[num * 4 + 1] =
                    track.Points[num].BottomLeftSideTangentVertex;
                backVertices[num * 4 + 1].uv = new Vector2(
                    backVertices[num * 4 + 0].U, 0.135f);
                backVertices[num * 4 + 2] =
                    track.Points[num].BottomRightSideTangentVertex;
                backVertices[num * 4 + 2].uv = new Vector2(
                    backVertices[num * 4 + 0].U, 1.0f - 0.135f);
                backVertices[num * 4 + 3] = track.Points[num].RightTangentVertex;
                backVertices[num * 4 + 3].uv = new Vector2(
                    backVertices[num * 4 + 3].U, 1.0f);
            }
            var backIndices = new int[(count - 1) * 18];
            vertexIndex = 0;
            for (int num = 0; num < count - 1; ++num)
            {
                for (int side = 0; side < 3; ++side)
                {
                    backIndices[num * 18 + 6 * side + 0] = vertexIndex + side;
                    backIndices[num * 18 + 6 * side + 1] = vertexIndex + 5 + side;
                    backIndices[num * 18 + 6 * side + 2] = vertexIndex + 4 + side;
                    backIndices[num * 18 + 6 * side + 3] = vertexIndex + 5 + side;
                    backIndices[num * 18 + 6 * side + 4] = vertexIndex + side;
                    backIndices[num * 18 + 6 * side + 5] = vertexIndex + 1 + side;
                }
                vertexIndex += 4;
            }

            int tunnelLength = 0;
            foreach (TrackLine.RoadHelperPosition helper in track.Helpers)
                if (helper.type == TrackData.RoadHelper.HelperType.Tunnel)
                    tunnelLength += 1 + helper.endNum - helper.startNum;
            var tunnelVertices = new TangentVertex[tunnelLength * 4];
            vertexIndex = 0;
            foreach (TrackLine.RoadHelperPosition helper in track.Helpers)
            {
                if (helper.type != TrackData.RoadHelper.HelperType.Tunnel) continue;
                for (int num = helper.startNum; num <= helper.endNum; ++num)
                {
                    tunnelVertices[vertexIndex + 0] =
                        track.Points[num].LeftTangentVertex;
                    tunnelVertices[vertexIndex + 0].uv = new Vector2(
                        tunnelVertices[vertexIndex + 0].U * 0.25f, 0.0f);
                    tunnelVertices[vertexIndex + 1] =
                        track.Points[num].TunnelTopLeftSideTangentVertex;
                    tunnelVertices[vertexIndex + 1].uv = new Vector2(
                        tunnelVertices[vertexIndex + 1].U * 0.25f, 0.235f);
                    tunnelVertices[vertexIndex + 2] =
                        track.Points[num].TunnelTopRightSideTangentVertex;
                    tunnelVertices[vertexIndex + 2].uv = new Vector2(
                        tunnelVertices[vertexIndex + 2].U * 0.25f, 1.0f - 0.235f);
                    tunnelVertices[vertexIndex + 3] =
                        track.Points[num].RightTangentVertex;
                    tunnelVertices[vertexIndex + 3].uv = new Vector2(
                        tunnelVertices[vertexIndex + 3].U * 0.25f, 1.0f);
                    tunnelVertices[vertexIndex + 0].normal *= -1.0f;
                    tunnelVertices[vertexIndex + 3].normal *= -1.0f;
                    tunnelVertices[vertexIndex + 0].tangent *= -1.0f;
                    tunnelVertices[vertexIndex + 3].tangent *= -1.0f;
                    vertexIndex += 4;
                }
            }
            int tunnelSegmentCount = 0;
            foreach (TrackLine.RoadHelperPosition helper in track.Helpers)
                if (helper.type == TrackData.RoadHelper.HelperType.Tunnel)
                    tunnelSegmentCount += helper.endNum - helper.startNum;
            var tunnelIndices = new int[tunnelSegmentCount * 18];
            vertexIndex = 0;
            int tunnelIndex = 0;
            foreach (TrackLine.RoadHelperPosition helper in track.Helpers)
            {
                if (helper.type != TrackData.RoadHelper.HelperType.Tunnel) continue;
                for (int num = helper.startNum; num < helper.endNum; ++num)
                {
                    for (int side = 0; side < 3; ++side)
                    {
                        tunnelIndices[tunnelIndex + 0] = vertexIndex + side;
                        tunnelIndices[tunnelIndex + 2] = vertexIndex + 4 + side;
                        tunnelIndices[tunnelIndex + 1] = vertexIndex + 5 + side;
                        tunnelIndices[tunnelIndex + 3] = vertexIndex + 5 + side;
                        tunnelIndices[tunnelIndex + 5] = vertexIndex + 1 + side;
                        tunnelIndices[tunnelIndex + 4] = vertexIndex + side;
                        tunnelIndex += 6;
                    }
                    vertexIndex += 4;
                }
                vertexIndex += 4;
            }

            report.WriteLine(
                "ROAD name={0} topVertices={1} topIndices={2} topVertexHash={3:x16} " +
                "topIndexHash={4:x16} backVertices={5} backIndices={6} " +
                "backVertexHash={7:x16} backIndexHash={8:x16} tunnelVertices={9} " +
                "tunnelIndices={10} tunnelVertexHash={11:x16} tunnelIndexHash={12:x16}",
                name, roadVertices.Length, roadIndices.Length,
                HashTangentVertices(roadVertices), HashIndices(roadIndices),
                backVertices.Length, backIndices.Length,
                HashTangentVertices(backVertices), HashIndices(backIndices),
                tunnelVertices.Length, tunnelIndices.Length,
                HashTangentVertices(tunnelVertices), HashIndices(tunnelIndices));
        }

        private static string Float(float value)
        {
            return value.ToString("R", CultureInfo.InvariantCulture);
        }

        private static string Vector(Vector3 value)
        {
            return Float(value.X) + "," + Float(value.Y) + "," + Float(value.Z);
        }

        private static void WriteOrientationPhases(
            StreamWriter report, string name, TrackLineProbe track, Landscape landscape)
        {
            int count = track.Points.Count - 1;
            var directions = new Vector3[count];
            var preUps = new Vector3[count];
            Vector3 lastUp = new Vector3(0.0f, 0.0f, 1.0f);
            for (int index = 0; index < count; ++index)
            {
                Vector3 direction = track.Points[(index + 1) % count].pos -
                    track.Points[index == 0 ? count - 1 : index - 1].pos;
                direction.Normalize();
                Vector3 middle = (track.Points[(index + 1) % count].pos +
                    track.Points[index == 0 ? count - 1 : index - 1].pos) / 2.0f;
                Vector3 optimal = middle - track.Points[index].pos;
                if (optimal.Length() < 0.0001f) optimal = lastUp;
                optimal.Normalize();
                directions[index] = direction;
                preUps[index] = optimal;
                lastUp = optimal;
            }
            preUps[0] = preUps[count - 1] + preUps[1];
            // The original List<Vector3> indexer call normalizes a temporary copy.

            lastUp = Vector3.Lerp(new Vector3(0.0f, 0.0f, 1.0f), preUps[0],
                1.5f * 0.25f * 0.6f);
            Vector3 up = Vector3.Zero;
            for (int smooth = -5; smooth <= 5; ++smooth)
                up += preUps[(count + smooth) % count];
            up.Normalize();
            up = Vector3.Lerp(lastUp, up, 0.25f);
            up.Normalize();
            Vector3 right = Vector3.Cross(directions[0], up);
            right.Normalize();
            Vector3 orthogonalUp = Vector3.Cross(right, directions[0]);
            orthogonalUp.Normalize();
            report.WriteLine(
                "PHASE name={0} preUp0={1} secondRight0={2} secondUp0={3}",
                name, Vector(preUps[0]), Vector(right), Vector(orthogonalUp));

            var secondUps = new Vector3[count];
            var secondRights = new Vector3[count];
            Vector3 lastUnmodified = lastUp;
            ulong groundHash = 14695981039346656037UL;
            int groundCount = 0;
            for (int index = 0; index < count; ++index)
            {
                up = Vector3.Zero;
                for (int smooth = -5; smooth <= 5; ++smooth)
                    up += preUps[(index + count + smooth) % count];
                up.Normalize();
                bool upsideDown = up.Z < -0.25f && lastUnmodified.Z < -0.05f;
                bool movingUp = directions[index].Z > 0.75f;
                bool movingDown = directions[index].Z < -0.75f;
                up = Vector3.Lerp(lastUp, up, 0.25f);
                up.Normalize();
                lastUnmodified = up;
                if (movingUp)
                    lastUp = Vector3.Lerp(up, new Vector3(0.0f, 0.0f, -1.0f), 0.6f);
                else if (movingDown)
                    lastUp = Vector3.Lerp(up, new Vector3(0.0f, 0.0f, 1.0f), 0.6f);
                else if (upsideDown)
                    lastUp = Vector3.Lerp(up, new Vector3(0.0f, 0.0f, -1.0f), 0.6f);
                else
                    lastUp = Vector3.Lerp(up, new Vector3(0.0f, 0.0f, 1.0f), 0.6f);
                bool nearGround = track.Points[index].pos.Z - landscape.GetMapHeight(
                    track.Points[index].pos.X, track.Points[index].pos.Y) < 8.0f;
                groundHash = HashByte(groundHash, nearGround ? (byte)1 : (byte)0);
                if (nearGround)
                {
                    ++groundCount;
                    lastUp = Vector3.Lerp(
                        up, new Vector3(0.0f, 0.0f, 1.0f), 1.75f * 0.6f);
                }
                right = Vector3.Cross(directions[index], up);
                right.Normalize();
                orthogonalUp = Vector3.Cross(right, directions[index]);
                orthogonalUp.Normalize();
                secondRights[index] = right;
                secondUps[index] = orthogonalUp;
            }
            ulong preHash = 14695981039346656037UL;
            ulong secondHash = 14695981039346656037UL;
            ulong finalHash = 14695981039346656037UL;
            Vector3 finalRight0 = Vector3.Zero;
            Vector3 finalUp0 = Vector3.Zero;
            for (int index = 0; index < count; ++index)
            {
                preHash = HashVector3(preHash, preUps[index]);
                secondHash = HashVector3(secondHash, secondRights[index]);
                secondHash = HashVector3(secondHash, secondUps[index]);
                up = Vector3.Zero;
                for (int smooth = -10; smooth <= 10; ++smooth)
                    up += secondUps[(index + count + smooth) % count];
                up.Normalize();
                Vector3 finalRight = Vector3.Cross(directions[index], up);
                if (index == 0) { finalRight0 = finalRight; finalUp0 = up; }
                finalHash = HashVector3(finalHash, finalRight);
                finalHash = HashVector3(finalHash, up);
            }
            report.WriteLine(
                "PHASEHASH name={0} pre={1:x16} second={2:x16} final={3:x16} ground={4:x16} groundCount={5}",
                name, preHash, secondHash, finalHash, groundHash, groundCount);
            report.WriteLine("PHASEFINAL name={0} right0={1} up0={2}",
                name, Vector(finalRight0), Vector(finalUp0));
        }

        private static void WriteTrack(StreamWriter report, string contentRoot, string name)
        {
            TrackData data = TrackData.Load(name);
            int inputPoints = data.TrackPoints.Count;
            int inputWidths = data.WidthHelpers.Count;
            int inputHelpers = data.RoadHelpers.Count;
            int inputObjects = data.NeutralsObjects.Count;
            var landscape = new Landscape(Path.Combine(contentRoot, "LandscapeHeights.data"));
            var track = new TrackLineProbe(data, landscape);
            TrackVertex first = track.Points[0];
            TrackVertex finalUnique = track.Points[track.Points.Count - 2];
            TrackVertex duplicate = track.Points[track.Points.Count - 1];

            report.WriteLine(
                "TRACK name={0} inputPoints={1} widthHelpers={2} roadHelpers={3} neutralObjects={4} " +
                "generatedPoints={5} helperRanges={6} emittedNeutralObjects={7} hash={8:x16} objectHash={9:x16}",
                name, inputPoints, inputWidths, inputHelpers, inputObjects, track.Points.Count,
                track.Helpers.Count, landscape.Objects.Count, HashTrack(track), HashObjects(landscape.Objects));
            report.WriteLine("FIRST name={0} pos={1} right={2} up={3} dir={4} width={5} u={6}",
                name, Vector(first.pos), Vector(first.right), Vector(first.up), Vector(first.dir),
                Float(first.roadWidth), Float(first.uv.X));
            report.WriteLine("LAST name={0} pos={1} duplicatePos={2} duplicateU={3}",
                name, Vector(finalUnique.pos), Vector(duplicate.pos), Float(duplicate.uv.X));
            report.WriteLine("FIELDHASH name={0} {1}", name, TrackFieldHashes(track));
            WriteOrientationPhases(report, name, track, landscape);
            WriteRoadGeometry(report, name, track);
            foreach (TrackLine.RoadHelperPosition helper in track.Helpers)
                report.WriteLine("HELPER name={0} type={1} start={2} end={3}",
                    name, helper.type, helper.startNum, helper.endNum);
        }

        private static void WriteCombi(StreamWriter report, string name)
        {
            var landscape = new Landscape(Path.Combine("Content", "LandscapeHeights.data"));
            var combi = new TrackCombiModels(name);
            combi.AddAllModels(landscape, Matrix.Identity);
            report.WriteLine("COMBI name={0} size={1} objects={2} hash={3:x16}", name,
                Float(combi.Size), landscape.Objects.Count, HashObjects(landscape.Objects));
        }

        private static int Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.Error.WriteLine("usage: TrackOracle <Content directory> <report path>");
                return 2;
            }

            string contentRoot = Path.GetFullPath(args[0]);
            string reportPath = Path.GetFullPath(args[1]);
            Directory.SetCurrentDirectory(Directory.GetParent(contentRoot).FullName);
            Directory.CreateDirectory(Path.GetDirectoryName(reportPath));
            using (var report = new StreamWriter(reportPath, false, new UTF8Encoding(false)))
            {
                report.WriteLine("FORMAT racing-fna-track-oracle-v1");
                Vector3 catmull = Vector3.CatmullRom(
                    new Vector3(1732.28723f, 0.0f, 0.0f),
                    new Vector3(1593.86609f, 943.755737f, 102.217514f),
                    new Vector3(1325.83545f, 832.348f, 53.6925468f),
                    new Vector3(1195.86243f, 693.090454f, 32.3657455f),
                    0.01f);
                report.WriteLine("MATH vector3CatmullRom={0:x8},{1:x8},{2:x8}",
                    BitConverter.SingleToInt32Bits(catmull.X),
                    BitConverter.SingleToInt32Bits(catmull.Y),
                    BitConverter.SingleToInt32Bits(catmull.Z));
                WriteLandscape(report, new Landscape(
                    Path.Combine(contentRoot, "LandscapeHeights.data")));
                WriteTrack(report, contentRoot, "TrackBeginner");
                WriteTrack(report, contentRoot, "TrackAdvanced");
                WriteTrack(report, contentRoot, "TrackExpert");
                string[] combis = {
                    "CombiBuildings", "CombiHotels", "CombiOilTanks", "CombiPalms",
                    "CombiPalms2", "CombiRuins", "CombiRuins2", "CombiSandCastle",
                    "CombiStones", "CombiStones2"
                };
                foreach (string combi in combis) WriteCombi(report, combi);
                report.WriteLine("RESULT PASS");
            }
            return 0;
        }
    }
}
