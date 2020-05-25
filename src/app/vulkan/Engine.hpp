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

#define GLFW_INCLUDE_VULKAN
#include <GLFWM/glfwm.hpp>

#include <vulkan/vulkan.hpp>

#include "src/base/understory.h"

namespace UnderStory {

namespace Vulkan {

class Engine {
 public:
    Engine() {
        // initialize the vk::ApplicationInfo structure
        auto appVersion = VK_MAKE_VERSION(APP_MAJOR_VERSION, APP_MINOR_VERSION, APP_PATCH_VERSION);
        vk::ApplicationInfo applicationInfo( APP_FULL_DENOM, appVersion, APP_ENGINE_NAME, appVersion, VK_API_VERSION_1_2 );

        // initialize the vk::InstanceCreateInfo
        vk::InstanceCreateInfo instanceCreateInfo( {}, &applicationInfo );
        this->_setRequiredExtensions(instanceCreateInfo);

        // create a UniqueInstance
        this->_instance = std::move(vk::createInstanceUnique(instanceCreateInfo));
    }

 private:
    vk::UniqueInstance _instance;

    void _setRequiredExtensions(vk::InstanceCreateInfo &ici) {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        ici.enabledExtensionCount = glfwExtensionCount;
        ici.ppEnabledExtensionNames = glfwExtensions;
    }
};

}  // namespace Vulkan

}  // namespace UnderStory
