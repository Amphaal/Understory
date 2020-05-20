# define build type if any
if(NOT DEFINED APP_BUNDLE_TYPE)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(APP_BUNDLE_TYPE "WIN32") 
    endif()
endif()

# define app executable
add_executable(appExec ${APP_BUNDLE_TYPE})
add_executable(UnderStory::executable ALIAS appExec)
set_target_properties(appExec PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")  # define executable name

#############
## Sources ##
#############

target_sources(appExec PRIVATE
    main.cpp
    Application.hpp
    Utility.hpp
)

#####################
## Deps : CMake RC ##
#####################

include(CMakeRC)

# find resources
file(GLOB_RECURSE APP_RESOURCES_FILES "resources/**")
cmrc_add_resource_library(appResources
    WHENCE resources
    ${APP_RESOURCES_FILES}
)

target_link_libraries(appExec PRIVATE appResources)

################
## Deps : stb ##
################

#link
target_link_libraries(appExec PRIVATE stb)

#################
## Deps : GLFW ##
#################

find_package(glfw3 REQUIRED)

target_link_libraries(appExec PRIVATE
    glfw3
)

###################
## Deps : Vulkan ##
###################

find_package(Vulkan REQUIRED)

# validation layers
find_library(VulkanLKV VkLayer_khronos_validation)
find_library(VulkanLU VkLayer_utils)

target_link_libraries(appExec PRIVATE
    Vulkan
    ${VulkanVL}
    ${VulkanLU}
)
