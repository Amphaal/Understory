if(WIN32)
    find_program(convert NAMES convert)
    SET(IconGenerator_SUFFIX ".ico")
elseif(APPLE)
    find_program(sips NAMES sips)
    find_program(iconutil NAMES iconutil)
    SET(IconGenerator_SUFFIX ".icns")
endif()


function(generateIcon fromPNG toFolder iconName)
    SET(IconGenerator_OUTPUT_ICON ${CMAKE_CURRENT_BINARY_DIR}/${toFolder}/${iconName}${IconGenerator_SUFFIX})
    SET(fromPNG ${CMAKE_CURRENT_SOURCE_DIR}/${fromPNG})
    SET(toFolder ${CMAKE_CURRENT_SOURCE_DIR}/${toFolder}/${iconName}.iconset)
    
    if(WIN32)
        # https://imagemagick.org/discourse-server/viewtopic.php?t=14080
        message(FATAL_ERROR "Unimplemented !")
    elseif(APPLE)
        add_custom_command(OUTPUT ${IconGenerator_OUTPUT_ICON}
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