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
    set_target_properties(appExec PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE) #enable LTO
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

# only for debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # configure
    SET(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    SET(WITH_IMGUI ON CACHE INTERNAL "")
    add_subdirectory(magnum-integration EXCLUDE_FROM_ALL)
    # link magnum-integration / IMGUI
    target_link_libraries(appExec PRIVATE MagnumIntegration::ImGui)
endif()

# resources
corrade_add_resource(appExec_RES
    resources.conf
)

# list code source
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

# linking Magnum
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

# linking inner deps
target_link_libraries(appExec PRIVATE UnderStory::network)
target_link_libraries(appExec PRIVATE UnderStory::core)

#################
## Translation ##
#################

include(CMakeGettextX)
configure_gettext(
    DOMAIN ${PROJECT_NAME}
    TARGET_NAME appExec-translate
    SOURCES
        USApplication.cpp
    POTFILE_DESTINATION 
        ${CMAKE_CURRENT_BINARY_DIR}/i18n
    GMOFILE_DESTINATION 
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations
    POFILE_DESTINATION i18n
    INSTALL_DESTINATION "translations"
    INSTALL_COMPONENT "App"
    LANGUAGES
        "fr"
    XGETTEXT_ARGS 
        "-k_"
)

add_dependencies(appExec appExec-translate)

#################
# Sentry-native #
#################

SET(SENTRY_BUILD_SHARED_LIBS ON CACHE INTERNAL "")
add_subdirectory(sentry-native EXCLUDE_FROM_ALL)

# get version
get_directory_property(SENTRY_NATIVE_PROJECT_VERSION
    DIRECTORY sentry-native 
    DEFINITION PROJECT_VERSION
)

# link
target_link_libraries(appExec PRIVATE sentry)

# track version into file
execute_process(
    COMMAND printf "${CMAKE_PROJECT_VERSION}"
    OUTPUT_FILE ${CMAKE_BINARY_DIR}/version
)

# install
install(FILES 
    $<TARGET_FILE:sentry>
    TYPE BIN
    COMPONENT "SentryNative"
)

# add crashpad_handler next to app, and to install process
set_target_properties(crashpad_handler PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:appExec>
)
install(TARGETS crashpad_handler 
    COMPONENT "SentryNative"
)

# generate pdb
IF(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")  
    
    target_link_options(appExec PRIVATE -Wl,-pdb=) # generate pdb files
    target_compile_options(appExec PRIVATE -gcodeview) #use codeview for pdbs
    
    # add .pdb to install
    install(
        FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_FILE_BASE_NAME:appExec>.pdb
        TYPE BIN
        COMPONENT "App"
    )

endif()

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