# resource packaging
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #add Windows ressources (icons)    
    list(APPEND ICON_RESOURCES
        "resources/generated/app.ico"
        "resources/generated/install.ico"
        "resources.rc"
    )
elseif(APPLE)   
    set(ICON_RESOURCES_NAME "app.icns")
    set(ICON_RESOURCES "resources/generated/${ICON_RESOURCES_NAME}") #add MacOS icons in output package
    set_source_files_properties(${ICON_RESOURCES}
        PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_target_properties(appExec PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_Info.plist"
        MACOSX_BUNDLE_ICON_FILE "${ICON_RESOURCES_NAME}"
    )
else()
    message(FATAL_ERROR "Could not determine icons ressources set !")
endif()

# link resources
message(STATUS "Using icon resources files: [${ICON_RESOURCES}]")
target_sources(appExec PRIVATE ${ICON_RESOURCES})