
#################
## Deps : asio ##
#################

find_file(ASIO_HEADER asio.hpp)
add_library(asio INTERFACE IMPORTED)
target_include_directories(asio INTERFACE ${ASIO_HEADER})
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(asio INTERFACE WIN32_LEAN_AND_MEAN) # prevents <winsock2.h> warnings
endif()

#########
## Lib ##
#########

add_library(appNetwork INTERFACE)
add_library(UnderStory::network ALIAS appNetwork)

target_link_libraries(appNetwork INTERFACE UnderStory::base)
target_link_libraries(appNetwork INTERFACE UnderStory::models)

#############
## Sources ##
#############

target_sources(appNetwork INTERFACE  
    HTTPDownloader.cpp
    AtomicQueue.cpp
    Marshaller.cpp
    IClientImpl.cpp
    IPayloadHandler.cpp
    IPayloadReceiver.cpp
    IPayloadSender.cpp
    SpawnedSocket.cpp
    ClientSocket.cpp
    Server.cpp
)

# link
target_link_libraries(appNetwork INTERFACE asio)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(appNetwork INTERFACE ASIO_DISABLE_IOCP) # prevent usage of wsock32
    target_link_libraries(appNetwork INTERFACE ws2_32) #WSA functions !
endif()
