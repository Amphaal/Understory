add_library(appBase INTERFACE)
add_library(UnderStory::base ALIAS appBase)

# configure header with basic app informations
configure_file(
    _understory.h 
    understory.h
)

target_sources(appBase INTERFACE 
    translate.h
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

#using header-only version to prevent linking errors with static lib version

include(FetchContent)

#fetch
FetchContent_Declare(spdlog
    GIT_REPOSITORY  "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"    
)
FetchContent_Populate(spdlog)

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE ${spdlog_SOURCE_DIR}/include)

#link
target_link_libraries(appBase INTERFACE spdlog)