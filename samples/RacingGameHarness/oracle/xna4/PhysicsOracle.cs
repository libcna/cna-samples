// SPDX-License-Identifier: MS-PL

using System;
using System.Globalization;
using System.IO;
using Microsoft.Xna.Framework;
using RacingGame.GameLogic.Physics;
using RacingGame.Helpers;

namespace RacingPhysicsOracle
{
    internal static partial class Program
    {
        private const ulong OffsetBasis = 14695981039346656037UL;
        private const ulong FnvPrime = 1099511628211UL;

        private static ulong HashByte(ulong hash, byte value)
        {
            return unchecked((hash ^ value) * FnvPrime);
        }

        private static ulong HashSingle(ulong hash, float value)
        {
            uint bits = BitConverter.SingleToUInt32Bits(value);
            hash = HashByte(hash, (byte)bits);
            hash = HashByte(hash, (byte)(bits >> 8));
            hash = HashByte(hash, (byte)(bits >> 16));
            return HashByte(hash, (byte)(bits >> 24));
        }

        private static ulong HashSpringState(
            ulong hash, SpringPhysicsObject spring)
        {
            hash = HashSingle(hash, spring.pos);
            hash = HashSingle(hash, spring.velocity);
            return HashSingle(hash, spring.force);
        }

        private static ulong ProbeVectors()
        {
            ulong hash = OffsetBasis;
            hash = HashSingle(hash, Vector3Helper.GetAngleBetweenVectors(
                Vector3.UnitX, Vector3.UnitY));
            hash = HashSingle(hash, Vector3Helper.GetAngleBetweenVectors(
                new Vector3(0.6f, 0.8f, 0.0f), Vector3.UnitX));
            hash = HashSingle(hash, Vector3Helper.DistanceToLine(
                new Vector3(2.0f, 3.0f, 4.0f),
                new Vector3(-1.0f, 0.5f, 2.0f),
                new Vector3(5.0f, 4.5f, -2.0f)));
            return HashSingle(hash, Vector3Helper.SignedDistanceToPlane(
                new Vector3(2.0f, -1.0f, 5.0f),
                new Vector3(0.0f, 0.0f, 2.0f), Vector3.UnitZ));
        }

        private static ulong ProbeDefaultSpring()
        {
            var spring = new SpringPhysicsObject();
            spring.ChangePos(1.25f);
            ulong hash = HashSpringState(OffsetBasis, spring);
            foreach (float step in new[]
                     {
                         0.016f, 0.010f, 0.033f, 0.025f, 0.016f, 0.008f
                     })
            {
                spring.Simulate(step);
                hash = HashSpringState(hash, spring);
            }
            return hash;
        }

        private static ulong ProbeCarSpring()
        {
            var spring = new SpringPhysicsObject(0.75f, 0.65f, 2.25f, -0.4f);
            spring.force = 0.125f;
            ulong hash = HashSpringState(OffsetBasis, spring);
            foreach (float step in new[] { 0.020f, 0.020f, 0.015f, 0.040f })
            {
                spring.Simulate(step);
                hash = HashSpringState(hash, spring);
            }
            spring.ChangePos(-0.35f);
            return HashSpringState(hash, spring);
        }

        private static int Main(string[] args)
        {
            if (args.Length > 1)
            {
                Console.Error.WriteLine("usage: PhysicsOracle.FNA [report path]");
                return 2;
            }

            TextWriter output = Console.Out;
            StreamWriter file = null;
            if (args.Length == 1)
            {
                file = new StreamWriter(args[0], false);
                output = file;
            }

            using (file)
            {
                output.WriteLine("FORMAT racing-fna-physics-oracle-v1");
                output.WriteLine(
                    "VECTOR hash={0:x16}", ProbeVectors());
                output.WriteLine(
                    "SPRING defaultHash={0:x16} carHash={1:x16}",
                    ProbeDefaultSpring(), ProbeCarSpring());
                output.WriteLine(
                    "BASE stateHash={0:x16}", ProbeBasePlayer());
                output.WriteLine(
                    "CONTROL stateHash={0:x16}", ProbeControlMapping());
                output.WriteLine(
                    "CAR stateHash={0:x16}", ProbeCarPhysics(output));
                output.WriteLine(
                    "COLLISION stateHash={0:x16}",
                    ProbeCarCollisions(output));
                output.WriteLine("RESULT PASS");
            }
            return 0;
        }
    }
}
