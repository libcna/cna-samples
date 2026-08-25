cna_add_sample(Spacewar
    SOURCES
        src/Asteroid.cpp
        src/BasicEffectShape.cpp
        src/Camera.cpp
        src/EvolvedShape.cpp
        src/EvolvedScreen.cpp
        src/EvolvedBackdrop.cpp
        src/EvolvedSun.cpp
        src/Font.cpp
        src/FullScreenSplash.cpp
        src/GamePadHelper.cpp
        src/GamePads.cpp
        src/Keymap.cpp
        src/Particle.cpp
        src/Particles.cpp
        src/Projectile.cpp
        src/Projectiles.cpp
        src/RetroProjectiles.cpp
        src/RetroScreen.cpp
        src/RetroShip.cpp
        src/RetroStarfield.cpp
        src/RetroSun.cpp
        src/Program.cpp
        src/SceneItem.cpp
        src/Screen.cpp
        src/SelectionScreen.cpp
        src/Settings.cpp
        src/Shape.cpp
        src/Ship.cpp
        src/ShipUpgradeScreen.cpp
        src/Sound.cpp
        src/SpacewarSceneItem.cpp
        src/SpacewarScreen.cpp
        src/SpacewarGame.cpp
        src/TitleScreen.cpp
        src/Sun.cpp
        src/VectorShape.cpp
        src/VictoryScreen.cpp
        src/XInputHelper.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(Spacewar_cna_samples PRIVATE SharpRuntime::Xml.Linq)

if(EMSCRIPTEN)
    target_link_options(Spacewar_cna_samples PRIVATE
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/settings.xml@/settings.xml"
    )
else()
    add_custom_command(TARGET Spacewar_cna_samples POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/settings.xml"
            "$<TARGET_FILE_DIR:Spacewar_cna_samples>/settings.xml"
        VERBATIM
    )
endif()
