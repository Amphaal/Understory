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

#include <vulkan/vulkan.hpp>

#include "src/base/understory.h"

#include <GLFWM/glfwm.hpp>

#include "src/app/vulkan/Debug.hpp"
#include "src/app/vulkan/Utils.hpp"

#if defined(_DEBUG)
#define USENGINE_USE_VALIDATION_LAYERS 1
#else
#define USENGINE_USE_VALIDATION_LAYERS 0
#endif

namespace UnderStory {

namespace Vulkan {

class Engine {
 protected:
    void bindEngineToWindow(glfwm::WindowPointer &window) {
        this->_createInstance();
        this->_pickPhysicalDevice();
        this->_createSurface(window);
        this->_findQueuesAndCreateDevice();
        this->_createCommandBuffer();
    }

 private:
    vk::UniqueSurfaceKHR _surface;
    // create unique instance surface
    void _createSurface(glfwm::WindowPointer &window) {
        VkSurfaceKHR surface;
        window->createVulkanWindowSurface( VkInstance( this->_instance.get() ), nullptr, &surface);
        vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter( this->_instance.get() );
        this->_surface = vk::UniqueSurfaceKHR( vk::SurfaceKHR( surface ), deleter );
    }

    vk::UniqueInstance _instance;
    void _createInstance() {
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

    vk::UniqueCommandBuffer _commandBuffer;
    vk::UniqueCommandPool _commandPool;
    void _createCommandBuffer() {
        // create a UniqueCommandPool to allocate a CommandBuffer from
        this->_commandPool = this->_device->createCommandPoolUnique(
            vk::CommandPoolCreateInfo({}, this->_queuesIndexes.graphicsQueueFamilyIndex)
        );

        // allocate a CommandBuffer from the CommandPool
        vk::CommandBufferAllocateInfo cbai(this->_commandPool.get(), vk::CommandBufferLevel::ePrimary, 1 );
        this->_commandBuffer = std::move(this->_device->allocateCommandBuffersUnique(cbai).front());
    }

    vk::UniqueDevice _device;
    Utils::QueueIndexes _queuesIndexes;
    void _findQueuesAndCreateDevice() {
        // queues indexes
        this->_queuesIndexes = Utils::findGraphicsAndPresentQueueFamilyIndex(this->_physicalDevice, this->_surface.get());

        // create a UniqueDevice
        auto queuePriority = 0.0f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo({}, this->_queuesIndexes.graphicsQueueFamilyIndex, 1, &queuePriority);
        vk::DeviceCreateInfo deviceCreateInfo({}, 1, &deviceQueueCreateInfo);
        this->_device = this->_physicalDevice.createDeviceUnique(deviceCreateInfo);
    }

    vk::PhysicalDevice _physicalDevice;
    // TODO(amphaal) pick best device
    void _pickPhysicalDevice() {
        auto physicalDevices = this->_instance->enumeratePhysicalDevices();
        this->_physicalDevice = physicalDevices.front();
    }

    std::vector<const char*> _extensions;
    void _setRequiredExtensions(vk::InstanceCreateInfo &ici) {
        // from glfw extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // extension vector
        this->_extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // push ext utils if validation layer is required
        if(USENGINE_USE_VALIDATION_LAYERS) {
            this->_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

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
        if(!USENGINE_USE_VALIDATION_LAYERS) return;

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
        if(!USENGINE_USE_VALIDATION_LAYERS) return;

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
