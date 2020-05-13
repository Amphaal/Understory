if(NOT DEFINED ENV{SCITER_SDK})
    message(FATAL_ERROR "Required ENV variable SCITER_SDK does not exists. Please check README.md for more details !")
endif()

SET(SCITER_SDK "$ENV{SCITER_SDK}")

if(WIN32)
    SET(SCITER_PLATFORM "win")
elseif(APPLE)
    SET(SCITER_PLATFORM "osx")
endif()

SET(SCITER_BIN_LOCATION ${SCITER_SDK}/bin.${SCITER_PLATFORM})

# packfolder
add_executable(sciterPackFolder IMPORTED)
set_target_properties(sciterPackFolder PROPERTIES
    IMPORTED_LOCATION "${SCITER_BIN_LOCATION}/packfolder${CMAKE_EXECUTABLE_SUFFIX}"
)

# tiscript
add_executable(sciterTiScript IMPORTED)
set_target_properties(sciterTiScript PROPERTIES
    IMPORTED_LOCATION "${SCITER_BIN_LOCATION}/tiscript${CMAKE_EXECUTABLE_SUFFIX}"
)

# lib
add_library(sciter SHARED IMPORTED)
set_target_properties(sciter PROPERTIES
    IMPORTED_LOCATION "${SCITER_BIN_LOCATION}/x64/sciter${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

target_include_directories(sciter INTERFACE ${SCITER_SDK}/include)

# use find
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Sciter DEFAULT_MSG 
    SCITER_SDK
)
