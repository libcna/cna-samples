#region File Description
//-----------------------------------------------------------------------------
// Program.cs
//
// An XNA Game Studio 4.0 program written for CNA; see AchievementsLeaderboardsGame.cs.
//-----------------------------------------------------------------------------
#endregion

namespace AchievementsLeaderboards
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            using (AchievementsLeaderboardsGame game = new AchievementsLeaderboardsGame())
            {
                game.Run();
            }
        }
    }
}
