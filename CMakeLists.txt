# use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message("== Using CCache ! ==")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# default configuration
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# pre-startup
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
message(STATUS "Using toolchain file: [${CMAKE_TOOLCHAIN_FILE}]")

# project setup
project(UnderStory
    VERSION 0.0.7
    DESCRIPTION "An intuitive Pen & Paper experience"
    HOMEPAGE_URL "https://github.com/Amphaal/understory"
    LANGUAGES C CXX
)

#
SET(APP_REPOSITORY_HOST         "dl.bintray.com")
SET(APP_REPOSITORY_COMMAND_LITE "/amphaal/understory/repository")
SET(APP_REPOSITORY_COMMAND      "${APP_REPOSITORY_COMMAND_LITE}/Updates.xml")
SET(APP_REPOSITORY_URL          "https://${APP_REPOSITORY_HOST}/${APP_REPOSITORY_COMMAND_LITE}")

# for sentry-native
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
SET(SENTRY_ENDPOINT "https://b9618c02d38d4633b02c3dd2b736674e@o333337.ingest.sentry.io/5518788")

#
SET(APP_TRANSLATION_FOLDER      "translations")

#policies
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0076 NEW)
endif()

################################
## CPP Compiler Configuration ##
################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# OPTIONS
option(LOCAL_TESTING "If tests are enabled, builds local-only tests" OFF)

#################################
## define bundle configuration ##
#################################

# name for default component binding when installing
SET(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "App")

# must set all 3 for Magnum
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out_lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out_arch)

#for generated config header
include_directories(
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# define _DEBUG markup if in DEBUG config
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>) 

###########
## ICONS ##
###########

# generate icons from png
include(IconGenerator)
generateIcon(
    "src/package/resources/logo.png"
    "src/package/resources/generated"
    "app"
)
generateIcon(
    "src/package/resources/install.png"
    "src/package/resources/generated"
    "install"
)

################
## Submodules ##
################

add_subdirectory(src/base)
add_subdirectory(src/models)
add_subdirectory(src/network)
add_subdirectory(src/core)
add_subdirectory(src/app)
add_subdirectory(src/package)

###########
## Tests ##
###########

#dont build tests if included as submodule
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

#########################
## publishing handling ##
#########################

include(CMakePublish)