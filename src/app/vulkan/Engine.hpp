// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#pragma once

#include <spdlog/spdlog.h>

#include <utility>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFWM/glfwm.hpp>

#include <vulkan/vulkan.hpp>

#include "src/base/understory.h"

#include "src/app/vulkan/Debug.hpp"

namespace UnderStory {

namespace Vulkan {

class Engine {
 public:
    explicit Engine(bool withValidationLayers = true) : _withValidationLayers(withValidationLayers) {
        // initialize the vk::ApplicationInfo structure
        auto appVersion = VK_MAKE_VERSION(APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_PATCH_VERSION);
        vk::ApplicationInfo applicationInfo( APP_FULL_DENOM, appVersion, APP_ENGINE_NAME, appVersion, VK_API_VERSION_1_2 );

        // initialize the vk::InstanceCreateInfo
        vk::InstanceCreateInfo instanceCreateInfo( {}, &applicationInfo );
        this->_setRequiredExtensions(instanceCreateInfo);
        this->_mayIncludeValidationLayers(instanceCreateInfo);

        // create a UniqueInstance
        this->_instance = vk::createInstanceUnique(instanceCreateInfo);
        this->_mayUseDebugMessenger();
    }

 private:
    bool _withValidationLayers;
    vk::UniqueInstance _instance;

    std::vector<const char*> _extensions;
    void _setRequiredExtensions(vk::InstanceCreateInfo &ici) {
        // from glfw extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // extension vector
        this->_extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // push ext utils if validation layer is required
        if(this->_withValidationLayers) this->_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // update struct
        ici.enabledExtensionCount = this->_extensions.size();
        ici.ppEnabledExtensionNames = this->_extensions.data();
    }

    //
    // DEBUGGING
    //

    vk::UniqueDebugUtilsMessengerEXT _debugUtilsMessenger;

    static inline std::vector<const char *> _instanceValidationLayerNames { "VK_LAYER_KHRONOS_validation" };
    void _mayIncludeValidationLayers(vk::InstanceCreateInfo &ici) {
        if(!this->_withValidationLayers) return;

        // check layer handling
        auto instanceLayerProperties = vk::enumerateInstanceLayerProperties();
        auto hasLayers = _checkLayers(_instanceValidationLayerNames, instanceLayerProperties);

        // error if not
        if(!hasLayers) {
            std::cout << "Set the environment variable VK_LAYER_PATH to point to the location of your layers" << std::endl;
            exit( 1 );
        }

        // update struct
        ici.enabledLayerCount = _instanceValidationLayerNames.size();
        ici.ppEnabledLayerNames = _instanceValidationLayerNames.data();
    }

    static bool _checkLayers( std::vector<char const *> const & layers, std::vector<vk::LayerProperties> const & properties ) {
        // return true if all layers are listed in the properties
        return std::all_of( layers.begin(), layers.end(), [&properties]( char const * name ) {
            return std::find_if( properties.begin(), properties.end(), [&name]( vk::LayerProperties const & property ) {
                    return strcmp( property.layerName, name ) == 0;
                } ) != properties.end();
        } );
    }

    void _mayUseDebugMessenger() {
        if(!this->_withValidationLayers) return;

        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            this->_instance->getProcAddr( "vkCreateDebugUtilsMessengerEXT" )
        );
        if ( !pfnVkCreateDebugUtilsMessengerEXT ) {
            std::cout << "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function." << std::endl;
            exit( 1 );
        }

        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            this->_instance->getProcAddr( "vkDestroyDebugUtilsMessengerEXT" )
        );
        if ( !pfnVkDestroyDebugUtilsMessengerEXT ) {
            std::cout << "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function." << std::endl;
            exit( 1 );
        }

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError );
        vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );

        this->_debugUtilsMessenger = this->_instance->createDebugUtilsMessengerEXTUnique(
            vk::DebugUtilsMessengerCreateInfoEXT( {}, severityFlags, messageTypeFlags, &debugCallback )
        );
    }
};

}  // namespace Vulkan

}  // namespace UnderStory
