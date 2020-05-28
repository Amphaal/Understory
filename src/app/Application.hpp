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

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "src/base/understory.h"
#include "Utility.hpp"

#include <GLFWM/glfwm.hpp>

namespace UnderStory {

class Application : public glfwm::EventHandler, public glfwm::Drawable, public std::enable_shared_from_this<Application> {
 public:
    Application() {
        assert(glfwm::WindowManager::init());

        glfwm::WindowManager::setHint(GLFW_CLIENT_API, GLFW_NO_API);  // remove OpenGL API handling

        this->_window = glfwm::WindowManager::createWindow(800, 600, APP_FULL_DENOM, this->getHandledEventTypes());

        #ifndef __APPLE__  // no window icon for OSX
            // define icon
            auto iconImage = Utility::getIcon();
            GLFWimage wIcon { iconImage.x, iconImage.y, iconImage.pixels };
            this->_window->setIcon(1, &wIcon);
        #endif
    }

    void run() {
        this->_window->bindEventHandler(this->shared_from_this(), 0);    // 0 is the rank among all event handlers bound
        this->_window->bindDrawable(this->shared_from_this(), 0);       // 0 is the rank among all drawables bound

        glfwm::WindowManager::mainLoop();
        glfwm::WindowManager::terminate();
    }

 private:
    glfwm::WindowPointer _window;

    glfwm::EventBaseType getHandledEventTypes() const override {
        return static_cast<glfwm::EventBaseType>(glfwm::EventType::MOUSE_BUTTON);
    }

    bool handle(const glfwm::EventPointer &e) override {
        if (e->getEventType() == glfwm::EventType::MOUSE_BUTTON) {
            // TODO(amphaal) handle mouse
            return true;
        }
        return false;
    }

    void draw(const glfwm::WindowID id) override  {
        // TODO(amphaal) handle drawing
    }
};

}  // namespace UnderStory
