add_library(appNetwork INTERFACE)
add_library(UnderStory::network ALIAS appNetwork)

target_link_libraries(appNetwork INTERFACE UnderStory::base)

#############
## Sources ##
#############

target_sources(appNetwork INTERFACE 
    uPnPHdlr.hpp
    USClient.hpp
    USServer.hpp
    SocketHelper.hpp
)

###################
## Deps : ZeroMQ ##
###################

find_package(ZeroMQ REQUIRED)

target_link_libraries(appNetwork INTERFACE libzmq)

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