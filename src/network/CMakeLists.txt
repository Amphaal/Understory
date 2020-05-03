#############
## Sources ##
#############

add_library(appNetwork STATIC 
    uPnPHandler.hpp
    payloads/Payloads.h
    payloads/_Payload.hpp
    payloads/specialized/Reset.hpp
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

##################
## Deps : cista ##
##################

#fetch
FetchContent_Declare(cista
    GIT_REPOSITORY  "https://github.com/felixguendling/cista.git"
)
FetchContent_MakeAvailable(cista)

#link
target_link_libraries(appNetwork PUBLIC cista)