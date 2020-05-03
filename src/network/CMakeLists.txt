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

#find spdlog
find_package(spdlog REQUIRED)

#hardcore definition to handle external reference
target_compile_definitions(spdlog::spdlog INTERFACE SPDLOG_FMT_EXTERNAL)

#link
target_link_libraries(appNetwork PUBLIC spdlog::spdlog)

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
target_sources(appNetwork PRIVATE
    payloads/Payload.capnp
)