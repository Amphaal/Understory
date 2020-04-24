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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <understory.h>

#include <string>

#include <cmrc/cmrc.hpp>  // resources

CMRC_DECLARE(appResources);

class UnderStoryApplication {
 public:
    void run() {
        _initWindow();
        _mainLoop();
        _cleanup();
    }

 private:
    GLFWwindow *_window = nullptr;

    void _initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(800, 600, APP_NAME, nullptr, nullptr);
    }

    void _initIcon() {
        // load from resources
        auto iconF = cmrc::appResources::get_filesystem().open("logo.png");
        std::string icon{ iconF.begin(), iconF.end()};
        int x, y, n;
        auto logoAsBMP = stbi_load_from_memory((const unsigned char *)icon.c_str(), icon.length(), &x, &y, &n, 4);

        // fill struct
        GLFWimage wIcon;
        wIcon.pixels = logoAsBMP;
        wIcon.height = y;
        wIcon.width = x;

        // define
        glfwSetWindowIcon(_window, 1, &wIcon);
    }

    void _mainLoop() {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void _cleanup() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
};
