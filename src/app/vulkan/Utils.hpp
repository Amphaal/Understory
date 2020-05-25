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

#include <limits>
#include <vector>

#include <vulkan/vulkan.hpp>

class Utils {
 public:
    struct QueueIndexes {
        uint32_t graphicsQueueFamilyIndex;
        uint32_t presentQueueFamilyIndex;
    };

    static QueueIndexes findGraphicsAndPresentQueueFamilyIndex(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR const & surface) {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        assert( queueFamilyProperties.size() < std::numeric_limits<uint32_t>::max() );

        auto graphicsQueueFamilyIndex = _findGraphicsQueueFamilyIndex( queueFamilyProperties );
        if ( physicalDevice.getSurfaceSupportKHR(graphicsQueueFamilyIndex, surface) ) {
            return {
                graphicsQueueFamilyIndex,
                graphicsQueueFamilyIndex
            };  // the first graphicsQueueFamilyIndex does also support presents
        }

        // the graphicsQueueFamilyIndex doesn't support present -> look for an other family index that supports both
        // graphics and present
        for ( size_t i = 0; i < queueFamilyProperties.size(); i++ ) {
            auto surfaceSupport = physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface);
            if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) && surfaceSupport) {
                return {static_cast<uint32_t>(i), static_cast<uint32_t>(i)};
            }
        }

        // there's nothing like a single family index that supports both graphics and present -> look for an other family
        // index that supports present
        for (size_t i = 0; i < queueFamilyProperties.size(); i++)  {
            auto surfaceSupport = physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface);
            if (surfaceSupport) {
                return {graphicsQueueFamilyIndex, static_cast<uint32_t>(i)};
            }
        }

        throw std::runtime_error( "Could not find queues for both graphics or present -> terminating" );
    }

 private:
    static uint32_t _findGraphicsQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties> &queueFamilyProperties) {
        // get the first index into queueFamiliyProperties which supports graphics
        auto graphicsQueueFamilyIndex = std::distance(
            queueFamilyProperties.begin(),
            std::find_if( queueFamilyProperties.begin(), queueFamilyProperties.end(), []( vk::QueueFamilyProperties const & qfp ) {
                return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
            })
        );

        assert( graphicsQueueFamilyIndex < queueFamilyProperties.size() );
        return graphicsQueueFamilyIndex;
    }
};
