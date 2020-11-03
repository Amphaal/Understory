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
add_subdirectory(corrade)
add_subdirectory(magnum)
add_subdirectory(magnum-plugins)

corrade_add_resource(appExec_RES
    resources.conf
)

target_sources(appExec PRIVATE
    widgets/animation/MatrixAnimator.cpp
    widgets/animation/TimelineBound.cpp
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
            -p $<TARGET_FILE_DIR:appExec>
            $<TARGET_FILE:appExec>
            $<TARGET_FILE:MagnumPlugins::FreeTypeFont>
            $<TARGET_FILE:MagnumPlugins::PngImporter>
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