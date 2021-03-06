add_library(appBase INTERFACE)
add_library(UnderStory::base ALIAS appBase)

# configure header with basic app informations
configure_file(
    understory.in.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/understory.h
)

target_sources(appBase INTERFACE 
    translate.h
    fs_compat.h
    Context.hpp
    Defaults.hpp
    Debug.hpp
)

###########################
## Deps : intl / gettext ##
###########################

find_package(Intl REQUIRED)
find_package(Gettext REQUIRED)

target_include_directories(appBase INTERFACE ${Intl_INCLUDE_DIRS})
target_link_libraries(appBase INTERFACE ${Intl_LIBRARIES})

###################
## Deps : spdlog ##
###################

if(NOT TARGET spdlog)
    #find
    find_package(spdlog REQUIRED)
endif()

#link
target_link_libraries(appBase INTERFACE spdlog::spdlog)

#######################
## Deps : magic_enum ##
#######################

add_subdirectory(magic_enum)

#link
target_link_libraries(appBase INTERFACE magic_enum::magic_enum)