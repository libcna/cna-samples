cna_add_sample(Yacht
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
    GAMER_SERVICES
)

# The online half talks to the game server over SOAP and is pushed to over a notification
# channel, and it is a phone game, so it needs the phone application-lifecycle service. None of
# that is wanted by any other sample, so it is linked here rather than pulled into every sample
# through CNA.
target_link_libraries(Yacht_cna_samples PRIVATE
    SharpRuntime::ServiceModel
    SharpRuntime::IO.IsolatedStorage
    cna_phone)

# The second product. The original ships a console host beside the phone client, and it is a
# separate executable there too -- Server.exe, from Yacht Server.sln. It needs no graphics, so
# it links the service model and the push sender rather than CNA.
add_executable(YachtServer_cna_samples
    src/Server/Program.cpp
    src/YachtServices/YachtService.cpp
)
target_compile_definitions(YachtServer_cna_samples PRIVATE YACHT_SERVER)
target_include_directories(YachtServer_cna_samples PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(YachtServer_cna_samples PRIVATE
    SharpRuntime::ServiceModel
    SharpRuntime::Xml
    SharpRuntime::Xml.Serialization
    cna_phone)
