add_library(appCore INTERFACE)
target_sources(appCore INTERFACE
    AssetIntegrator.hpp
    FileIntegrator.hpp
    UpdateChecker.hpp
    hash_fnv1a.hpp
)
add_library(UnderStory::core ALIAS appCore)

target_link_libraries(appCore INTERFACE appBase)
target_link_libraries(appCore INTERFACE appNetwork)

################
## Deps : stb ##
################

#link
target_link_libraries(appCore INTERFACE stb)

##
## MODELS
##

target_link_libraries(appCore INTERFACE UnderStory::models)