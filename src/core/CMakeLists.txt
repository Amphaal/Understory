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

#define lib
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")
target_compile_definitions(stb INTERFACE STB_IMAGE_IMPLEMENTATION)

#link
target_link_libraries(appCore INTERFACE stb)

###########################
## Deps : tinyProcessLib ##
###########################

#link
add_subdirectory(tiny-process-library)
target_link_libraries(appCore INTERFACE tiny-process-library)

##
## MODELS
##

target_link_libraries(appCore INTERFACE UnderStory::models)