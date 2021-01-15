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
set_target_properties(appExec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LC)
set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME_LC}")  # define executable name

#optimise for release
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # disable for now (LLD bug)
    # set_target_properties(appExec PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE) #enable LTO
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

# used
SET(WITH_SDL2APPLICATION    ON CACHE INTERNAL "")
SET(WITH_PNGIMPORTER        ON CACHE INTERNAL "") 
SET(WITH_FREETYPEFONT       ON CACHE INTERNAL "")

# not used
SET(WITH_INTERCONNECT       OFF CACHE INTERNAL "")
SET(WITH_TESTSUITE          OFF CACHE INTERNAL "")
SET(WITH_RC                 OFF CACHE INTERNAL "")

SET(WITH_DEBUGTOOLS         OFF CACHE INTERNAL "")
SET(WITH_MESHTOOLS          OFF CACHE INTERNAL "")
SET(WITH_SCENEGRAPH         OFF CACHE INTERNAL "")
SET(WITH_PRIMITIVES         OFF CACHE INTERNAL "")

#fetch
add_subdirectory(corrade EXCLUDE_FROM_ALL)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
add_subdirectory(magnum-plugins EXCLUDE_FROM_ALL)

corrade_add_resource(appExec_RES
    resources.conf
)

target_sources(appExec PRIVATE
    widgets/animation/MatrixAnimator.cpp
    widgets/animation/TimelineBound.cpp
    widgets/scroll/Scissorable.cpp
    widgets/scroll/ScrollablePanel.cpp
    widgets/scroll/Scroller.cpp
    widgets/scroll/ScrollerHandle.cpp
    USApplication.cpp
    ${appExec_RES}
)

# linking
target_link_libraries(appExec PRIVATE
    Corrade::Main
    Magnum::Sdl2Application
    Magnum::GL
    Magnum::Magnum
    Magnum::Shaders
    Magnum::Text
    Magnum::Trade
    MagnumPlugins::FreeTypeFont
    MagnumPlugins::PngImporter
)

target_link_libraries(appExec PRIVATE UnderStory::network)
target_link_libraries(appExec PRIVATE UnderStory::core)

##################
## Deps : RxCpp ##
##################

#link
target_include_directories(appExec PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/RxCpp/Rx/v2/src")

########################
# Runtime dependencies #
########################

include(CMakeDependencies)

# ALL
DeployPEDependencies(appExec "GStreamer"
    $<TARGET_FILE_DIR:appExec>
    $<TARGET_FILE:appExec>
    $<TARGET_FILE:MagnumPlugins::FreeTypeFont>
    $<TARGET_FILE:MagnumPlugins::PngImporter>
)
