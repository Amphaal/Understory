add_library(appCore INTERFACE)
target_sources(appCore INTERFACE
    AssetIntegrator.hpp
    FileIntegrator.hpp
    UpdateChecker.hpp
    hash_fnv1a.hpp
)
add_library(UnderStory::core ALIAS appCore)

target_link_libraries(appCore INTERFACE UnderStory::network)

################
## Deps : stb ##
################

#link
target_link_libraries(appCore INTERFACE stb)

##
##
##

#fetch
FetchContent_Declare(tinyProcessLib
    GIT_REPOSITORY  "https://gitlab.com/eidheim/tiny-process-library.git"
)
FetchContent_MakeAvailable(tinyProcessLib)

#link
target_link_libraries(appCore INTERFACE tiny-process-library)

##
## MODELS
##

target_link_libraries(appCore INTERFACE UnderStory::models)