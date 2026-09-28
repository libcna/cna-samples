#region File Description
//-----------------------------------------------------------------------------
// AchievementsLeaderboardsGame.cs
//
// An XNA Game Studio 4.0 program written for CNA: no Microsoft sample covers
// achievements or leaderboards, which XNA reserved for licensed titles. It uses
// only the public XNA 4.0 API and compiles against the shipped assemblies.
//-----------------------------------------------------------------------------
#endregion

#region Using Statements
using System;
using System.Collections.Generic;
using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Net;
#endregion

namespace AchievementsLeaderboards
{
    /// <summary>
    /// Plays short rounds in a PlayerMatch session. When a round ends the score is
    /// written to a leaderboard, achievements are awarded, and both lists are read
    /// back and drawn.
    /// </summary>
    public class AchievementsLeaderboardsGame : Microsoft.Xna.Framework.Game
    {
        #region Constants

        const int RoundSeconds = 5;
        const int HighScorePoints = 20;
        const int VeteranRounds = 3;
        const int PageSize = 8;

        static readonly LeaderboardIdentity BestScore =
            LeaderboardIdentity.Create(LeaderboardKey.BestScoreLifeTime, 0);

        #endregion

        #region Fields

        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;
        SpriteFont font;

        NetworkSession networkSession;
        bool roundOver;
        int score;
        int roundsPlayed;
        TimeSpan roundTimeLeft;

        AchievementCollection achievements;
        Dictionary<string, Texture2D> pictures = new Dictionary<string, Texture2D>();
        LeaderboardReader leaderboard;
        Queue<string> awards = new Queue<string>();

        // True while an asynchronous operation is outstanding.
        bool busy;
        string message = string.Empty;

        KeyboardState currentKeyboardState;
        GamePadState currentGamePadState;
        KeyboardState previousKeyboardState;
        GamePadState previousGamePadState;

        #endregion

        #region Initialization

        public AchievementsLeaderboardsGame()
        {
            graphics = new GraphicsDeviceManager(this);

            graphics.PreferredBackBufferWidth = 1280;
            graphics.PreferredBackBufferHeight = 720;

            Content.RootDirectory = "Content";

            Components.Add(new GamerServicesComponent(this));
        }


        /// <summary>
        /// Load your content.
        /// </summary>
        protected override void LoadContent()
        {
            spriteBatch = new SpriteBatch(GraphicsDevice);

            font = Content.Load<SpriteFont>("Font");
        }


        /// <summary>
        /// Releases the pictures and any session still open.
        /// </summary>
        protected override void UnloadContent()
        {
            foreach (Texture2D picture in pictures.Values)
                picture.Dispose();

            if (networkSession != null)
                networkSession.Dispose();
        }

        #endregion

        #region Update

        /// <summary>
        /// Allows the game to run logic.
        /// </summary>
        protected override void Update(GameTime gameTime)
        {
            HandleInput();

            if (networkSession == null)
            {
                // Only listen for menu input when the Guide is not visible.
                if (!Guide.IsVisible)
                    UpdateMenu();
            }
            else
            {
                UpdateRound(gameTime);
            }

            base.Update(gameTime);
        }


        /// <summary>
        /// Menu screen: signs in, starts rounds and pages through the leaderboard.
        /// </summary>
        void UpdateMenu()
        {
            if (IsPressed(Keys.Escape, Buttons.Back))
            {
                Exit();
                return;
            }

            SignedInGamer gamer = Gamer.SignedInGamers[PlayerIndex.One];

            if (gamer == null)
            {
                // Achievements and leaderboards belong to an online profile.
                if (IsPressed(Keys.Enter, Buttons.A))
                {
                    try
                    {
                        Guide.ShowSignIn(1, true);
                    }
                    catch (GamerServicesNotAvailableException error)
                    {
                        message = error.Message;
                    }
                }

                return;
            }

            if (busy)
                return;

            if (achievements == null)
            {
                Refresh(gamer);
            }
            else if (IsPressed(Keys.Enter, Buttons.A))
            {
                StartRound();
            }
            else if (IsPressed(Keys.Down, Buttons.DPadDown) &&
                     leaderboard != null && leaderboard.CanPageDown)
            {
                busy = true;
                leaderboard.BeginPageDown(PageFinished, false);
            }
            else if (IsPressed(Keys.Up, Buttons.DPadUp) &&
                     leaderboard != null && leaderboard.CanPageUp)
            {
                busy = true;
                leaderboard.BeginPageUp(PageFinished, true);
            }
        }


        /// <summary>
        /// Round screen: counts presses until the time runs out.
        /// </summary>
        void UpdateRound(GameTime gameTime)
        {
            networkSession.Update();

            if (roundOver)
            {
                // The leaderboard was written when the game ended; the session is done.
                roundOver = false;
                networkSession.Dispose();
                networkSession = null;

                roundsPlayed++;
                message = "Round over: " + score + " points.";

                SignedInGamer gamer = Gamer.SignedInGamers[PlayerIndex.One];

                if (gamer != null)
                {
                    awards.Enqueue("FirstRound");

                    if (score >= HighScorePoints)
                        awards.Enqueue("HighScore");

                    if (roundsPlayed >= VeteranRounds)
                        awards.Enqueue("Veteran");

                    AwardNext(gamer);
                }
                return;
            }

            if (networkSession.SessionState != NetworkSessionState.Playing)
                return;

            if (IsPressed(Keys.Space, Buttons.A))
                score++;

            roundTimeLeft -= gameTime.ElapsedGameTime;

            if (roundTimeLeft <= TimeSpan.Zero && networkSession.IsHost)
                networkSession.EndGame();
        }


        /// <summary>
        /// Creates the session a round is played in.
        /// </summary>
        void StartRound()
        {
            busy = true;
            message = "Creating session...";

            NetworkSession.BeginCreate(NetworkSessionType.PlayerMatch, 1, 2,
                                       SessionCreated, null);
        }


        /// <summary>
        /// Starts the game once the session exists.
        /// </summary>
        void SessionCreated(IAsyncResult result)
        {
            try
            {
                networkSession = NetworkSession.EndCreate(result);

                networkSession.GameStarted += GameStarted;
                networkSession.GameEnded += GameEnded;
                networkSession.WriteUnarbitratedLeaderboard += WriteLeaderboard;

                networkSession.StartGame();

                message = string.Empty;
            }
            catch (Exception error)
            {
                message = error.Message;
            }

            busy = false;
        }


        void GameStarted(object sender, GameStartedEventArgs e)
        {
            score = 0;
            roundTimeLeft = TimeSpan.FromSeconds(RoundSeconds);
        }


        void GameEnded(object sender, GameEndedEventArgs e)
        {
            roundOver = true;
        }


        /// <summary>
        /// Writes this gamer's round to the leaderboard while the game ends.
        /// </summary>
        void WriteLeaderboard(object sender, WriteLeaderboardsEventArgs e)
        {
            LeaderboardEntry entry = e.Gamer.LeaderboardWriter.GetLeaderboard(BestScore);

            entry.Rating = score;
            entry.Columns.SetValue("Rounds", roundsPlayed + 1);
        }


        /// <summary>
        /// Awards the queued achievements one at a time, then reads both lists again.
        /// </summary>
        void AwardNext(SignedInGamer gamer)
        {
            busy = true;

            if (awards.Count > 0)
                gamer.BeginAwardAchievement(awards.Dequeue(), AwardFinished, gamer);
            else
                Refresh(gamer);
        }


        void AwardFinished(IAsyncResult result)
        {
            SignedInGamer gamer = (SignedInGamer)result.AsyncState;

            try
            {
                gamer.EndAwardAchievement(result);
            }
            catch (Exception error)
            {
                message = error.Message;
            }

            AwardNext(gamer);
        }


        /// <summary>
        /// Reads the gamer's achievements, then the first leaderboard page.
        /// </summary>
        void Refresh(SignedInGamer gamer)
        {
            busy = true;

            gamer.BeginGetAchievements(AchievementsRead, gamer);
        }


        void AchievementsRead(IAsyncResult result)
        {
            SignedInGamer gamer = (SignedInGamer)result.AsyncState;

            try
            {
                AchievementCollection list = gamer.EndGetAchievements(result);

                foreach (Achievement achievement in list)
                {
                    if (!pictures.ContainsKey(achievement.Key))
                    {
                        using (Stream stream = achievement.GetPicture())
                        {
                            pictures[achievement.Key] = Texture2D.FromStream(GraphicsDevice, stream);
                        }
                    }
                }

                if (achievements != null)
                    achievements.Dispose();

                achievements = list;

                LeaderboardReader.BeginRead(BestScore, 0, PageSize, LeaderboardRead, null);
            }
            catch (Exception error)
            {
                message = error.Message;
                busy = false;
            }
        }


        void LeaderboardRead(IAsyncResult result)
        {
            try
            {
                LeaderboardReader reader = LeaderboardReader.EndRead(result);

                if (leaderboard != null)
                    leaderboard.Dispose();

                leaderboard = reader;
            }
            catch (Exception error)
            {
                message = error.Message;
            }

            busy = false;
        }


        void PageFinished(IAsyncResult result)
        {
            try
            {
                if ((bool)result.AsyncState)
                    leaderboard.EndPageUp(result);
                else
                    leaderboard.EndPageDown(result);
            }
            catch (Exception error)
            {
                message = error.Message;
            }

            busy = false;
        }

        #endregion

        #region Draw

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            spriteBatch.Begin();

            if (networkSession != null)
                DrawRound();
            else
                DrawMenu();

            spriteBatch.DrawString(font, message, new Vector2(60, 640), Color.White);

            spriteBatch.End();

            base.Draw(gameTime);
        }


        void DrawRound()
        {
            if (networkSession.SessionState != NetworkSessionState.Playing)
                return;

            string text = "Press Space or A as often as you can!\n\n" +
                          "Score: " + score + "\n" +
                          "Time: " + Math.Max(0, (int)Math.Ceiling(roundTimeLeft.TotalSeconds));

            spriteBatch.DrawString(font, text, new Vector2(60, 60), Color.White);
        }


        void DrawMenu()
        {
            SignedInGamer gamer = Gamer.SignedInGamers[PlayerIndex.One];

            if (gamer == null)
            {
                spriteBatch.DrawString(font, "Enter / A: sign in", new Vector2(60, 60), Color.White);
                return;
            }

            spriteBatch.DrawString(font, gamer.Gamertag, new Vector2(60, 20), Color.Yellow);

            DrawAchievements(new Vector2(60, 80));
            DrawLeaderboard(new Vector2(700, 80));

            spriteBatch.DrawString(font, "Enter / A: play a round   Up/Down: page   Esc / Back: exit",
                                   new Vector2(60, 680), Color.LightGray);
        }


        void DrawAchievements(Vector2 position)
        {
            spriteBatch.DrawString(font, "Achievements", position, Color.White);

            if (achievements == null)
                return;

            position.Y += 50;

            foreach (Achievement achievement in achievements)
            {
                Texture2D picture;

                if (pictures.TryGetValue(achievement.Key, out picture))
                {
                    Color tint = achievement.IsEarned ? Color.White : Color.Gray;

                    spriteBatch.Draw(picture, new Rectangle((int)position.X, (int)position.Y, 48, 48), tint);
                }

                string title, detail;

                if (achievement.IsEarned)
                {
                    title = achievement.Name + " (" + achievement.GamerScore + ")";
                    detail = "Earned " + achievement.EarnedDateTime.ToString("yyyy-MM-dd");
                }
                else if (achievement.DisplayBeforeEarned)
                {
                    title = achievement.Name + " (" + achievement.GamerScore + ")";
                    detail = achievement.HowToEarn;
                }
                else
                {
                    title = "Secret achievement";
                    detail = "Keep playing";
                }

                spriteBatch.DrawString(font, title, position + new Vector2(60, 0), Color.White);
                spriteBatch.DrawString(font, detail, position + new Vector2(60, 24), Color.LightGray);

                position.Y += 70;
            }
        }


        void DrawLeaderboard(Vector2 position)
        {
            if (leaderboard == null)
                return;

            spriteBatch.DrawString(font, "Best scores (" + leaderboard.TotalLeaderboardSize + ")",
                                   position, Color.White);

            position.Y += 50;

            int rank = leaderboard.PageStart + 1;

            foreach (LeaderboardEntry entry in leaderboard.Entries)
            {
                string text = rank + ". " + entry.Gamer.Gamertag + "  " + entry.Rating +
                              "  (round " + entry.Columns.GetValueInt32("Rounds") + ")";

                spriteBatch.DrawString(font, text, position, Color.White);

                position.Y += 30;
                rank++;
            }
        }

        #endregion

        #region Handle Input

        void HandleInput()
        {
            previousKeyboardState = currentKeyboardState;
            previousGamePadState = currentGamePadState;

            currentKeyboardState = Keyboard.GetState();
            currentGamePadState = GamePad.GetState(PlayerIndex.One);
        }


        bool IsPressed(Keys key, Buttons button)
        {
            return (currentKeyboardState.IsKeyDown(key) &&
                    previousKeyboardState.IsKeyUp(key)) ||
                   (currentGamePadState.IsButtonDown(button) &&
                    previousGamePadState.IsButtonUp(button));
        }

        #endregion
    }
}
