# determine programs to use and file extensions
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_program(convert NAMES convert)
    SET(IconGenerator_SUFFIX ".ico")
elseif(APPLE)
    find_program(sips NAMES sips)
    find_program(iconutil NAMES iconutil)
    SET(IconGenerator_SUFFIX ".icns")
endif()

# allows to generate icons from PNG file
function(generateIcon fromPNG toFolder iconName)
    # override with current source dir
    SET(fromPNG ${CMAKE_CURRENT_SOURCE_DIR}/${fromPNG})

    # define specific output directory target for MacOS
    if(APPLE)
        SET(toFolder ${CMAKE_CURRENT_SOURCE_DIR}/${toFolder}/${iconName}.iconset)
    endif()
    
    # Generate [${iconName}] icon
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        #
        SET(IconGenerator_OUTPUT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/${toFolder}/${iconName}${IconGenerator_SUFFIX})
        
        # execute
        execute_process(COMMAND 
            ${convert} -background transparent ${fromPNG} 
                       -define icon:auto-resize=16,32,48,64,96,128,256 
                       ${IconGenerator_OUTPUT_ICON}
        )
    elseif(APPLE)
        execute_process(
            COMMAND mkdir ${toFolder}
            COMMAND ${sips} -z 16 16     ${fromPNG} --out ${toFolder}/icon_16x16.png
            COMMAND ${sips} -z 32 32     ${fromPNG} --out ${toFolder}/icon_16x16@2x.png
            COMMAND ${sips} -z 32 32     ${fromPNG} --out ${toFolder}/icon_32x32.png
            COMMAND ${sips} -z 64 64     ${fromPNG} --out ${toFolder}/icon_32x32@2x.png
            COMMAND ${sips} -z 128 128   ${fromPNG} --out ${toFolder}/icon_128x128.png
            COMMAND ${sips} -z 256 256   ${fromPNG} --out ${toFolder}/icon_128x128@2x.png
            COMMAND ${sips} -z 256 256   ${fromPNG} --out ${toFolder}/icon_256x256.png
            COMMAND ${sips} -z 512 512   ${fromPNG} --out ${toFolder}/icon_256x256@2x.png
            COMMAND ${sips} -z 512 512   ${fromPNG} --out ${toFolder}/icon_512x512.png
            COMMAND ${iconutil} -c icns ${toFolder}
            COMMAND rm -R ${toFolder}
        )
    endif()
endfunction()