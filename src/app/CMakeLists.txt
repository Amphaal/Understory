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

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LC)
set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME_LC}")  # define executable name

#optimise for release
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # disable for now (LLD bug)
    # set_target_properties(appExec PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE) #enable LTO
endif()

# generate icons from png
include(IconGenerator)
generateIcon("resources/logo.png" "package" "app")
generateIcon("resources/install.png" "package" "install")

# resource packaging
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #add Windows ressources (icons)    
    target_sources(appExec PRIVATE 
        "package/app.ico"
        "package/install.ico"
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

###########
# pe-util #
###########

#pe-util is required to find all .dll dependencies
IF(MINGW AND CMAKE_CROSSCOMPILING)
    include(ExternalProject)
    ExternalProject_Add(peldd
        GIT_REPOSITORY  "https://github.com/Amphaal/pe-util.git"
        INSTALL_COMMAND ""
    )

    #define exec
    ExternalProject_Get_Property(peldd BINARY_DIR)
    set(PELDD_EXEC "${BINARY_DIR}/peldd")
    
    #make sure env var is set
    if(NOT MINGW64_ROOT)
        message(FATAL_ERROR "MINGW64_ROOT must be set ! Please use a toolchain file !")
    endif()

    #generate requirements
    add_dependencies(appExec peldd)
    add_custom_command(TARGET appExec POST_BUILD
        COMMAND 
            ${PELDD_EXEC} -a
            -p ${MINGW64_ROOT}/bin
            $<TARGET_FILE:appExec>
            | sed -f ${CMAKE_SOURCE_DIR}/cmake/antislash-to-slash.sed
            | tr -d '\\015' 
            > lib_requirements.txt
        DEPENDS 
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/antislash-to-slash.sed
        BYPRODUCTS
            lib_requirements.txt
    )

    #copy required libs
    add_custom_command(TARGET appExec POST_BUILD
        COMMAND 
            cat lib_requirements.txt | xargs cp -n -t ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        DEPENDS 
            lib_requirements.txt
    )
endif()

#############
## Install ##
#############

#install whole output directory
install(
    DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    TYPE BIN
    COMPONENT app
)

#############
## Sources ##
#############

target_sources(appExec PRIVATE
    Application.hpp
    Utility.hpp
    main.cpp
)

add_subdirectory(widgets)
add_subdirectory(ui)

target_link_libraries(appExec PRIVATE UnderStory::network)
target_link_libraries(appExec PRIVATE UnderStory::core)

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

if(WIN32)
    SET(GML_TARGET "glm")
else()
    SET(GML_TARGET "glm::glm")
endif()

#link
target_link_libraries(appExec PRIVATE ${GML_TARGET})

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
target_include_directories(nuklear INTERFACE ${nuklear_SOURCE_DIR})

#link
target_link_libraries(appExec PRIVATE nuklear)
