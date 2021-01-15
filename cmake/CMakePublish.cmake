SET(APP_PACKAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/package)

#########
# CPack #
#########

set(CPACK_GENERATOR IFW)

set(CPACK_PACKAGE_DIRECTORY           "CPack")
set(CPACK_PACKAGE_VENDOR              "LVWL")
set(CPACK_PACKAGE_NAME                ${PROJECT_NAME})
set(CPACK_RESOURCE_FILE_LICENSE       ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README        ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_FILE_NAME           ${PROJECT_NAME}-setup)
set(CPACK_PACKAGE_INSTALL_DIRECTORY   ${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})

#################
# IFW specifics #
#################

# out ext (must be defined for XCompilation)
IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".exe")
elseif(APPLE)
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".dmg")
endif()

# icons
SET(CPACK_IFW_PACKAGE_LOGO "${APP_PACKAGE_SOURCES}/resources/logo_64.png")
SET(CPACK_IFW_PACKAGE_ICON "${APP_PACKAGE_SOURCES}/resources/generated/install.ico")

SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${CPACK_PACKAGE_INSTALL_DIRECTORY})

#########
# Setup #
#########

SET(CPACK_IFW_VERBOSE ON)
include(CPack)
INCLUDE(CPackIFW)

# App
cpack_add_component("App" DOWNLOADED)
cpack_ifw_configure_component("App"
    DISPLAY_NAME ${PROJECT_NAME}
    DESCRIPTION
        ${PROJECT_DESCRIPTION}
        fr "L'experience Papier-Crayon intuitive !"
    SCRIPT          "${APP_PACKAGE_SOURCES}/ifw/EndInstallerForm.js"
    USER_INTERFACES "${APP_PACKAGE_SOURCES}/ifw/EndInstallerForm.ui"
    FORCED_INSTALLATION
)

# repository for updates
cpack_ifw_add_repository(coreRepo 
    URL ${APP_REPOSITORY_URL}
)

###########
# Zipping #
###########

# source
SET(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}) #override as CPACK_SYSTEM_NAME may end up wrong (CMAKE bug?)
SET(CPACK_PACKAGE_FILE_NAME_FULL ${CPACK_PACKAGE_FILE_NAME}${CPACK_IFW_PACKAGE_FILE_EXTENSION})

SET(CPACK_PACKAGES_DIR ${CMAKE_BINARY_DIR}/_CPack_Packages)
SET(APP_REPOSITORY ${CPACK_PACKAGES_DIR}/${CPACK_SYSTEM_NAME}/IFW/${CPACK_PACKAGE_FILE_NAME}/repository)

SET(APP_PACKAGE_LATEST ${CPACK_PACKAGE_NAME}-latest-${CPACK_SYSTEM_NAME})
SET(APP_PACKAGE_LATEST_FULL ${APP_PACKAGE_LATEST}${CPACK_IFW_PACKAGE_FILE_EXTENSION})

# create target to be invoked with bash
add_custom_target(zipForDeploy DEPENDS package)

# zip repository
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/repository.zip --format=zip .
    WORKING_DIRECTORY ${APP_REPOSITORY}
    COMMENT "Ziping IFW repository..."
)

# zip installer
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E rename ${CPACK_PACKAGE_FILE_NAME_FULL} ${APP_PACKAGE_LATEST_FULL}
    COMMAND ${CMAKE_COMMAND} -E tar c installer.zip --format=zip ${APP_PACKAGE_LATEST_FULL}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Ziping IFW installer..."
)

# cleanup
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E rm -r
        ${CPACK_PACKAGES_DIR} 
        ${APP_PACKAGE_LATEST_FULL}
    COMMENT "Cleanup CPack files..."
)