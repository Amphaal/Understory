add_library(appCore INTERFACE)
target_sources(appCore INTERFACE
    AssetIntegrator.hpp
    FileIntegrator.hpp
    Defaults.hpp
    hash_fnv1a.hpp
)
add_library(UnderStory::core ALIAS appCore)

################
## Deps : stb ##
################

#link
target_link_libraries(appCore INTERFACE stb)

##
## MODELS
##

target_link_libraries(appCore INTERFACE UnderStory::models)