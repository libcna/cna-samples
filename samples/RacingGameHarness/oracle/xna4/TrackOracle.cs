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

        internal Matrix GetTrackPositionMatrix(
            float trackPositionPercent, out float roadWidth,
            out float nextRoadWidth)
        {
            while (trackPositionPercent < 0)
                trackPositionPercent += 1;
            while (trackPositionPercent > 1)
                trackPositionPercent -= 1;
            int num = ((int)(trackPositionPercent * points.Count)) % points.Count;

            TrackVertex p1 = points[num - 1 < 0 ? points.Count - 1 : num - 1];
            TrackVertex p2 = points[num];
            TrackVertex p3 = points[(num + 1) % points.Count];
            TrackVertex p4 = points[(num + 2) % points.Count];
            float eachPointPercent = 1.0f / (float)points.Count;
            float pointPercent =
                (trackPositionPercent - num * eachPointPercent) / eachPointPercent;
            Vector3 interpolatedPos = Vector3.CatmullRom(
                p1.pos, p2.pos, p3.pos, p4.pos, pointPercent);
            Vector3 interpolatedDir = Vector3.CatmullRom(
                p1.dir, p2.dir, p3.dir, p4.dir, pointPercent);
            Vector3 interpolatedRight = Vector3.CatmullRom(
                p1.right, p2.right, p3.right, p4.right, pointPercent);
            Vector3 interpolatedUp = Vector3.CatmullRom(
                p1.up, p2.up, p3.up, p4.up, pointPercent);

            Matrix mat = Matrix.Identity;
            mat.Right = interpolatedRight;
            mat.Up = interpolatedUp;
            mat.Forward = interpolatedDir;
            mat.Translation = interpolatedPos;
            roadWidth = MathHelper.Lerp(p2.roadWidth, p3.roadWidth, pointPercent) *
                TrackVertex.RoadWidthScale;
            nextRoadWidth = p4.roadWidth * TrackVertex.RoadWidthScale;
            return mat;
        }

        internal Matrix GetTrackPositionMatrix(
            int trackSegmentNum, float trackSegmentPercent,
            out float roadWidth, out float nextRoadWidth)
        {
            if (trackSegmentPercent < 0)
                trackSegmentPercent = 0;
            if (trackSegmentPercent > 1)
                trackSegmentPercent = 1;
            float pointPercent = trackSegmentPercent;
            int num = trackSegmentNum % points.Count;

            TrackVertex p1 = points[num - 1 < 0 ? points.Count - 1 : num - 1];
            TrackVertex p2 = points[num];
            TrackVertex p3 = points[(num + 1) % points.Count];
            TrackVertex p4 = points[(num + 2) % points.Count];
            Vector3 interpolatedPos = Vector3.CatmullRom(
                p1.pos, p2.pos, p3.pos, p4.pos, pointPercent);
            Vector3 interpolatedDir = Vector3.CatmullRom(
                p1.dir, p2.dir, p3.dir, p4.dir, pointPercent);
            Vector3 interpolatedRight = Vector3.CatmullRom(
                p1.right, p2.right, p3.right, p4.right, pointPercent);
            Vector3 interpolatedUp = Vector3.CatmullRom(
                p1.up, p2.up, p3.up, p4.up, pointPercent);

            Matrix mat = Matrix.Identity;
            mat.Right = interpolatedRight;
            mat.Up = interpolatedUp;
            mat.Forward = interpolatedDir;
            mat.Translation = interpolatedPos;
            roadWidth = MathHelper.Lerp(p2.roadWidth, p3.roadWidth, pointPercent) *
                TrackVertex.RoadWidthScale;
            nextRoadWidth = MathHelper.Lerp(
                p3.roadWidth, p4.roadWidth, pointPercent) *
                TrackVertex.RoadWidthScale;
            return mat;
        }

        internal void UpdateCarTrackPosition(
            Vector3 carPos, ref int trackSegmentNumber,
            ref float trackSegmentPercent)
        {
            int num = trackSegmentNumber;
            bool gotCarInThisSegment = false;
            float thisPointDist = 0;
            float nextPointDist = 1;
            int maxNumberOfIterations = 100;
            do
            {
                TrackVertex thisPoint = points[num];
                TrackVertex nextPoint = points[(num + 1) % points.Count];
                thisPointDist = RacingGame.Helpers.Vector3Helper.SignedDistanceToPlane(
                    carPos, thisPoint.pos, -thisPoint.dir);
                nextPointDist = RacingGame.Helpers.Vector3Helper.SignedDistanceToPlane(
                    carPos, nextPoint.pos, nextPoint.dir);
                if (thisPointDist < 0)
                    num--;
                else if (nextPointDist < 0)
                    num++;
                else
                    gotCarInThisSegment = true;
                if (num < 0)
                    num = points.Count - 1;
                if (num >= points.Count)
                    num = 0;
                if (maxNumberOfIterations-- < 0)
                    return;
            } while (gotCarInThisSegment == false);

            trackSegmentNumber = num;
            float segmentLength = thisPointDist + nextPointDist;
            if (segmentLength == 0)
                trackSegmentPercent = 0;
            else
                trackSegmentPercent = thisPointDist / segmentLength;
        }

        internal bool IsTunnel(int trackSegment)
        {
            foreach (RoadHelperPosition tunnelPos in helperPositions)
                if (tunnelPos.type == TrackData.RoadHelper.HelperType.Tunnel &&
                    trackSegment >= tunnelPos.startNum &&
                    trackSegment <= tunnelPos.endNum)
                    return true;
            return false;
        }

        internal IList<int> GetCheckpointSegmentPositions()
        {
            const float checkpointGap = 500.0f;
            var result = new List<int>();
            float lastGap = checkpointGap;
            for (int num = 0; num < points.Count - 24; ++num)
            {
                float distance = Vector3.Distance(
                    points[(num + 1) % points.Count].pos, points[num].pos);
                Vector3 direction = points[num].dir;
                Vector3 up = points[num].up;
                bool upsideDown = up.Z < 0.05f;
                bool movingUp = direction.Z > 0.65f;
                bool movingDown = direction.Z < -0.65f;
                if (upsideDown || movingUp || movingDown)
                    continue;
                if (lastGap - distance <= 0.0f)
                {
                    result.Add(num);
                    lastGap += checkpointGap;
                }
                lastGap -= distance;
            }
            return result;
        }
    }

    internal sealed class StaticSceneMesh
    {
        internal TangentVertex[] Vertices;
        internal int[] Indices;

        internal StaticSceneMesh(TangentVertex[] vertices, int[] indices)
        {
            Vertices = vertices;
            Indices = indices;
        }
    }

    internal sealed class StaticSceneGeometry
    {
        internal Landscape Landscape;
        internal TrackLineProbe Track;
        internal StaticSceneMesh LandscapeMesh;
        internal StaticSceneMesh Road;
        internal StaticSceneMesh RoadBack;
        internal StaticSceneMesh Tunnel;
        internal StaticSceneMesh LeftGuard;
        internal StaticSceneMesh RightGuard;
        internal Matrix[] LeftHolders;
        internal Matrix[] RightHolders;
        internal StaticSceneMesh Columns;
        internal Vector3[] ColumnPositions;
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
            StreamWriter report, string name, TrackLineProbe track,
            StaticSceneGeometry scene = null)
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

            if (scene != null)
            {
                scene.Road = new StaticSceneMesh(roadVertices, roadIndices);
                scene.RoadBack = new StaticSceneMesh(backVertices, backIndices);
                scene.Tunnel = new StaticSceneMesh(tunnelVertices, tunnelIndices);
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

        private static TangentVertex[] GuardRailBaseVertices()
        {
            return new TangentVertex[]
            {
                new TangentVertex(new Vector3(10, 0, -105),
                    new Vector2(0.0f, 1 - 0.442877f),
                    new Vector3(-0.382683f, 0, -0.923880f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(20, 0, -105),
                    new Vector2(0.0f, 1 - 0.432881f),
                    new Vector3(0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-10, 0, -75),
                    new Vector2(0.0f, 1 - 0.402893f),
                    new Vector3(0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-10, 0, -45),
                    new Vector2(0.0f, 1 - 0.372905f),
                    new Vector3(0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(20, 0, -15),
                    new Vector2(0.0f, 1 - 0.342917f),
                    new Vector3(0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(20, 0, 15),
                    new Vector2(0.0f, 1 - 0.312929f),
                    new Vector3(0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-10, 0, 45),
                    new Vector2(0.0f, 1 - 0.282941f),
                    new Vector3(0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-10, 0, 75),
                    new Vector2(0.0f, 1 - 0.252953f),
                    new Vector3(0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(20, 0, 105),
                    new Vector2(0.0f, 1 - 0.222965f),
                    new Vector3(0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(10, 0, 105),
                    new Vector2(0.0f, 1 - 0.212969f),
                    new Vector3(-0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-20, 0, 75),
                    new Vector2(0.0f, 1 - 0.182981f),
                    new Vector3(-0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-20, 0, 45),
                    new Vector2(0.0f, 1 - 0.152993f),
                    new Vector3(-0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(10, 0, 15),
                    new Vector2(0.0f, 1 - 0.123005f),
                    new Vector3(-0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(10, 0, -15),
                    new Vector2(0.0f, 1 - 0.093017f),
                    new Vector3(-0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-20, 0, -45),
                    new Vector2(0.0f, 1 - 0.063029f),
                    new Vector3(-0.923880f, 0, 0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(-20, 0, -75),
                    new Vector2(0.0f, 1 - 0.033041f),
                    new Vector3(-0.923880f, 0, -0.382683f), new Vector3(0, -1, 0)),
                new TangentVertex(new Vector3(10, 0, -105),
                    new Vector2(0.0f, 1 - 0.003053f),
                    new Vector3(-0.382683f, 0, -0.923880f), new Vector3(0, -1, 0)),
            };
        }

        private static void WriteGuardRailGeometry(
            StreamWriter report, string name, TrackLineProbe track, bool left,
            StaticSceneGeometry scene = null)
        {
            TangentVertex[] source = GuardRailBaseVertices();
            var railPoints = new TrackVertex[track.Points.Count / 2 + 1];
            for (int num = 0; num < railPoints.Length; ++num)
            {
                int pointNum = num * 2;
                if (pointNum >= track.Points.Count - 1)
                    pointNum = track.Points.Count - 1;
                if (left)
                {
                    railPoints[num] = track.Points[pointNum].LeftTrackVertex;
                    railPoints[num].right = -railPoints[num].right;
                    railPoints[num].dir = -railPoints[num].dir;
                    railPoints[num].pos -= railPoints[num].right * 0.5f;
                }
                else
                {
                    railPoints[num] = track.Points[pointNum].RightTrackVertex;
                    railPoints[num].pos -= railPoints[num].right * 0.5f;
                }
            }

            var vertices = new TangentVertex[railPoints.Length * source.Length];
            var holders = new List<Tuple<string, Matrix, bool>>();
            float u = 0.5f;
            float lastGap = 0.0f;
            for (int num = 0; num < railPoints.Length; ++num)
            {
                Vector3 right = railPoints[num].right;
                Vector3 direction = railPoints[num].dir;
                Vector3 up = railPoints[num].up;
                Matrix pointSpace = Matrix.Identity;
                pointSpace.M11 = right.X; pointSpace.M12 = right.Y; pointSpace.M13 = right.Z;
                pointSpace.M21 = direction.X; pointSpace.M22 = direction.Y;
                pointSpace.M23 = direction.Z;
                pointSpace.M31 = up.X; pointSpace.M32 = up.Y; pointSpace.M33 = up.Z;
                Vector3 localPos = railPoints[num].pos + up * (1.35f * 1.5f * 0.425f);
                for (int index = 0; index < source.Length; ++index)
                {
                    Vector3 pos = Vector3.Transform(
                        source[index].pos * 0.0019f,
                        pointSpace * Matrix.CreateTranslation(localPos));
                    Vector3 normal = Vector3.TransformNormal(
                        (left ? -1.0f : 1.0f) * source[index].normal, pointSpace);
                    Vector3 tangent = Vector3.TransformNormal(-source[index].tangent,
                        pointSpace);
                    vertices[num * source.Length + index] = new TangentVertex(
                        pos, u, source[index].V, normal, tangent);
                }

                float distance = Vector3.Distance(
                    railPoints[(num + 1) % railPoints.Length].pos,
                    railPoints[num].pos);
                u += (1.0f / 15.0f) * distance * 2.0f;
                if (lastGap - distance <= 0.0f)
                {
                    Vector3 p1 = railPoints[num - 1 < 0 ? railPoints.Length - 1 : num - 1].pos;
                    Vector3 p2 = railPoints[num].pos;
                    Vector3 p3 = railPoints[(num + 1) % railPoints.Length].pos;
                    Vector3 p4 = railPoints[(num + 2) % railPoints.Length].pos;
                    Vector3 holderPoint = Vector3.CatmullRom(
                        p1, p2, p3, p4, lastGap / distance);
                    Matrix matrix = Matrix.CreateScale(1.125f) *
                        Matrix.CreateTranslation(new Vector3(0.225f, 0, 0)) *
                        pointSpace * Matrix.CreateTranslation(holderPoint);
                    holders.Add(Tuple.Create("GuardRailHolder", matrix, false));
                    lastGap += 15.0f;
                }
                lastGap -= distance;
            }

            int quads = source.Length - 1;
            var indices = new int[6 * quads * (railPoints.Length - 1)];
            int vertexIndex = 0;
            for (int num = 0; num < railPoints.Length - 1; ++num)
            {
                for (int index = 0; index < quads; ++index)
                {
                    int output = 6 * (num * quads + index);
                    indices[output + 0] = vertexIndex + index;
                    indices[output + 1] = vertexIndex + 1 + index;
                    indices[output + 2] = vertexIndex + 1 + source.Length + index;
                    indices[output + 3] = indices[output + 2];
                    indices[output + 4] = vertexIndex + source.Length + index;
                    indices[output + 5] = indices[output + 0];
                }
                vertexIndex += source.Length;
            }

            if (scene != null)
            {
                var holderMatrices = new Matrix[holders.Count];
                for (int index = 0; index < holders.Count; ++index)
                    holderMatrices[index] = holders[index].Item2;
                if (left)
                {
                    scene.LeftGuard = new StaticSceneMesh(vertices, indices);
                    scene.LeftHolders = holderMatrices;
                }
                else
                {
                    scene.RightGuard = new StaticSceneMesh(vertices, indices);
                    scene.RightHolders = holderMatrices;
                }
            }

            report.WriteLine(
                "GUARD name={0} mode={1} points={2} vertices={3} indices={4} " +
                "vertexHash={5:x16} indexHash={6:x16} holders={7} holderHash={8:x16}",
                name, left ? "Left" : "Right", railPoints.Length, vertices.Length,
                indices.Length, HashTangentVertices(vertices), HashIndices(indices),
                holders.Count, HashObjects(holders));
        }

        private static TangentVertex[] ColumnBaseVertices()
        {
            return new TangentVertex[]
            {
                new TangentVertex(new Vector3(1, 0, 0), new Vector2(0.0f / 6.0f, 0),
                    new Vector3(1, 0, 0), new Vector3(0, 0, -1)),
                new TangentVertex(new Vector3(0.5f, 0.866025f, 0),
                    new Vector2(1.0f / 6.0f, 0), new Vector3(0.5f, 0.866025f, 0),
                    new Vector3(0, 0, -1)),
                new TangentVertex(new Vector3(-0.5f, 0.866025f, 0),
                    new Vector2(2.0f / 6.0f, 0), new Vector3(-0.5f, 0.866025f, 0),
                    new Vector3(0, 0, -1)),
                new TangentVertex(new Vector3(-1, 0, 0), new Vector2(3.0f / 6.0f, 0),
                    new Vector3(-1, 0, 0), new Vector3(0, 0, -1)),
                new TangentVertex(new Vector3(-0.5f, -0.866025f, 0),
                    new Vector2(4.0f / 6.0f, 0), new Vector3(-0.5f, -0.866025f, 0),
                    new Vector3(0, 0, -1)),
                new TangentVertex(new Vector3(0.5f, -0.866025f, 0),
                    new Vector2(5.0f / 6.0f, 0), new Vector3(0.5f, -0.866025f, 0),
                    new Vector3(0, 0, -1)),
                new TangentVertex(new Vector3(1, 0, 0), new Vector2(6.0f / 6.0f, 0),
                    new Vector3(1, 0, 0), new Vector3(0, 0, -1)),
            };
        }

        private static void WriteColumnGeometry(
            StreamWriter report, string name, TrackLineProbe track, Landscape landscape,
            StaticSceneGeometry scene = null)
        {
            var positions = new List<Vector3>();
            var topSpaces = new List<Matrix>();
            var bottomSpaces = new List<Matrix>();
            float remaining = 33.0f;
            for (int num = 0; num < track.Points.Count; ++num)
            {
                float distance = Vector3.Distance(
                    track.Points[(num + 1) % track.Points.Count].pos,
                    track.Points[num].pos);
                if (remaining - distance <= 0.0f)
                {
                    Vector3 p1 = track.Points[num - 1 < 0 ? track.Points.Count - 1 : num - 1].pos;
                    Vector3 p2 = track.Points[num].pos;
                    Vector3 p3 = track.Points[(num + 1) % track.Points.Count].pos;
                    Vector3 p4 = track.Points[(num + 2) % track.Points.Count].pos;
                    Vector3 point = Vector3.CatmullRom(p1, p2, p3, p4,
                        remaining / distance);
                    float draft = Vector3.Dot(track.Points[num].up, Vector3.UnitZ);
                    float height = point.Z - landscape.GetMapHeight(point.X, point.Y);
                    if (draft > 0.3f && height > 2.5f)
                    {
                        positions.Add(point);
                        Vector3 right = track.Points[num].right;
                        Vector3 direction = track.Points[num].dir;
                        Vector3 up = track.Points[num].up;
                        Matrix space = Matrix.Identity;
                        space.M11 = right.X; space.M12 = right.Y; space.M13 = right.Z;
                        space.M21 = direction.X; space.M22 = direction.Y;
                        space.M23 = direction.Z;
                        space.M31 = up.X; space.M32 = up.Y; space.M33 = up.Z;
                        topSpaces.Add(space);

                        space = Matrix.Identity;
                        Vector3 bottomRight = Vector3.Cross(direction, Vector3.UnitZ);
                        space.M11 = bottomRight.X; space.M12 = bottomRight.Y;
                        space.M13 = bottomRight.Z;
                        space.M21 = direction.X; space.M22 = direction.Y;
                        space.M23 = direction.Z;
                        bottomSpaces.Add(space);
                    }
                    remaining += 33.0f;
                }
                remaining -= distance;
            }

            TangentVertex[] source = ColumnBaseVertices();
            var vertices = new TangentVertex[positions.Count * source.Length * 2];
            var objects = new List<Tuple<string, Matrix, bool>>();
            for (int num = 0; num < positions.Count; ++num)
            {
                Vector3 pos = positions[num];
                Vector3 bottom = new Vector3(pos.X, pos.Y,
                    landscape.GetMapHeight(pos.X, pos.Y) + 1.0f);
                Vector3 top = new Vector3(pos.X, pos.Y, pos.Z - 0.55f);
                float topV = Vector3.Distance(top, bottom) / (MathHelper.Pi * 2.0f);
                for (int topBottom = 0; topBottom < 2; ++topBottom)
                    for (int index = 0; index < source.Length; ++index)
                    {
                        int output = num * source.Length * 2 + topBottom * source.Length + index;
                        Matrix transform = topBottom == 0 ? bottomSpaces[num] : topSpaces[num];
                        vertices[output] = new TangentVertex(
                            (topBottom == 0 ? bottom : top) +
                                Vector3.Transform(source[index].pos, transform),
                            source[index].U, topBottom == 0 ? 0.0f : topV,
                            Vector3.Transform(source[index].normal, transform),
                            Vector3.Transform(-source[index].tangent, transform));
                    }
                objects.Add(Tuple.Create("RoadColumnSegment",
                    Matrix.CreateTranslation(new Vector3(
                        bottom.X, bottom.Y, bottom.Z - 1.0f)), false));
            }

            int quads = source.Length - 1;
            var indices = new int[6 * quads * positions.Count];
            int vertexIndex = 0;
            for (int num = 0; num < positions.Count; ++num)
            {
                for (int index = 0; index < quads; ++index)
                {
                    int output = 6 * (num * quads + index);
                    indices[output + 0] = vertexIndex + index;
                    indices[output + 1] = vertexIndex + 1 + source.Length + index;
                    indices[output + 2] = vertexIndex + 1 + index;
                    indices[output + 3] = indices[output + 1];
                    indices[output + 4] = indices[output + 0];
                    indices[output + 5] = vertexIndex + source.Length + index;
                }
                vertexIndex += source.Length * 2;
            }

            if (scene != null)
            {
                scene.Columns = new StaticSceneMesh(vertices, indices);
                scene.ColumnPositions = positions.ToArray();
            }

            ulong positionHash = 14695981039346656037UL;
            foreach (Vector3 position in positions)
                positionHash = HashVector3(positionHash, position);
            report.WriteLine(
                "COLUMN name={0} columns={1} positionHash={2:x16} vertices={3} " +
                "indices={4} vertexHash={5:x16} indexHash={6:x16} objects={7} " +
                "objectHash={8:x16}", name, positions.Count, positionHash,
                vertices.Length, indices.Length, HashTangentVertices(vertices),
                HashIndices(indices), objects.Count, HashObjects(objects));
        }

        internal static StaticSceneGeometry BuildStaticSceneGeometry(
            string contentRoot, string name)
        {
            var landscape = new Landscape(
                Path.Combine(contentRoot, "LandscapeHeights.data"));
            var track = new TrackLineProbe(TrackData.Load(name), landscape);
            var scene = new StaticSceneGeometry();
            scene.Landscape = landscape;
            scene.Track = track;
            var landscapeIndices = new int[landscape.Indices.Length];
            for (int index = 0; index < landscape.Indices.Length; ++index)
                landscapeIndices[index] = unchecked((int)landscape.Indices[index]);
            scene.LandscapeMesh = new StaticSceneMesh(
                landscape.Vertices, landscapeIndices);
            using (var sink = new StreamWriter(Stream.Null))
            {
                WriteRoadGeometry(sink, name, track, scene);
                WriteGuardRailGeometry(sink, name, track, true, scene);
                WriteGuardRailGeometry(sink, name, track, false, scene);
                WriteColumnGeometry(sink, name, track, landscape, scene);
            }
            return scene;
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
            WriteKinematics(report, name, track);
            IList<int> checkpoints = track.GetCheckpointSegmentPositions();
            report.WriteLine("CHECKPOINT name={0} count={1} segments={2}",
                name, checkpoints.Count, string.Join(",", checkpoints));
            WriteOrientationPhases(report, name, track, landscape);
            WriteRoadGeometry(report, name, track);
            WriteGuardRailGeometry(report, name, track, true);
            WriteGuardRailGeometry(report, name, track, false);
            WriteColumnGeometry(report, name, track, landscape);
            foreach (TrackLine.RoadHelperPosition helper in track.Helpers)
                report.WriteLine("HELPER name={0} type={1} start={2} end={3}",
                    name, helper.type, helper.startNum, helper.endNum);
        }

        private static void WriteKinematics(
            StreamWriter report, string name, TrackLineProbe track)
        {
            ulong startHash = 14695981039346656037UL;
            startHash = HashVector3(startHash, track.Points[0].pos);
            startHash = HashVector3(startHash, track.Points[0].dir);
            startHash = HashVector3(startHash, track.Points[0].up);
            startHash = HashSingle(startHash,
                track.Points.Count * 100.0f / 40.0f);
            startHash = HashInt32(startHash, track.Points.Count);

            ulong percentHash = 14695981039346656037UL;
            float[] percents = {
                -0.125f, 0.0f, 0.0001f, 0.249f,
                0.5f, 0.999f, 1.0f, 1.125f
            };
            foreach (float percent in percents)
            {
                float roadWidth;
                float nextRoadWidth;
                Matrix matrix = track.GetTrackPositionMatrix(
                    percent, out roadWidth, out nextRoadWidth);
                percentHash = HashMatrix(percentHash, matrix);
                percentHash = HashSingle(percentHash, roadWidth);
                percentHash = HashSingle(percentHash, nextRoadWidth);
            }

            ulong segmentHash = 14695981039346656037UL;
            int[] segments = {
                0, 1, track.Points.Count / 2,
                track.Points.Count - 2, track.Points.Count + 3
            };
            float[] segmentPercents = { -0.5f, 0.0f, 0.37f, 1.0f, 1.5f };
            for (int index = 0; index < segments.Length; ++index)
            {
                float roadWidth;
                float nextRoadWidth;
                Matrix matrix = track.GetTrackPositionMatrix(
                    segments[index], segmentPercents[index],
                    out roadWidth, out nextRoadWidth);
                segmentHash = HashMatrix(segmentHash, matrix);
                segmentHash = HashSingle(segmentHash, roadWidth);
                segmentHash = HashSingle(segmentHash, nextRoadWidth);
            }

            ulong updateHash = 14695981039346656037UL;
            int[] targets = {
                0, 7, track.Points.Count / 3, track.Points.Count - 6, 2
            };
            float[] targetPercents = { 0.2f, 0.75f, 0.41f, 0.9f, 0.05f };
            int[] guesses = {
                0, 4, track.Points.Count / 3 - 3,
                track.Points.Count - 10, track.Points.Count - 2
            };
            float[] lateral = { 0.0f, 1.5f, -2.0f, 0.75f, -0.25f };
            for (int index = 0; index < targets.Length; ++index)
            {
                float roadWidth;
                float nextRoadWidth;
                Matrix matrix = track.GetTrackPositionMatrix(
                    targets[index], targetPercents[index],
                    out roadWidth, out nextRoadWidth);
                Vector3 carPosition = matrix.Translation +
                    matrix.Right * lateral[index] + matrix.Up * 0.25f;
                int segment = guesses[index];
                float segmentPercent = -1.0f;
                track.UpdateCarTrackPosition(
                    carPosition, ref segment, ref segmentPercent);
                updateHash = HashInt32(updateHash, segment);
                updateHash = HashSingle(updateHash, segmentPercent);
            }

            ulong tunnelHash = 14695981039346656037UL;
            for (int segment = 0; segment < track.Points.Count; ++segment)
                tunnelHash = HashByte(
                    tunnelHash, track.IsTunnel(segment) ? (byte)1 : (byte)0);

            report.WriteLine(
                "KINEMATIC name={0} startHash={1:x16} percentHash={2:x16} " +
                "segmentHash={3:x16} updateHash={4:x16} tunnelHash={5:x16}",
                name, startHash, percentHash, segmentHash, updateHash, tunnelHash);
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
