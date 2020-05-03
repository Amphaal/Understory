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

# define build type if any
if(NOT DEFINED APP_BUNDLE_TYPE)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(APP_BUNDLE_TYPE "WIN32") 
    endif()
endif()

# configure header with basic app informations
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/_understory.h 
    understory.h
)

#############
## Sources ##
#############

add_library(appCore STATIC)
add_library(UnderStory::core ALIAS appCore)

target_compile_definitions(appCore PRIVATE $<$<CONFIG:Debug>:_DEBUG>)  # define _DEBUG markup if in DEBUG config

target_sources(appCore PRIVATE
    src/app/Application.hpp
    src/app/Utility.hpp
    src/network/uPnPHandler.hpp
)

#for generated config header
target_include_directories(appCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

###############
## Resources ##
###############

include(CMakeRC)

# find resources
file(GLOB_RECURSE APP_RESOURCES_FILES "resources/**")
cmrc_add_resource_library(appResources
    WHENCE resources
    ${APP_RESOURCES_FILES}
)

target_link_libraries(appCore PUBLIC appResources)

###################
## Deps : spdlog ##
###################

#find spdlog
find_package(spdlog REQUIRED)

#hardcore definition to handle external reference
target_compile_definitions(spdlog::spdlog INTERFACE SPDLOG_FMT_EXTERNAL)

#link
target_link_libraries(appCore PUBLIC spdlog::spdlog)

######################
## Deps : miniupnpc ##
######################

#find miniupnpc
find_package(miniupnpc REQUIRED)

#link
target_link_libraries(appCore PRIVATE miniupnpc)

#WSA functions !
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(appCore PRIVATE ws2_32) 
endif()

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

#link
target_link_libraries(appCore PUBLIC stb)

##################
## Deps : cista ##
##################

#fetch
FetchContent_Declare(cista
    GIT_REPOSITORY  "https://github.com/felixguendling/cista.git"
)
FetchContent_MakeAvailable(cista)

#link
target_link_libraries(appCore PUBLIC cista)

###################
## Deps : Magnum ##
###################

find_package(Magnum REQUIRED GL GlfwApplication)
find_package(Corrade REQUIRED Main)

target_link_libraries(appCore PUBLIC
    Corrade::Main    
    Magnum::Application
    Magnum::GL
)

###########
## Tests ##
###########

#dont build tests if included as submodule
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
else()
    # define app executable
    add_executable(appExec ${APP_BUNDLE_TYPE} src/main.cpp)
    add_executable(UnderStory::executable ALIAS appExec)
    set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")  # define executable name
    target_link_libraries(appExec PRIVATE appCore)
endif()