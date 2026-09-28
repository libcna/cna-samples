# Missing / Differences from XNA 4.0 original

## Written for CNA — 2026-09-29

**This is not a Microsoft sample.** No sample in the XNA Game Studio 4.0 collection covers
achievements or leaderboards: XNA reserved them for licensed (XDK) titles, and the shipped Windows
assemblies throw `NotSupportedException` ("pro feature") from `LeaderboardWriter.GetLeaderboard`,
`Achievement.EarnedDateTime` and the rest. The CNA Gamer Services mission (CNA
`plans/plan_gamer_services_server.md`, priority 9) therefore needed a minimal XNA-shaped program of
its own to prove the service-backed behaviour end to end. This branch is not merged into `develop`.

The authoritative source is the C# program in `reference/`, written in the collection's style
against the public XNA 4.0 API only. It is compiled, warnings as errors, against the shipped XNA
Game Studio 4.0 Windows assemblies, and its font is built with the official content pipeline, by
`/rv/tmp/samples/GS-AchievementsLeaderboards/scripts/build-reference.sh` (retained snapshot
`xna4-reference/`, output `xna4-build/windows-reach/`, log `evidence/build-reference.log`):

| File | SHA-256 |
|---|---|
| `AchievementsLeaderboards.exe` | `8ca47cca5965a40432fcb1311ee71e299f5118cf7e45f64db76908fc617c5700` |
| `Font.xnb` (also `Content/Font.xnb` here) | `f4e11e0abaa75a7a41681f20b9d36a7c5d87f249013879a04aa4677aeaa1606d` |

The executable cannot be run meaningfully: its whole subject throws on Windows XNA. Compiling it is
what proves that every call the C++ port makes exists, with that shape, in XNA 4.0.

What the program does: `GamerServicesComponent`; `Guide.ShowSignIn(1, true)` for an online profile;
`SignedInGamer.BeginGetAchievements` with each `Achievement.GetPicture` loaded through
`Texture2D.FromStream`, drawn with name, gamer score, earned date, `HowToEarn`, and "Secret
achievement" for one that is not `DisplayBeforeEarned`; a five-second round in a `PlayerMatch`
session whose `WriteUnarbitratedLeaderboard` handler writes `Rating` and an int32 `Rounds` column to
`LeaderboardIdentity.Create(LeaderboardKey.BestScoreLifeTime, 0)` through `Gamer.LeaderboardWriter`;
`BeginAwardAchievement` for First Round, High Score (20 points in a round) and Veteran (three rounds in
one sitting); `LeaderboardReader.BeginRead` of an eight-row page with `BeginPageDown`/`BeginPageUp`.

The C++ port follows the C# line by line. Differences a C++ port needs, each at its site:

- An `IAsyncResult` is caller-owned in CNA and releasing a pending one cancels its callback, so the
  game holds the one outstanding operation and, in its callback, parks it until the next `Update`
  (the garbage collector's job in C#; destroying it inside its own callback would free the
  operation that is calling).
- The session returned by `EndCreate` is owned by a `std::unique_ptr` and deleted after `Dispose`.
- `Achievement.GetPicture`'s stream is owned by a `std::unique_ptr` for the C# `using`.

Porting found three CNA defects, fixed in CNA rather than worked around here: `SignedInGamers[PlayerIndex]`
indexed by position instead of finding the player (GS-004m), service achievements reported
`EarnedDateTime` in UTC (GS-005e), and earlier in the mission lobby readiness and gamer order
(GS-007i, GS-007l) were found by SAMPLE-075.

Run it against a CNA account service configured outside the game (see CNA
`docs/gamer-services-server.md`); the title needs the three achievements and the
`BestScoreLifeTime` board with an int32 `Rounds` column, which the acceptance script shows how to
provision:

```
CNA_GAMER_SERVICES_ENDPOINT=https://host:port/cna/v1 CNA_GAME_ID=scores ./AchievementsLeaderboards_cna_samples
```

Accepted by `/rv/tmp/samples/GS-AchievementsLeaderboards/scripts/capture-cna-achievements-leaderboards.py`
(evidence `evidence/gs-achievements-leaderboards-20260929/`): its own verified-TLS service with the
three achievements (pictures are original PNGs drawn by the script), the board with nine seeded rows
so it has a second page, and two accounts; three Release processes one after another on a private
Xvfb display, driven only through the game's own input. ScoreAda signs in through the Guide, sees
First Round and High Score locked and Veteran secret, plays a 26-point round (First Round and High
Score earned, fourth on the board) and two 3-point rounds (Veteran earned; the board keeps her best
row). ScoreBo plays an 8-point round: First Round earned, High Score locked, Veteran still secret; he
is ninth, on page two, reached with Down and left with Up. A new ScoreAda process still shows all
three earned. Every process exits with code 0.
