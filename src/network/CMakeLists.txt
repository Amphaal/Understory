#############
## Sources ##
#############

add_library(appNetwork STATIC 
    uPnPHandler.hpp
)
add_library(UnderStory::network ALIAS appNetwork)

###################
## Deps : spdlog ##
###################

include(FetchContent)

#fetch
FetchContent_Declare(spdlog
    GIT_REPOSITORY  "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"    
)
FetchContent_Populate(spdlog)

#link
target_include_directories(appNetwork PRIVATE ${spdlog_SOURCE_DIR}/include)

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

######################
## Deps : capnproto ##
######################

#fetch
find_package(CapnProto REQUIRED)

capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS 
    payloads/Payload.capnp
)

target_sources(appNetwork PRIVATE ${CAPNP_SRCS})

target_include_directories(appNetwork PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(appNetwork PRIVATE CapnProto::capnp-rpc)
