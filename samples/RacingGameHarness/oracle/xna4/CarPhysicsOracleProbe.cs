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
            Input.MouseWheelDelta = 0;
            Input.MouseLeftButtonPressed = false;
            Input.MouseRightButtonPressed = false;
            Input.MouseMiddleButtonPressed = false;
            Input.GamePadAPressed = false;
            Input.GamePadBPressed = false;
            Input.GamePadXPressed = false;
            Input.GamePadYPressed = false;
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
