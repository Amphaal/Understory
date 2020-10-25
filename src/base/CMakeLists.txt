add_library(appBase INTERFACE)
add_library(UnderStory::base ALIAS appBase)

# configure header with basic app informations
configure_file(
    _understory.h 
    understory.h
)

target_sources(appBase INTERFACE 
    translate.h
    fs_compat.h
    Context.hpp
    Defaults.hpp
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

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include")

#link
target_link_libraries(appBase INTERFACE spdlog)