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
    vulkan/Engine.hpp
    vulkan/Debug.hpp
    Application.hpp
    Utility.hpp
)

target_link_libraries(appExec PRIVATE appBase)

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

target_link_libraries(appExec PRIVATE glfw)

###################
## Deps : Vulkan ##
###################

find_package(Vulkan REQUIRED)

# validation layers
#find_library(VulkanLKV VkLayer_khronos_validation)

target_link_libraries(appExec PRIVATE
    ${Vulkan_LIBRARIES}
    #${VulkanLKV}
)

target_include_directories(appExec PRIVATE
    ${Vulkan_INCLUDE_DIRS}
)

# glslc
find_program(GLSLC_EXECUTABLE NAMES glslangValidator)

function(compile_to_spirv)
    foreach(file ${ARGN})
        set(generated_ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${file}.spv)
        get_filename_component(_generated_directory_ ${generated_} DIRECTORY)
        
        add_custom_command(TARGET appExec PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${_generated_directory_}
            COMMAND ${GLSLC_EXECUTABLE} -V
                ${CMAKE_CURRENT_SOURCE_DIR}/${file} 
                -o ${generated_}
            BYPRODUCTS ${generated_}
        )
    endforeach()
endfunction()

##################
## Deps : GLFWM ##
##################

#fetch
FetchContent_Declare(glfwm
    GIT_REPOSITORY  "https://github.com/giorgiomarcias/glfwm.git"
)
FetchContent_MakeAvailable(glfwm)

#handle vulkan integration
target_compile_definitions(glfwm PUBLIC GLFW_INCLUDE_VULKAN)

#link
target_link_libraries(appExec PRIVATE glfwm)

##
## shaders compilation
##

# compile_to_spirv(
#     shaders/test.frag
#     shaders/test.vert
# )
