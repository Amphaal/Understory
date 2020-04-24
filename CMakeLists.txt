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
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE INTERNAL "")

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/understory.h
)

######################################
## define executables and libraries ##
######################################

# define app executable
add_executable(appExec ${APP_BUNDLE_TYPE})
target_include_directories(appExec PRIVATE
    "include"
    "libs"
)
set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")  # define executable name
target_compile_definitions(appExec PRIVATE $<$<CONFIG:Debug>:_DEBUG>)  # define _DEBUG markup if in DEBUG config

#############
## Sources ##
#############

target_sources(appExec PRIVATE 
    "src/main.cpp"
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

target_link_libraries(appExec PRIVATE appResources)

################
## Deps : stb ##
################

add_library(stb STATIC "libs/stb_image.h")
target_compile_definitions(stb PUBLIC STB_IMAGE_IMPLEMENTATION)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(appExec PRIVATE stb)

#################
## Deps : GLFW ##
#################

find_package(glfw3 REQUIRED)
target_link_libraries(appExec PRIVATE glfw3)