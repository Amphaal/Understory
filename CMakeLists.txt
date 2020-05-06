# use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message("== Using CCache ! ==")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# default configuration
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE INTERNAL "")

# pre-startup
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
message(STATUS "Using toolchain file: [${CMAKE_TOOLCHAIN_FILE}].")

# project setup
project(UnderStory
    VERSION 0.0.1
    DESCRIPTION "An intuitive Pen & Paper experience"
    HOMEPAGE_URL "https://github.com/Amphaal/Understory"
    LANGUAGES C CXX
)

include(FetchContent)

################################
## CPP Compiler Configuration ##
################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

#################################
## define bundle configuration ##
#################################

# configure header with basic app informations
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/_understory.h 
    understory.h
)

#for generated config header
include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# define _DEBUG markup if in DEBUG config
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>) 

################
## Deps : stb ##
################

#fetch
FetchContent_Declare(stb
    GIT_REPOSITORY  "https://github.com/nothings/stb.git"
)
FetchContent_Populate(stb)

#define lib
add_library(stb STATIC "${stb_SOURCE_DIR}/stb_image.h")
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
target_compile_definitions(stb INTERFACE STB_IMAGE_IMPLEMENTATION)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)

###########
## Tests ##
###########

add_subdirectory(src/network)
add_subdirectory(src/models)
add_subdirectory(src/core)

#dont build tests if included as submodule
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
else()
    add_subdirectory(src/app)
endif()