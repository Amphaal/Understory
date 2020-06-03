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

#include <string>

#include <GLFWM/glfwm.hpp>
#include "src/app/ui/Nuklear.hpp"

namespace UnderStory {

namespace Widget {

class Modal : public glfwm::EventHandler, public glfwm::Drawable, public std::enable_shared_from_this<Modal> {
 public:
    Modal(const glfwm::WindowPointer &parent, const std::string &title, const std::string &text) : _text(text) {
        // create window and initial context
        this->_window = glfwm::WindowManager::createWindow(
            400,
            200,
            title,
            this->getHandledEventTypes(),
            nullptr,
            parent
        );

        this->_window->setAttribute(GLFW_FLOATING, GLFW_TRUE);
        this->_window->setAttribute(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        this->_window->setAttribute(GLFW_RESIZABLE, GLFW_FALSE);
    }

    void init() {
        this->_window->bindEventHandler(this->shared_from_this(), 0);  // 0 is the rank among all event handlers bound
        this->_window->bindDrawable(    this->shared_from_this(), 0);  // 0 is the rank among all drawables bound

        // init nuklear
        this->_nuklear.init(this->_window->glfwWindow, &this->_backgroundColor);
    }

 private:
    glfwm::WindowPointer _window;
    nk_colorf _backgroundColor {0.10f, 0.18f, 0.24f, 1.0f};

    UI::Nuklear _nuklear;

    std::string _text;

    glfwm::EventBaseType getHandledEventTypes() const override {
        return static_cast<glfwm::EventBaseType>(glfwm::EventType::EMPTY);
    }

    bool handle(const glfwm::EventPointer &e) override {
        return true;
    }

    void draw(const glfwm::WindowID id) override {
        this->_updateViewportAndClear();
        this->_nuklear.drawModal(_text);
    }

    void _updateViewportAndClear() {
        // clear frame
        glClear(GL_COLOR_BUFFER_BIT);
        // define clear color
        glClearColor(
            this->_backgroundColor.r,
            this->_backgroundColor.g,
            this->_backgroundColor.b,
            this->_backgroundColor.a
        );
    }
};

}  // namespace Widget

}  // namespace UnderStory
