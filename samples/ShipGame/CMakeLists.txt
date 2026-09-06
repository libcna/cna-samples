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
)

target_link_libraries(ShipGame_cna_samples PRIVATE
    SharpRuntime::Console
    SharpRuntime::Xml.Serialization
)
