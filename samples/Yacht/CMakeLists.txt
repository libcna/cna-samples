cna_add_sample(Yacht
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The online half talks to the game server over SOAP and is pushed to over a notification
# channel, and it is a phone game, so it needs the phone application-lifecycle service. None of
# that is wanted by any other sample, so it is linked here rather than pulled into every sample
# through CNA.
target_link_libraries(Yacht_cna_samples PRIVATE SharpRuntime::ServiceModel cna_phone)
