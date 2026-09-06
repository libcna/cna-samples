cna_add_sample(ShipGame
    SOURCES
        src/GameManager.cpp
        src/Graphics/AnimSpriteManager.cpp
        src/Graphics/Powerup.cpp
        src/Graphics/PowerupManager.cpp
        src/Graphics/Projectile.cpp
        src/Graphics/ProjectileManager.cpp
        src/PlayerShip.cpp
        src/Program.cpp
        src/Screens/ScreenEnd.cpp
        src/Screens/ScreenGame.cpp
        src/Screens/ScreenHelp.cpp
        src/Screens/ScreenIntro.cpp
        src/Screens/ScreenLevel.cpp
        src/Screens/ScreenManager.cpp
        src/Screens/ScreenPlayer.cpp
        src/ShipGame.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
    # ShipGame reads its level/light/ship XML through System.IO with a literal
    # lowercase "content/..." path, and its models and textures through
    # ContentManager with RootDirectory "Content". On Windows those name one
    # directory; here the strict reader is System.IO, so the deployed directory
    # takes its spelling. ContentManager resolves the other spelling itself.
    CONTENT_NAME content
)

target_link_libraries(ShipGame_cna_samples PRIVATE
    SharpRuntime::Console
    SharpRuntime::Xml.Serialization
)
