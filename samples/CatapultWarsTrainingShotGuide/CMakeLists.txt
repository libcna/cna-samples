cna_add_sample(CatapultWarsTrainingShotGuide
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
        src/Screens/GameplayScreen.cpp
        src/Screens/PauseScreen.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(CatapultWarsTrainingShotGuide_cna_samples PRIVATE
    SharpRuntime::IO.IsolatedStorage
    SharpRuntime::Xml.Linq
)
