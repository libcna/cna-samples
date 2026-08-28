cna_add_sample(Graphics3D
    SOURCES
        src/Graphics3DSampleGame.cpp
        src/Animation/Animation.cpp
        src/Buttons/Checkbox.cpp
        src/Buttons/Clickable.cpp
        src/Models/Spaceship.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The upstream project references System.Xml.Linq: AnimationDef.xml is a <None Include> copied
# beside the content, and GameMain.cs parses it with XDocument.Load at run time.
target_link_libraries(Graphics3D_cna_samples PRIVATE SharpRuntime::Xml.Linq)
