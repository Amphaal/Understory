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

#################
## Deps : asio ##
#################

find_file(ASIO_HEADER asio.hpp)
add_library(asio INTERFACE IMPORTED)
target_sources(asio INTERFACE ${ASIO_HEADER})

target_link_libraries(appNetwork INTERFACE asio)

if(WIN32)
    target_link_libraries(appNetwork INTERFACE wsock32)
endif()

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