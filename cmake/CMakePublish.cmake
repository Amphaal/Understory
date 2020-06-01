#
#
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_PACKAGE_FILE_EXTENSION ".exe")
elseif(APPLE)
    set(CPACK_PACKAGE_FILE_EXTENSION ".dmg")
endif()

set(SETUP_NAME          "${PROJECT_NAME}-setup")
set(SETUP_NAME_WITH_EXT "${SETUP_NAME}${CPACK_PACKAGE_FILE_EXTENSION}")

#########
# CPack #
#########

set(CPACK_GENERATOR "NSIS")

set(CPACK_PACKAGE_VENDOR              "LVWL")
set(CPACK_PACKAGE_NAME                "${PROJECT_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README        "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_FILE_NAME           "${SETUP_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_EXECUTABLES         "understory;UnderStory")

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL  ON)
set(CPACK_NSIS_PACKAGE_NAME                    "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_MUI_ICON                        "${CMAKE_CURRENT_SOURCE_DIR}/src/app/package/install.ico")
set(CPACK_NSIS_UNINSTALL_NAME                  "Uninstall ${CPACK_PACKAGE_NAME}")

include(CPack)

#
# Zipping
#

#create target to be invoked with bash
add_custom_target(zipForDeploy DEPENDS package)

#installer
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c installer.zip  --format=zip ${SETUP_NAME_WITH_EXT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Ziping installer..."
)

#repository
add_custom_command(TARGET zipForDeploy
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_BINARY_DIR}/repository.zip --format=zip .
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Ziping repository..."
)