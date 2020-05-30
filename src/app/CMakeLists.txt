# define build type if any
if(NOT DEFINED APP_BUNDLE_TYPE)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(APP_BUNDLE_TYPE WIN32)
    elseif(APPLE)
        SET(APP_BUNDLE_TYPE MACOSX_BUNDLE)
    endif()
endif()

# define app executable
add_executable(appExec ${APP_BUNDLE_TYPE})
add_executable(UnderStory::executable ALIAS appExec)
set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")  # define executable name

# generate icons from png
include(IconGenerator)
generateIcon("resources/logo.png" "package" "app")

# resource packaging
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #add Windows ressources (icons)    
    target_sources(appExec PRIVATE 
        "package/app.ico"
        "package/resources.rc"
    )
elseif(APPLE)   
    target_sources(appExec PRIVATE "package/app.icns") #add MacOS icons in output package
    set_source_files_properties("package/app.icns"
        PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_target_properties(appExec PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/package/_Info.plist"
        MACOSX_BUNDLE_ICON_FILE "app.icns"
    ) 
endif()

#############
## Sources ##
#############

target_sources(appExec PRIVATE
    main.cpp
    Application.hpp
    Utility.hpp
)

target_link_libraries(appExec PRIVATE appBase)

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

#link
target_link_libraries(appExec PRIVATE stb)

#################
## Deps : GLFW ##
#################

find_package(glfw3 REQUIRED)

target_link_libraries(appExec PRIVATE glfw)

#################
## Deps : GLEW ##
#################

find_package(GLEW REQUIRED)

target_link_libraries(appExec PRIVATE GLEW::GLEW)

##################
## Deps : GLFWM ##
##################

#fetch
FetchContent_Declare(glfwm
    GIT_REPOSITORY  "https://github.com/Amphaal/glfwm.git"
)
FetchContent_MakeAvailable(glfwm)

#link
target_link_libraries(appExec PRIVATE glfwm)

################
## Deps : GLM ##
################

#find
find_package(glm REQUIRED)

#link
target_link_libraries(appExec PRIVATE glm)

##################
## Deps : RxCpp ##
##################

#fetch
FetchContent_Declare(rxcpp
    GIT_REPOSITORY  "https://github.com/ReactiveX/RxCpp.git"
)
FetchContent_Populate(rxcpp)

#link
target_include_directories(appExec PRIVATE ${rxcpp_SOURCE_DIR}/Rx/v2/src)

####################
## Deps : Nuklear ##
####################

#fetch
FetchContent_Declare(nuklear
    GIT_REPOSITORY  "https://github.com/Immediate-Mode-UI/Nuklear.git"
)
FetchContent_Populate(nuklear)

#define as target
add_library(nuklear INTERFACE)
target_include_directories(stb INTERFACE ${nuklear_SOURCE_DIR})

#link
target_link_libraries(appExec PRIVATE nuklear)