add_library(appNetwork INTERFACE)
add_library(UnderStory::network ALIAS appNetwork)

target_link_libraries(appNetwork INTERFACE UnderStory::base)
target_link_libraries(appNetwork INTERFACE UnderStory::models)

#############
## Sources ##
#############

target_sources(appNetwork INTERFACE 
    USClient.hpp
    USServer.hpp
    SocketHelper.hpp
    HTTPDownloader.hpp
    Marshaller.hpp
)

#################
## Deps : asio ##
#################

find_file(ASIO_HEADER asio.hpp)
add_library(asio INTERFACE IMPORTED)
target_sources(asio INTERFACE ${ASIO_HEADER})

IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(asio INTERFACE WIN32_LEAN_AND_MEAN) # prevents <winsock2.h> warnings
endif()

target_link_libraries(appNetwork INTERFACE asio)
#WSA functions !
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(appNetwork INTERFACE ws2_32) 
endif()
