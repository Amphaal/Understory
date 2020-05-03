# define build type if any
if(NOT DEFINED APP_BUNDLE_TYPE)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(APP_BUNDLE_TYPE "WIN32") 
    endif()
endif()

# define app executable
add_executable(appExec ${APP_BUNDLE_TYPE})
add_executable(UnderStory::executable ALIAS appExec)
set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")  # define executable name

#############
## Sources ##
#############

target_sources(appExec PRIVATE
    main.cpp
    Application.hpp
    Utility.hpp
)

#####################
## Deps : CMake RC ##
#####################

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
target_link_libraries(appExec PRIVATE stb)


###################
## Deps : Magnum ##
###################

find_package(Magnum REQUIRED GL GlfwApplication)
find_package(Corrade REQUIRED Main)

target_link_libraries(appExec PRIVATE
    Corrade::Main    
    Magnum::Application
    Magnum::GL
)