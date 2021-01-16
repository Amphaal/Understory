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

# define executable name
set_target_properties(appExec PROPERTIES 
    OUTPUT_NAME ${CMAKE_PROJECT_NAME}
)

#optimise for release
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # disabled, buged because of rxcpp ? (https://github.com/msys2/MINGW-packages/issues/5786)
    #set_target_properties(appExec PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE) #enable LTO
endif()

#############
## Install ##
#############

#install whole output directory
install(TARGETS appExec
    COMPONENT "App"
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

#############
## INSTALL ##
#############

# spdlog
if(MINGW)
    find_file(MINGW_SPDLOG libspdlog.dll REQUIRED)
    install(FILES ${MINGW_SPDLOG}
        TYPE BIN
        COMPONENT "Runtime"
    )
endif()

# runtime
if(MINGW)
    find_file(MINGW_STDLIB libstdc++-6.dll REQUIRED)
    find_file(MINGW_GCC_SEH libgcc_s_seh-1.dll REQUIRED)
    find_file(MINGW_WINPTHREAD libwinpthread-1.dll REQUIRED)
    install(
        FILES 
            ${MINGW_STDLIB}
            ${MINGW_GCC_SEH}
            ${MINGW_WINPTHREAD}
        TYPE BIN
        COMPONENT "Runtime"
    )
    list(APPEND PEUTIL_WHITELIST_EXTENSIONS 
        "libstdc++-6.dll"
        "libgcc_s_seh-1.dll"
        "libwinpthread-1.dll"
    )
endif()

# Corrade
install(
    FILES 
        $<TARGET_FILE:CorradePluginManager>
        $<TARGET_FILE:CorradeUtility>
    TYPE BIN
    COMPONENT "Magnum"
)

# Magnum
install(
    FILES 
        ${SDL2_DLL_RELEASE}
        $<TARGET_FILE:MagnumGL>
        $<TARGET_FILE:Magnum>
        $<TARGET_FILE:MagnumShaders>
        $<TARGET_FILE:MagnumText>
        $<TARGET_FILE:MagnumTrade>
        $<TARGET_FILE:MagnumTextureTools>
    TYPE BIN
    COMPONENT "Magnum"
)

# Magnum Plugins
include(GNUInstallDirs)
install(
    FILES 
        $<TARGET_FILE:FreeTypeFont>
        $<TARGET_FILE_DIR:FreeTypeFont>/FreeTypeFont.conf
    DESTINATION ${CMAKE_INSTALL_BINDIR}/magnum$<$<CONFIG:Debug>:-d>/fonts
    COMPONENT "Magnum"
)
install(
    FILES 
        $<TARGET_FILE:PngImporter>
        $<TARGET_FILE_DIR:PngImporter>/PngImporter.conf
    DESTINATION ${CMAKE_INSTALL_BINDIR}/magnum$<$<CONFIG:Debug>:-d>/importers
    COMPONENT "Magnum"
)

if(MINGW)
    # Magnum Plugins direct Deps
    find_file(MINGW_FREETYPE_DLL libfreetype-6.dll REQUIRED)
    find_file(MINGW_PNG_DLL libpng16-16.dll REQUIRED)
    install(
        FILES
            ${MINGW_FREETYPE_DLL}
            ${MINGW_PNG_DLL}
        TYPE BIN
        COMPONENT "Magnum"
    )

    # Magnum Plugins further Deps
    include(CMakeDependencies)
    DeployPEDependencies(appExec "Magnum"
        ${MINGW_FREETYPE_DLL}
        ${MINGW_PNG_DLL}
    )
endif()