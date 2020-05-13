SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)

list(APPEND CMAKE_PREFIX_PATH 
    "/usr/local/opt"
)

SET(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    "/usr/local/opt/protobuf/include"
    "/usr/local/opt/miniupnpc/include"
)

SET(CMAKE_BUILD_TYPE Debug)