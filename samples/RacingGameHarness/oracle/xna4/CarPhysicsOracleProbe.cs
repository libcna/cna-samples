// SPDX-License-Identifier: MS-PL

using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using RacingGame.GameLogic;

namespace RacingPhysicsOracle
{
    internal static partial class Program
    {
        private sealed class BasePlayerProbe : BasePlayer
        {
            public float Zoom
            {
                get => ZoomInTime;
                set => ZoomInTime = value;
            }

            public void CompleteLap() => StartNewLap();
        }

        private sealed class PlayerProbe : Player
        {
            public PlayerProbe(Vector3 position) : base(position)
            {
            }

            public void SetZoom(float value) => ZoomInTime = value;

            public void CompleteLapAt(float milliseconds)
            {
                currentGameTimeMilliseconds = milliseconds;
                StartNewLap();
            }

            public void ForceGameOver(bool won)
            {
                isGameOver = true;
                victory = won;
            }

            public void SetLap(int value) => lap = value;
            public void SetOnGround(bool value) => isCarOnGround = value;
        }

        private static ulong HashInt32(ulong hash, int value)
        {
            uint bits = unchecked((uint)value);
            hash = HashByte(hash, (byte)bits);
            hash = HashByte(hash, (byte)(bits >> 8));
            hash = HashByte(hash, (byte)(bits >> 16));
            return HashByte(hash, (byte)(bits >> 24));
        }

        private static ulong HashVector3(ulong hash, Vector3 value)
        {
            hash = HashSingle(hash, value.X);
            hash = HashSingle(hash, value.Y);
            return HashSingle(hash, value.Z);
        }

        private static ulong HashMatrix(ulong hash, Matrix value)
        {
            hash = HashSingle(hash, value.M11);
            hash = HashSingle(hash, value.M12);
            hash = HashSingle(hash, value.M13);
            hash = HashSingle(hash, value.M14);
            hash = HashSingle(hash, value.M21);
            hash = HashSingle(hash, value.M22);
            hash = HashSingle(hash, value.M23);
            hash = HashSingle(hash, value.M24);
            hash = HashSingle(hash, value.M31);
            hash = HashSingle(hash, value.M32);
            hash = HashSingle(hash, value.M33);
            hash = HashSingle(hash, value.M34);
            hash = HashSingle(hash, value.M41);
            hash = HashSingle(hash, value.M42);
            hash = HashSingle(hash, value.M43);
            return HashSingle(hash, value.M44);
        }

        private static void SetInput(int frame)
        {
            Input.KeyboardLeftPressed = false;
            Input.KeyboardRightPressed = false;
            Input.KeyboardUpPressed = false;
            Input.KeyboardDownPressed = false;
            Input.MouseXMovement = 0.0f;
            Input.MouseYMovement = 0.0f;
            Input.MouseWheelDelta = 0;
            Input.MouseLeftButtonPressed = false;
            Input.MouseRightButtonPressed = false;
            Input.MouseMiddleButtonPressed = false;
            Input.GamePadAPressed = false;
            Input.GamePadBPressed = false;
            Input.GamePadXPressed = false;
            Input.GamePadYPressed = false;
            Input.GamePadLeftPressed = false;
            Input.GamePadRightPressed = false;
            Input.GamePadUpPressed = false;
            Input.GamePadDownPressed = false;
            Input.IsGamePadConnected = false;
            Input.Keyboard = new KeyboardState();
            Input.GamePad = new GamePadState();

            if (frame >= 330 && frame < 420)
            {
                Input.KeyboardUpPressed = true;
                Input.Keyboard = new KeyboardState(Keys.W);
            }
            else if (frame < 460 && frame >= 420)
            {
                Input.KeyboardLeftPressed = true;
                Input.Keyboard = new KeyboardState(Keys.A, Keys.W);
            }
            else if (frame < 490 && frame >= 460)
            {
                Input.MouseXMovement = (frame & 1) == 0 ? 2.5f : -1.25f;
                Input.MouseLeftButtonPressed = true;
            }
            else if (frame < 520 && frame >= 490)
            {
                Input.Keyboard = new KeyboardState(Keys.Space);
            }
            else if (frame < 550 && frame >= 520)
            {
                Input.KeyboardDownPressed = true;
                Input.Keyboard = new KeyboardState(Keys.S);
            }
            else if (frame >= 550)
            {
                Input.IsGamePadConnected = true;
                Input.GamePad = new GamePadState(
                    new Vector2(0.35f, 0.0f), Vector2.Zero,
                    0.0f, 0.65f, Buttons.DPadRight);
            }
        }

        private static ulong ProbeBasePlayer()
        {
            var player = new BasePlayerProbe();
            player.Reset();
            ulong hash = OffsetBasis;
            hash = HashInt32(hash, player.CurrentLap);
            hash = HashSingle(hash, player.BestTimeMilliseconds);
            hash = HashSingle(hash, player.GameTimeMilliseconds);
            hash = HashSingle(hash, player.Zoom);
            hash = HashInt32(hash, player.GameOver ? 1 : 0);

            BaseGame.ElapsedTimeThisFrameInMilliseconds = 100.0f;
            player.Update();
            for (int index = 0; index < 4; index++)
            {
                BaseGame.ElapsedTimeThisFrameInMilliseconds = 1600.0f;
                player.Update();
            }
            player.Zoom = 0.0f;
            BaseGame.ElapsedTimeThisFrameInMilliseconds = 33.0f;
            player.Update();
            player.CompleteLap();
            player.SetGameOverAndUploadHighscore();
            player.SetGameOverAndUploadHighscore();

            hash = HashInt32(hash, player.CurrentLap);
            hash = HashSingle(hash, player.BestTimeMilliseconds);
            hash = HashSingle(hash, player.GameTimeMilliseconds);
            hash = HashSingle(hash, player.Zoom);
            hash = HashInt32(hash, player.GameOver ? 1 : 0);
            hash = HashInt32(hash, RacingGameManager.Landscape.StartedLaps);
            hash = HashInt32(hash, RacingGameManager.Landscape.StartLightHash);
            hash = HashInt32(hash, RacingGame.Graphics.Highscores.Submissions);
            return hash;
        }

        private static ulong ProbeControlMapping()
        {
            var keyboard = new KeyboardState(
                Keys.Left, Keys.A, Keys.E, Keys.PageUp,
                Keys.W, Keys.O, Keys.Space);
            var mouse = new MouseState(
                17, 23, 240, ButtonState.Pressed,
                ButtonState.Pressed, ButtonState.Released,
                ButtonState.Released, ButtonState.Released);
            var gamePad = new GamePadState(
                new Vector2(0.35f, -0.2f), Vector2.Zero,
                0.75f, 0.65f,
                Buttons.DPadRight, Buttons.A, Buttons.X);
            ulong hash = OffsetBasis;
            Action<bool> addBool = value =>
                hash = HashInt32(hash, value ? 1 : 0);
            addBool(keyboard.IsKeyDown(Keys.Left));
            addBool(keyboard.IsKeyDown(Keys.Right));
            addBool(keyboard.IsKeyDown(Keys.A));
            addBool(keyboard.IsKeyDown(Keys.D));
            addBool(keyboard.IsKeyDown(Keys.E));
            hash = HashSingle(hash, 2.25f);
            addBool(gamePad.IsConnected);
            hash = HashSingle(hash, gamePad.ThumbSticks.Left.X);
            addBool(gamePad.DPad.Left == ButtonState.Pressed);
            addBool(gamePad.DPad.Right == ButtonState.Pressed);
            addBool(keyboard.IsKeyDown(Keys.PageUp));
            addBool(keyboard.IsKeyDown(Keys.PageDown));
            addBool(gamePad.Buttons.X == ButtonState.Pressed);
            addBool(gamePad.Buttons.Y == ButtonState.Pressed);
            hash = HashInt32(hash, -120);
            addBool(keyboard.IsKeyDown(Keys.Up));
            addBool(keyboard.IsKeyDown(Keys.Down));
            addBool(keyboard.IsKeyDown(Keys.W));
            addBool(keyboard.IsKeyDown(Keys.S));
            addBool(keyboard.IsKeyDown(Keys.O));
            addBool(mouse.LeftButton == ButtonState.Pressed);
            addBool(mouse.RightButton == ButtonState.Pressed);
            addBool(mouse.MiddleButton == ButtonState.Pressed);
            addBool(gamePad.Buttons.A == ButtonState.Pressed);
            addBool(gamePad.Buttons.B == ButtonState.Pressed);
            hash = HashSingle(hash, gamePad.Triggers.Left);
            hash = HashSingle(hash, gamePad.Triggers.Right);
            addBool(gamePad.DPad.Up == ButtonState.Pressed);
            addBool(gamePad.DPad.Down == ButtonState.Pressed);
            addBool(keyboard.IsKeyDown(Keys.Space));
            hash = HashSingle(hash, -1.75f);
            hash = HashSingle(hash, gamePad.ThumbSticks.Left.Y);
            hash = HashSingle(hash, gamePad.ThumbSticks.Right.Y);
            addBool(keyboard.IsKeyDown(Keys.LeftShift));
            addBool(keyboard.IsKeyDown(Keys.Home));
            addBool(keyboard.IsKeyDown(Keys.End));
            return hash;
        }

        private static void SetChaseInput(int frame)
        {
            SetInput(-1);
            Input.MouseXMovement = frame % 7 == 0 ? 1.25f : 0.0f;
            Input.MouseYMovement = frame % 11 == 0 ? -0.75f : 0.0f;
            Input.KeyboardLeftPressed = frame >= 8 && frame < 16;
            Input.KeyboardDownPressed = frame >= 16 && frame < 24;

            var keys = new System.Collections.Generic.List<Keys>();
            if (frame >= 24 && frame < 30)
                keys.Add(Keys.PageUp);
            if (frame >= 30 && frame < 36)
            {
                keys.Add(Keys.Home);
                keys.Add(Keys.LeftShift);
            }
            Input.Keyboard = new KeyboardState(keys.ToArray());
            Input.MouseWheelDelta = frame == 40 ? 8 : 0;

            Vector2 left = frame >= 48
                ? new Vector2(0.2f, -0.15f)
                : Vector2.Zero;
            Vector2 right = frame >= 56
                ? new Vector2(0.0f, 0.25f)
                : Vector2.Zero;
            Input.GamePad = new GamePadState(
                left, right, 0.0f, 0.0f, new Buttons[0]);
        }

        private static ulong ProbeChaseCamera(System.IO.TextWriter output)
        {
            RacingGameManager.Landscape.Reset();
            BaseGame.MoveFactorPerSecond = 0.016f;
            BaseGame.ElapsedTimeThisFrameInMilliseconds = 16.0f;
            BaseGame.TotalTimeMilliseconds = 0.0f;
            BaseGame.ViewMatrix = Matrix.Identity;

            var camera = new Player(new Vector3(1.0f, 2.0f, 3.0f));
            RacingGameManager.Player = camera;
            camera.Reset();
            camera.SetCameraPosition(new Vector3(5.0f, 7.0f, 11.0f));
            camera.InterpolateCameraPosition(new Vector3(6.0f, 8.0f, 14.0f));
            camera.FreeCamera = true;
            ChaseCamera.WobbelCamera(0.0f);

            ulong hash = OffsetBasis;
            for (int frame = 0; frame < 72; frame++)
            {
                BaseGame.TotalTimeMilliseconds +=
                    BaseGame.ElapsedTimeThisFrameInMilliseconds;
                SetChaseInput(frame);
                camera.Update();
                hash = HashVector3(hash, camera.CameraPosition);
                hash = HashMatrix(hash, camera.RotationMatrix);
                hash = HashMatrix(hash, BaseGame.ViewMatrix);
                hash = HashVector3(hash, ChaseCamera.XAxis);
                hash = HashVector3(hash, ChaseCamera.YAxis);
                hash = HashVector3(hash, ChaseCamera.ZAxis);
                Vector3 cameraPosition = camera.CameraPosition;
                Matrix rotation = camera.RotationMatrix;
                Matrix view = BaseGame.ViewMatrix;
                Vector3 xAxis = ChaseCamera.XAxis;
                Vector3 yAxis = ChaseCamera.YAxis;
                Vector3 zAxis = ChaseCamera.ZAxis;
                float[] values =
                {
                    cameraPosition.X, cameraPosition.Y, cameraPosition.Z,
                    rotation.M11, rotation.M12, rotation.M13, rotation.M14,
                    rotation.M21, rotation.M22, rotation.M23, rotation.M24,
                    rotation.M31, rotation.M32, rotation.M33, rotation.M34,
                    rotation.M41, rotation.M42, rotation.M43, rotation.M44,
                    view.M11, view.M12, view.M13, view.M14,
                    view.M21, view.M22, view.M23, view.M24,
                    view.M31, view.M32, view.M33, view.M34,
                    view.M41, view.M42, view.M43, view.M44,
                    xAxis.X, xAxis.Y, xAxis.Z,
                    yAxis.X, yAxis.Y, yAxis.Z,
                    zAxis.X, zAxis.Y, zAxis.Z,
                };
                output.Write("CHASESTATE{0:D3} bits=", frame);
                for (int index = 0; index < values.Length; index++)
                {
                    if (index != 0)
                        output.Write(',');
                    output.Write("{0:x8}",
                        BitConverter.SingleToUInt32Bits(values[index]));
                }
                output.WriteLine();
            }
            return hash;
        }

        private static ulong HashTextEntries(ulong hash)
        {
            foreach (RacingGame.Graphics.TextEntry entry in
                     RacingGame.Graphics.TextureFont.Entries)
            {
                hash = HashInt32(hash, entry.X);
                hash = HashInt32(hash, entry.Y);
                foreach (char value in entry.Text)
                    hash = HashByte(hash, (byte)value);
                hash = HashInt32(hash, unchecked((int)entry.Color.PackedValue));
                hash = HashSingle(hash, entry.Scale);
            }
            return hash;
        }

        private static ulong ProbePlayer()
        {
            RacingGameManager.InGame = true;
            RacingGameManager.InMenu = false;
            BaseGame.MoveFactorPerSecond = 0.016f;
            BaseGame.ElapsedTimeThisFrameInMilliseconds = 16.0f;
            BaseGame.TotalTimeMilliseconds = 0.0f;
            BaseGame.ViewMatrix = Matrix.Identity;
            RacingGame.Graphics.TextureFont.Reset();
            RacingGame.Sounds.Sound.Reset();

            var resultPlayer = new PlayerProbe(Vector3.Zero);
            RacingGameManager.Player = resultPlayer;
            resultPlayer.Reset();
            resultPlayer.SetZoom(0.0f);
            resultPlayer.CompleteLapAt(12345.0f);
            resultPlayer.AddLapTime(61.23f);
            resultPlayer.AddLapTime(5.5f);
            resultPlayer.ForceGameOver(true);
            resultPlayer.Update();

            ulong hash = OffsetBasis;
            hash = HashInt32(hash, resultPlayer.GameOver ? 1 : 0);
            hash = HashInt32(hash, resultPlayer.Victory ? 1 : 0);
            hash = HashInt32(hash, resultPlayer.CurrentLap);
            hash = HashSingle(hash, resultPlayer.BestTimeMilliseconds);
            hash = HashSingle(hash, resultPlayer.GameTimeMilliseconds);
            hash = HashVector3(hash, resultPlayer.CameraPosition);
            hash = HashMatrix(hash, BaseGame.ViewMatrix);
            hash = HashInt32(hash,
                RacingGame.Graphics.TextureFont.Entries.Count);
            ulong textHash = HashTextEntries(OffsetBasis);
            hash = HashInt32(hash, unchecked((int)textHash));
            hash = HashInt32(hash, unchecked((int)(textHash >> 32)));

            RacingGame.Graphics.TextureFont.Reset();
            RacingGame.Sounds.Sound.Reset();
            var losingPlayer = new PlayerProbe(new Vector3(25.0f, 0.0f, 0.0f));
            RacingGameManager.Player = losingPlayer;
            losingPlayer.Reset();
            losingPlayer.SetZoom(0.0f);
            losingPlayer.SetGroundPlaneAndGuardRails(
                Vector3.Zero, Vector3.UnitZ,
                new Vector3(-100.0f, -100.0f, 0.0f),
                new Vector3(-100.0f, 100.0f, 0.0f),
                new Vector3(100.0f, -100.0f, 0.0f),
                new Vector3(100.0f, 100.0f, 0.0f));
            losingPlayer.Update();
            hash = HashInt32(hash, losingPlayer.GameOver ? 1 : 0);
            hash = HashInt32(hash, losingPlayer.Victory ? 1 : 0);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.LoseSounds);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.VictorySounds);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.GearStops);

            RacingGame.Graphics.TextureFont.Reset();
            RacingGame.Sounds.Sound.Reset();
            var winningPlayer = new PlayerProbe(Vector3.Zero);
            RacingGameManager.Player = winningPlayer;
            winningPlayer.Reset();
            winningPlayer.SetZoom(0.0f);
            winningPlayer.SetLap(3);
            winningPlayer.SetOnGround(true);
            winningPlayer.SetGroundPlaneAndGuardRails(
                Vector3.Zero, Vector3.UnitZ,
                new Vector3(-100.0f, -100.0f, 0.0f),
                new Vector3(-100.0f, 100.0f, 0.0f),
                new Vector3(100.0f, -100.0f, 0.0f),
                new Vector3(100.0f, 100.0f, 0.0f));
            winningPlayer.Update();
            hash = HashInt32(hash, winningPlayer.GameOver ? 1 : 0);
            hash = HashInt32(hash, winningPlayer.Victory ? 1 : 0);
            hash = HashInt32(hash, winningPlayer.CurrentLap);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.LoseSounds);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.VictorySounds);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.GearStops);
            return hash;
        }

        private static ulong ProbeFullRace(
            System.IO.TextWriter output, out bool passed)
        {
            RacingGameManager.Landscape.Reset();
            RacingGameManager.Landscape.CheckpointSegmentPositions.AddRange(
                new[] { 1, 2, 3 });
            RacingGameManager.Landscape.CheckpointDifferences.AddRange(
                new[] { -250, 375, 0 });
            RacingGameManager.Landscape.FullRaceLifecycle = true;
            RacingGameManager.InGame = true;
            RacingGameManager.InMenu = false;
            RacingGame.Graphics.Highscores.Reset();
            RacingGame.Graphics.TextureFont.Reset();
            RacingGame.Sounds.Sound.Reset();
            BaseGame.UI.Reset();
            BaseGame.MoveFactorPerSecond = 0.016f;
            BaseGame.ElapsedTimeThisFrameInMilliseconds = 0.0f;
            BaseGame.TotalTimeMilliseconds = 0.0f;
            BaseGame.ViewMatrix = Matrix.Identity;
            SetInput(-1);

            var player = new PlayerProbe(Vector3.Zero);
            RacingGameManager.Player = player;
            player.Reset();
            player.SetZoom(0.0f);

            ulong hash = OffsetBasis;
            int state = 0;
            Action<int, float> advance = (segment, milliseconds) =>
            {
                RacingGameManager.Landscape.ScriptedSegment = segment;
                BaseGame.ElapsedTimeThisFrameInMilliseconds = milliseconds;
                BaseGame.TotalTimeMilliseconds += milliseconds;
                player.Update();

                hash = HashInt32(hash, player.CurrentLap);
                hash = HashSingle(hash, player.GameTimeMilliseconds);
                hash = HashSingle(hash, player.BestTimeMilliseconds);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.NewReplay.CheckpointTimes.Count);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.NewReplay.NumberOfTrackMatrices);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.StartedLaps);
                hash = HashInt32(hash,
                    RacingGame.Graphics.Highscores.Submissions);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BestReplayReplacements);
                hash = HashSingle(hash,
                    RacingGameManager.Landscape.BestReplayLapTime);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BestReplayCheckpointCount);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BestReplayMatrixCount);
                hash = HashInt32(hash, BaseGame.UI.FadeupModes.Count);
                hash = HashInt32(hash,
                    RacingGame.Sounds.Sound.CheckpointBetterSounds);
                hash = HashInt32(hash,
                    RacingGame.Sounds.Sound.CheckpointWorseSounds);
                hash = HashInt32(hash, player.GameOver ? 1 : 0);
                hash = HashInt32(hash, player.Victory ? 1 : 0);
                output.WriteLine(
                    "RACESTATE{0:D3} hash={1:x16}", state++, hash);
            };

            advance(0, 16.0f);
            advance(1, 100.0f);
            advance(2, 110.0f);
            advance(0, 120.0f);
            advance(1, 80.0f);
            advance(2, 85.0f);
            advance(0, 90.0f);
            advance(1, 120.0f);
            advance(2, 130.0f);
            advance(0, 140.0f);
            advance(0, 16.0f);
            advance(0, 16.0f);

            for (int index = 0; index < BaseGame.UI.FadeupModes.Count; ++index)
            {
                hash = HashInt32(hash, BaseGame.UI.FadeupMilliseconds[index]);
                hash = HashInt32(hash, (int)BaseGame.UI.FadeupModes[index]);
            }
            hash = HashInt32(hash,
                RacingGame.Graphics.TextureFont.Entries.Count);
            ulong textHash = HashTextEntries(OffsetBasis);
            hash = HashInt32(hash, unchecked((int)textHash));
            hash = HashInt32(hash, unchecked((int)(textHash >> 32)));
            hash = HashInt32(hash, RacingGame.Sounds.Sound.VictorySounds);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.LoseSounds);
            hash = HashInt32(hash, RacingGame.Sounds.Sound.GearStops);

            output.WriteLine(
                "RACEOUTCOME laps={0} started={1} checkpoints={2} " +
                "replayMatrices={3} highscores={4} bestReplacements={5} " +
                "bestLapBits={6:x8} bestTimeBits={7:x8} bestCheckpoints={8} " +
                "bestMatrices={9} fadeups={10} betterSounds={11} " +
                "worseSounds={12} gameOver={13} victory={14} text={15} " +
                "victorySounds={16} loseSounds={17} gearStops={18}",
                player.CurrentLap,
                RacingGameManager.Landscape.StartedLaps,
                RacingGameManager.Landscape.NewReplay.CheckpointTimes.Count,
                RacingGameManager.Landscape.NewReplay.NumberOfTrackMatrices,
                RacingGame.Graphics.Highscores.Submissions,
                RacingGameManager.Landscape.BestReplayReplacements,
                BitConverter.SingleToUInt32Bits(
                    RacingGameManager.Landscape.BestReplayLapTime),
                BitConverter.SingleToUInt32Bits(player.BestTimeMilliseconds),
                RacingGameManager.Landscape.BestReplayCheckpointCount,
                RacingGameManager.Landscape.BestReplayMatrixCount,
                BaseGame.UI.FadeupModes.Count,
                RacingGame.Sounds.Sound.CheckpointBetterSounds,
                RacingGame.Sounds.Sound.CheckpointWorseSounds,
                player.GameOver ? 1 : 0,
                player.Victory ? 1 : 0,
                RacingGame.Graphics.TextureFont.Entries.Count,
                RacingGame.Sounds.Sound.VictorySounds,
                RacingGame.Sounds.Sound.LoseSounds,
                RacingGame.Sounds.Sound.GearStops);

            passed = player.CurrentLap == 2 &&
                RacingGameManager.Landscape.StartedLaps == 3 &&
                RacingGameManager.Landscape.NewReplay.CheckpointTimes.Count == 0 &&
                RacingGameManager.Landscape.NewReplay.NumberOfTrackMatrices == 0 &&
                RacingGame.Graphics.Highscores.Submissions == 3 &&
                RacingGameManager.Landscape.BestReplayReplacements == 2 &&
                Math.Abs(RacingGameManager.Landscape.BestReplayLapTime -
                    0.255f) < 0.000001f &&
                Math.Abs(player.BestTimeMilliseconds - 255.0f) < 0.000001f &&
                RacingGameManager.Landscape.BestReplayCheckpointCount == 3 &&
                RacingGameManager.Landscape.BestReplayMatrixCount == 1 &&
                BaseGame.UI.FadeupModes.Count == 9 &&
                RacingGame.Sounds.Sound.CheckpointBetterSounds == 3 &&
                RacingGame.Sounds.Sound.CheckpointWorseSounds == 3 &&
                player.GameOver && player.Victory &&
                RacingGame.Graphics.TextureFont.Entries.Count == 5 &&
                RacingGame.Sounds.Sound.VictorySounds == 1 &&
                RacingGame.Sounds.Sound.LoseSounds == 0 &&
                RacingGame.Sounds.Sound.GearStops == 1;
            return hash;
        }

        private static ulong ProbeCarPhysics(System.IO.TextWriter output)
        {
            RacingGameManager.Landscape.Reset();
            BaseGame.TotalTimeMilliseconds = 0.0f;
            CarPhysics.SetCarVariablesForCarType(
                CarPhysics.DefaultMaxSpeed * 1.05f,
                CarPhysics.DefaultCarMass * 1.015f,
                CarPhysics.DefaultMaxAccelerationPerSec * 0.85f);
            var player = new Player(Vector3.Zero);
            RacingGameManager.Player = player;
            RacingGameManager.InMenu = false;
            player.Reset();

            ulong hash = OffsetBasis;
            for (int frame = 0; frame < 600; frame++)
            {
                float moveFactor = frame % 113 == 0 ? 0.0005f :
                    frame % 127 == 0 ? 0.6f :
                    frame % 17 == 0 ? 0.033f : 0.016f;
                BaseGame.MoveFactorPerSecond = moveFactor;
                BaseGame.ElapsedTimeThisFrameInMilliseconds =
                    moveFactor * 1000.0f;
                BaseGame.TotalTimeMilliseconds +=
                    BaseGame.ElapsedTimeThisFrameInMilliseconds;
                SetInput(frame);
                player.Update();

                hash = HashVector3(hash, player.CarPosition);
                hash = HashVector3(hash, player.CarDirection);
                hash = HashVector3(hash, player.CarUpVector);
                hash = HashSingle(hash, player.Speed);
                float acceleration = player.Acceleration;
                hash = HashSingle(hash, acceleration);
                hash = HashSingle(hash, player.CarWheelPos);
                hash = HashMatrix(hash, player.CarRenderMatrix);
                hash = HashSingle(hash, player.GameTimeMilliseconds);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.NewReplay.NumberOfTrackMatrices);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BrakeTracks);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BrakeSounds);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.CrashSounds);
                output.WriteLine(
                    "CARFRAME{0:D3} hash={1:x16}", frame, hash);
            }
            return hash;
        }

        private static ulong ProbeCarCollisions(System.IO.TextWriter output)
        {
            RacingGameManager.Landscape.Reset();
            RacingGameManager.Landscape.RoadWidth = 3.0f;
            BaseGame.TotalTimeMilliseconds = 0.0f;
            CarPhysics.SetCarVariablesForCarType(
                CarPhysics.DefaultMaxSpeed * 1.05f,
                CarPhysics.DefaultCarMass * 1.015f,
                CarPhysics.DefaultMaxAccelerationPerSec * 0.85f);
            var player = new Player(Vector3.Zero);
            RacingGameManager.Player = player;
            player.Reset();

            ulong hash = OffsetBasis;
            for (int frame = 0; frame < 60; frame++)
            {
                BaseGame.MoveFactorPerSecond = 0.016f;
                BaseGame.ElapsedTimeThisFrameInMilliseconds =
                    frame < 2 ? 3000.0f : 16.0f;
                BaseGame.TotalTimeMilliseconds +=
                    BaseGame.ElapsedTimeThisFrameInMilliseconds;
                SetInput(0);
                if (frame >= 2)
                {
                    Input.KeyboardUpPressed = true;
                    Input.KeyboardLeftPressed = (frame & 1) == 0;
                    Input.Keyboard = new KeyboardState(Keys.W,
                        (frame & 1) == 0 ? Keys.A : Keys.W);
                }
                player.Update();
                hash = HashVector3(hash, player.CarPosition);
                hash = HashVector3(hash, player.CarDirection);
                hash = HashSingle(hash, player.Speed);
                hash = HashSingle(hash, player.CarWheelPos);
                hash = HashMatrix(hash, player.CarRenderMatrix);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BrakeTracks);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.BrakeSounds);
                hash = HashInt32(hash,
                    RacingGameManager.Landscape.CrashSounds);
                output.WriteLine(
                    "COLLISIONFRAME{0:D3} hash={1:x16}", frame, hash);
            }
            return hash;
        }
    }
}
