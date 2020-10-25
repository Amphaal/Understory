###########
# Prepare #
###########

set(SETUP_NAME "${PROJECT_NAME}-setup")

# trad
SET(APP_DESCRIPTION ${PROJECT_DESCRIPTION}
    fr "L'experience Papier-Crayon intuitive !"
)

#########
# CPack #
#########

set(CPACK_COMPONENTS_ALL "app") # only app, no Unspecified 

set(CPACK_GENERATOR "IFW")

set(CPACK_PACKAGE_DIRECTORY           "CPack")
set(CPACK_PACKAGE_VENDOR              "LVWL")
set(CPACK_PACKAGE_NAME                "${PROJECT_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README        "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_FILE_NAME           "${SETUP_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_EXECUTABLES         "understory;UnderStory")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")

#################
# IFW specifics #
#################

# out ext (must be defined for XCompilation)
IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".exe")
elseif(APPLE)
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".dmg")
endif()

set(SETUP_NAME_WITH_EXT "${SETUP_NAME}${CPACK_IFW_PACKAGE_FILE_EXTENSION}")

SET(APP_PACKAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/app/package)

# icons
SET(CPACK_IFW_PACKAGE_LOGO "${APP_PACKAGE_SOURCES}/logo_64.png")
SET(CPACK_IFW_PACKAGE_ICON "${APP_PACKAGE_SOURCES}/install.ico")

SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${CPACK_PACKAGE_INSTALL_DIRECTORY})

#########
# Setup #
#########

include(CPack)

# configure default component
cpack_add_component(app REQUIRED
    DISPLAY_NAME ${PROJECT_NAME}
)

INCLUDE(CPackIFW)

# installer configuration
cpack_ifw_configure_component(app
    DESCRIPTION      ${APP_DESCRIPTION}
    SCRIPT          "${APP_PACKAGE_SOURCES}/ifw/install.js"
    USER_INTERFACES "${APP_PACKAGE_SOURCES}/ifw/install.ui"
    TRANSLATIONS    "${APP_PACKAGE_SOURCES}/ifw/_i18n/fr.qm"
)

# repository for updates
cpack_ifw_add_repository(coreRepo 
    URL ${APP_REPOSITORY_URL}
)

###########
# Zipping #
###########

#create target to be invoked with bash
add_custom_target(zipForDeploy DEPENDS package)

SET(APP_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/${CPACK_PACKAGE_DIRECTORY})
SET(APP_REPOSITORY_DIRECTORY ${APP_PACKAGE_DIRECTORY}/_CPack_Packages/${CPACK_SYSTEM_NAME}/IFW/${SETUP_NAME}/repository)

#installer
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/installer.zip --format=zip ${SETUP_NAME_WITH_EXT}
    WORKING_DIRECTORY ${APP_PACKAGE_DIRECTORY}
    COMMENT "Ziping installer..."
)

#repository
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/repository.zip --format=zip .
    WORKING_DIRECTORY ${APP_REPOSITORY_DIRECTORY}
    COMMENT "Ziping repository..."
)
