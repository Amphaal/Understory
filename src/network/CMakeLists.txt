#############
## Sources ##
#############

add_library(appNetwork STATIC 
    uPnPHandler.hpp
)
set_target_properties(appNetwork PROPERTIES LINKER_LANGUAGE CXX)
add_library(UnderStory::network ALIAS appNetwork)

###################
## Deps : spdlog ##
###################

#using header-only version to prevent linking errors with static lib version

include(FetchContent)

#fetch
FetchContent_Declare(spdlog
    GIT_REPOSITORY  "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"    
)
FetchContent_Populate(spdlog)

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE ${spdlog_SOURCE_DIR}/include)

#link
target_link_libraries(appNetwork PRIVATE spdlog)

######################
## Deps : miniupnpc ##
######################

#find miniupnpc
find_package(miniupnpc REQUIRED)

#link
target_link_libraries(appNetwork PUBLIC miniupnpc)

#WSA functions !
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(appNetwork PRIVATE ws2_32) 
endif()