##########################
### CPACK CONFIGURATION ##
##########################

SET(CPACK_GENERATOR IFW)

SET(APP_DESCRIPTION ${PROJECT_NAME}
    fr "L'experience JdR simplifiée."
)

SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})
SET(CPACK_IFW_PACKAGE_PUBLISHER ${APP_PUBLISHER})
SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${APP_PUBLISHER})
SET(CPACK_IFW_PRODUCT_URL ${APP_PATCHNOTE_URL})
SET(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDirX64@/${PROJECT_NAME}")

IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".exe")
elseif(APPLE)
    SET(CPACK_IFW_PACKAGE_FILE_EXTENSION ".dmg")
endif()

#icons
SET(CPACK_IFW_PACKAGE_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/src/app/package/logo_64.png")
SET(CPACK_IFW_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/app/package/install.ico")

##############
### INSTALL ##
##############

INCLUDE(CPack)

#configure default component
cpack_add_component(${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME} DOWNLOADED)

######################################
# CPACK IFW COMPONENTS CONFIGURATION #
######################################

#SET(CPACK_IFW_VERBOSE ON)
INCLUDE(CPackIFW)

# #installer configuration
cpack_ifw_configure_component(${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME}
FORCED_INSTALLATION
SCRIPT          "${CMAKE_CURRENT_SOURCE_DIR}/src/app/package/ifw/install.js"
USER_INTERFACES "${CMAKE_CURRENT_SOURCE_DIR}/src/app/package/ifw/install.ui"
#TRANSLATIONS "${CMAKE_BINARY_DIR}/fr.qm"
DESCRIPTION ${APP_DESCRIPTION}
)

#repository for updates
cpack_ifw_add_repository(coreRepo 
URL "https://dl.bintray.com/amphaal/rpgrpz/ifw-${CPACK_SYSTEM_NAME}"
)

# ###########
# # Prepare #
# ###########

# if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#     set(CPACK_PACKAGE_FILE_EXTENSION ".exe")
# elseif(APPLE)
#     set(CPACK_PACKAGE_FILE_EXTENSION ".dmg")
# endif()

# set(SETUP_NAME          "${PROJECT_NAME}-setup")
# set(SETUP_NAME_WITH_EXT "${SETUP_NAME}${CPACK_PACKAGE_FILE_EXTENSION}")

# # trad
# SET(APP_DESCRIPTION ${PROJECT_DESCRIPTION}
#     fr "L'experience Papier-Crayon intuitive !"
# )

# #########
# # CPack #
# #########

# set(CPACK_GENERATOR "IFW")

# # set(CPACK_PACKAGE_DIRECTORY           "CPack")
# # set(CPACK_PACKAGE_VENDOR              "LVWL")
# # set(CPACK_PACKAGE_NAME                "${PROJECT_NAME}")
# # set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
# # set(CPACK_RESOURCE_FILE_README        "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# # set(CPACK_PACKAGE_FILE_NAME           "${SETUP_NAME}")
# # set(CPACK_PACKAGE_INSTALL_DIRECTORY   "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}")
# # set(CPACK_PACKAGE_EXECUTABLES         "understory;UnderStory")
# # SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")

# #################
# # IFW specifics #
# #################

# SET(APP_PACKAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/app/package)

# # icons
# SET(CPACK_IFW_PACKAGE_LOGO "${APP_PACKAGE_SOURCES}/logo_64.png")
# SET(CPACK_IFW_PACKAGE_ICON "${APP_PACKAGE_SOURCES}/install.ico")

# SET(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
# SET(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${CPACK_PACKAGE_INSTALL_DIRECTORY})

# #########
# # Setup #
# #########

# include(CPack)

# # configure default component
# cpack_add_component(app REQUIRED
#     DISPLAY_NAME ${PROJECT_NAME}
# )

# INCLUDE(CPackIFW)

# # installer configuration
# cpack_ifw_configure_component(app
#     DESCRIPTION      ${APP_DESCRIPTION}
#     SCRIPT          "${APP_PACKAGE_SOURCES}/ifw/install.js"
#     USER_INTERFACES "${APP_PACKAGE_SOURCES}/ifw/install.ui"
#     TRANSLATIONS    "${APP_PACKAGE_SOURCES}/ifw/_i18n/fr.qm"
# )

# # repository for updates
# cpack_ifw_add_repository(coreRepo 
#     URL "https://dl.bintray.com/amphaal/understory/repository"
# )

# ###########
# # Zipping #
# ###########

# #create target to be invoked with bash
# add_custom_target(zipForDeploy DEPENDS package)

# SET(APP_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/${CPACK_PACKAGE_DIRECTORY})
# SET(APP_REPOSITORY_DIRECTORY ${APP_PACKAGE_DIRECTORY}/_CPack_Packages/${CPACK_SYSTEM_NAME}/IFW/${SETUP_NAME}/repository)

# #installer
# add_custom_command(TARGET zipForDeploy
#     COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/installer.zip --format=zip 
#         ${SETUP_NAME_WITH_EXT}
#     WORKING_DIRECTORY ${APP_PACKAGE_DIRECTORY}
#     COMMENT "Ziping installer..."
# )

# #repository
# add_custom_command(TARGET zipForDeploy
#     COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/repository.zip --format=zip
#         Updates.xml
#         app
#     WORKING_DIRECTORY ${APP_REPOSITORY_DIRECTORY}
#     COMMENT "Ziping repository..."
# )
