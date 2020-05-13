#############
## Sources ##
#############

add_library(appNetwork INTERFACE)
target_sources(appNetwork INTERFACE 
    uPnPHandler.hpp
    USClient.hpp
    USServer.hpp
)
add_library(UnderStory::network ALIAS appNetwork)

###################
## Deps : ZeroMQ ##
###################

find_package(ZeroMQ REQUIRED)

target_link_libraries(appNetwork INTERFACE libzmq)

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
target_link_libraries(appNetwork INTERFACE spdlog)

######################
## Deps : miniupnpc ##
######################

#find miniupnpc
find_package(miniupnpc REQUIRED)

#link
target_link_libraries(appNetwork INTERFACE miniupnpc)

#WSA functions !
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(appNetwork INTERFACE ws2_32) 
endif()