add_library(appCore STATIC 
    AssetIntegrator.hpp
    FileIntegrator.hpp
    Defaults.hpp
    hash_fnv1a.hpp
)
set_target_properties(appCore PROPERTIES LINKER_LANGUAGE CXX)
add_library(UnderStory::core ALIAS appCore)

################
## Deps : stb ##
################

#link
target_link_libraries(appCore PUBLIC stb)

##
## MODELS
##

target_link_libraries(appCore PRIVATE UnderStory::models)