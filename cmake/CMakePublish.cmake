set(APP_PACKAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/package)

#########
# CPack #
#########

set(CPACK_GENERATOR IFW)

set(CPACK_PACKAGE_VENDOR              "LVWL")
set(CPACK_PACKAGE_NAME                ${PROJECT_NAME})
set(CPACK_RESOURCE_FILE_LICENSE       ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README        ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_FILE_NAME           ${PROJECT_NAME}-setup)
set(CPACK_PACKAGE_INSTALL_DIRECTORY   ${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})

#################
# IFW specifics #
#################

# out ext (must be defined for XCompilation)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_IFW_PACKAGE_FILE_EXTENSION ".exe")
elseif(APPLE)
    set(CPACK_IFW_PACKAGE_FILE_EXTENSION ".dmg")
endif()

# must be hardset before including CPack, if not, CPACK_PACKAGE_FILE_NAME is overriden for some reason
set(SETUP_NAME          ${CPACK_PACKAGE_FILE_NAME})
set(SETUP_NAME_WITH_EXT ${CPACK_PACKAGE_FILE_NAME}${CPACK_IFW_PACKAGE_FILE_EXTENSION})

# icons
set(CPACK_IFW_PACKAGE_LOGO "${APP_PACKAGE_SOURCES}/resources/logo_64.png")
set(CPACK_IFW_PACKAGE_ICON "${APP_PACKAGE_SOURCES}/resources/generated/install.ico")

set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${CPACK_PACKAGE_INSTALL_DIRECTORY})

#########
# Setup #
#########

set(CPACK_IFW_VERBOSE ON)
include(CPack)
include(CPackIFW)

# App
cpack_add_component("App" DOWNLOADED)
cpack_ifw_configure_component("App"
    DISPLAY_NAME "${PROJECT_NAME} ${PROJECT_VERSION}"
    DESCRIPTION
        ${PROJECT_DESCRIPTION}
        fr "L'experience Papier-Crayon intuitive !"
    SORTING_PRIORITY 1000
    SCRIPT          "${APP_PACKAGE_SOURCES}/ifw/EndInstallerForm.js"
    USER_INTERFACES "${APP_PACKAGE_SOURCES}/ifw/EndInstallerForm.ui"
    FORCED_INSTALLATION
)

# Runtime 
cpack_add_component("Runtime" DOWNLOADED)
cpack_ifw_configure_component("Runtime"
    DISPLAY_NAME 
        "Runtime" 
        fr "Composants de base"
    DESCRIPTION 
        "Essential components used by ${PROJECT_NAME} and other libraries"
        fr "Composants essentiels utilisés par ${PROJECT_NAME} et autres librairies"
    SORTING_PRIORITY 900
    VERSION "1.0.0"
    FORCED_INSTALLATION
)

# Magnum
get_directory_property(MAGNUM_LIBRARY_VERSION
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/app/magnum 
    DEFINITION MAGNUM_LIBRARY_VERSION
)
cpack_add_component("Magnum" DOWNLOADED)
cpack_ifw_configure_component("Magnum"
    DISPLAY_NAME "Magnum ${MAGNUM_LIBRARY_VERSION}"
    DESCRIPTION 
        "Essential framework used by ${PROJECT_NAME}"
        fr "Framework essentiel utilisé par ${PROJECT_NAME}"
    SORTING_PRIORITY 98
    VERSION ${MAGNUM_LIBRARY_VERSION}
    FORCED_INSTALLATION
)

# sentry-native
cpack_add_component("SentryNative" DOWNLOADED)
cpack_ifw_configure_component("SentryNative"
    DISPLAY_NAME "Sentry-Native ${SENTRY_NATIVE_PROJECT_VERSION}"
    DESCRIPTION
        "Utility that allows automatic bugs reports"
        fr "Utilitaire qui permet la génération de rapports de bugs automatiques"
    SORTING_PRIORITY 95
    VERSION ${SENTRY_NATIVE_PROJECT_VERSION}
    FORCED_INSTALLATION
)

######################################
# CPACK IFW COMPONENTS CONFIGURATION #
######################################

# repository for updates
cpack_ifw_add_repository(coreRepo 
    URL ${APP_REPOSITORY_URL}
)

###########
# Zipping #
###########

# create target to be invoked with bash
add_custom_target(zipForDeploy DEPENDS package)

# variables
set(CPACK_PACKAGES_DIR ${CMAKE_BINARY_DIR}/_CPack_Packages)
set(APP_REPOSITORY_DIRECTORY ${CPACK_PACKAGES_DIR}/${CPACK_SYSTEM_NAME}/IFW/${SETUP_NAME}/repository)

# repository
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/repository.zip --format=zip .
    WORKING_DIRECTORY ${APP_REPOSITORY_DIRECTORY}
    COMMENT "Ziping repository..."
)

#installer
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/installer.zip --format=zip ${SETUP_NAME_WITH_EXT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Ziping installer..."
)

# cleanup
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E rm -r
        ${CPACK_PACKAGES_DIR} 
        ${SETUP_NAME_WITH_EXT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleanup CPack files..."
)