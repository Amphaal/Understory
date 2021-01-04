# resource packaging
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #add Windows ressources (icons)    
    list(APPEND ICON_RESOURCES
        "app.ico"
        "install.ico"
        "resources.rc"
    )
elseif(APPLE)   
    set(ICON_RESOURCES "app.icns") #add MacOS icons in output package
    set_source_files_properties("app.icns"
        PROPERTIES 
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_target_properties(appExec PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_Info.plist"
        MACOSX_BUNDLE_ICON_FILE "app.icns"
    )
else()
    message(FATAL_ERROR "Could not determine icons ressources set !")
endif()

# link resources
message(STATUS "Using icon resources files: [${ICON_RESOURCES}]")
target_sources(appExec PRIVATE ${ICON_RESOURCES})