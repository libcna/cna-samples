// SPDX-License-Identifier: MS-PL

using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;

namespace RacingGame.GameScreens
{
    internal static class TrackSelection
    {
        public static int SelectedTrackNumber = 0;
    }
}

namespace RacingGame.Graphics
{
    internal sealed class Model
    {
    }

    internal static class Highscores
    {
        public static int Submissions;

        public static void SubmitHighscore(int level, int milliseconds)
        {
            Submissions++;
        }
    }

    internal static class UIRenderer
    {
        internal enum TimeFadeupMode
        {
            Normal,
            Minus,
            Plus,
        }
    }
}

namespace RacingGame.Helpers
{
    internal static class RandomHelper
    {
        public static Vector3 GetRandomVector3(float minimum, float maximum)
        {
            return Vector3.Zero;
        }
    }
}

namespace RacingGame.Shaders
{
    internal sealed class Placeholder
    {
    }
}

namespace RacingGame.Landscapes
{
    internal sealed class Placeholder
    {
    }
}

namespace RacingGame.Properties
{
    internal sealed class GameSettings
    {
        public static readonly GameSettings Default = new GameSettings();
        public float ControllerSensitivity = 0.5f;
    }
}

namespace RacingGame.Sounds
{
    internal static class Sound
    {
        internal enum Sounds
        {
            BrakeCurveMajor,
            BrakeCurveMinor,
            BrakeMajor,
            BrakeMinor,
            CheckpointBetter,
            CheckpointWorse,
        }

        public static Sounds GetBreakSoundType(
            float speed, float speedChange, float rotationChange)
        {
            bool inRotation = rotationChange >
                0.25f * RacingGame.GameLogic.Player.MaxRotationPerSec *
                RacingGame.GameLogic.BaseGame.MoveFactorPerSecond;
            Sounds result = inRotation
                ? Sounds.BrakeCurveMinor
                : Sounds.BrakeMinor;
            if (speed > 1.5f && System.Math.Abs(speedChange) >
                5.0f * RacingGame.GameLogic.BaseGame.MoveFactorPerSecond)
            {
                result = inRotation
                    ? Sounds.BrakeCurveMajor
                    : Sounds.BrakeMajor;
            }
            return result;
        }

        public static void PlayBrakeSound(Sounds type)
        {
            RacingGame.GameLogic.RacingGameManager.Landscape.BrakeSounds++;
        }

        public static void PlayCrashSound(bool totalCrash)
        {
            RacingGame.GameLogic.RacingGameManager.Landscape.CrashSounds++;
        }

        public static void Play(Sounds type)
        {
        }
    }
}

namespace RacingGame.Tracks
{
    internal static class GuardRail
    {
        public const float InsideRoadDistance = 0.5f;
    }
}

namespace RacingGame.GameLogic
{
    internal static class BaseGame
    {
        public static float MoveFactorPerSecond;
        public static float ElapsedTimeThisFrameInMilliseconds;
        public static float TotalTimeMilliseconds;
        public static Matrix ViewMatrix = Matrix.Identity;
        public const float Epsilon = 0.000001f;
        public static readonly UiStub UI = new UiStub();

        internal sealed class UiStub
        {
            public void AddTimeFadeupEffect(
                int milliseconds,
                RacingGame.Graphics.UIRenderer.TimeFadeupMode mode)
            {
            }
        }
    }

    internal static class Input
    {
        public static KeyboardState Keyboard = new KeyboardState();
        public static GamePadState GamePad = new GamePadState();
        public static bool IsGamePadConnected;
        public static bool KeyboardLeftPressed;
        public static bool KeyboardRightPressed;
        public static bool KeyboardUpPressed;
        public static bool KeyboardDownPressed;
        public static float MouseXMovement;
        public static float MouseYMovement;
        public static int MouseWheelDelta;
        public static bool MouseLeftButtonPressed;
        public static bool MouseRightButtonPressed;
        public static bool MouseMiddleButtonPressed;
        public static bool GamePadAPressed;
        public static bool GamePadBPressed;
        public static bool GamePadXPressed;
        public static bool GamePadYPressed;
        public static bool GamePadLeftPressed;
        public static bool GamePadRightPressed;
        public static bool GamePadUpPressed;
        public static bool GamePadDownPressed;
    }

    internal sealed class ReplayStub
    {
        public readonly List<float> CheckpointTimes = new List<float>();
        public readonly List<Matrix> Matrices = new List<Matrix>();
        public int NumberOfTrackMatrices => Matrices.Count;
        public void AddCarMatrix(Matrix matrix) => Matrices.Add(matrix);
    }

    internal sealed class LandscapeStub
    {
        private Vector3 lastCarPosition;

        public readonly ReplayStub NewReplay = new ReplayStub();
        public readonly List<int> CheckpointSegmentPositions = new List<int>();
        public int BrakeTracks;
        public int BrakeSounds;
        public int CrashSounds;
        public int StartedLaps;
        public int StartLightHash;
        public float RoadWidth = 100.0f;

        public void Reset()
        {
            NewReplay.CheckpointTimes.Clear();
            NewReplay.Matrices.Clear();
            CheckpointSegmentPositions.Clear();
            BrakeTracks = 0;
            BrakeSounds = 0;
            CrashSounds = 0;
            StartedLaps = 0;
            StartLightHash = 0;
            RoadWidth = 100.0f;
            lastCarPosition = Vector3.Zero;
        }

        public void StartNewLap()
        {
            StartedLaps++;
        }

        public void ReplaceStartLightObject(int state)
        {
            StartLightHash = unchecked(StartLightHash * 31 + state);
        }

        public void UpdateCarTrackPosition(
            Vector3 carPosition, ref int segment, ref float segmentPercent)
        {
            lastCarPosition = carPosition;
            segment = 0;
            segmentPercent = 0.0f;
        }

        public Matrix GetTrackPositionMatrix(
            int segment, float segmentPercent,
            out float roadWidth, out float nextRoadWidth)
        {
            roadWidth = RoadWidth;
            nextRoadWidth = RoadWidth;
            Matrix matrix = Matrix.Identity;
            matrix.Right = Vector3.UnitX;
            matrix.Up = Vector3.UnitZ;
            matrix.Forward = Vector3.UnitY;
            matrix.Translation = new Vector3(
                lastCarPosition.X, lastCarPosition.Y, 0.0f);
            return matrix;
        }

        public void AddBrakeTrack(CarPhysics car) => BrakeTracks++;
        public int CompareCheckpointTime(int index) => 0;
    }

    internal static class Replay
    {
        public const float TrackMatrixIntervals = 0.2f;
    }

    internal sealed class Player : ChaseCamera
    {
        public Player(Vector3 position) : base(position)
        {
        }

    }

    internal static class RacingGameManager
    {
        public static bool InMenu;
        public static bool InGame = true;
        public static readonly LandscapeStub Landscape = new LandscapeStub();
        public static Player Player;
    }
}
