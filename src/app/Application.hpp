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

#include <GL/glew.h>
#include <GL/wglext.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <chrono>
#include <string>
#include <memory>

#include "src/base/understory.h"

#include "src/app/ui/debug/FrameTracker.hpp"
#include "src/app/ui/nuklear/Nuklear.hpp"
#include "src/app/ui/Engine.hpp"

#include "src/app/widgets/UpdateCheckerWidget.hpp"

#include "Utility.hpp"

#include <spdlog/spdlog.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

namespace UnderStory {

class Application : public glfwm::EventHandler, public glfwm::Drawable, public std::enable_shared_from_this<Application> {
 public:
    Application() : _engine(&this->_framebufferSize, &_pointerPos) {
        // init GLFW
        if(!glfwm::WindowManager::init()) throw std::exception();

        // define window flags
        glfwm::WindowManager::setHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwm::WindowManager::setHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwm::WindowManager::setHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // force core
        glfwm::WindowManager::setHint(GLFW_SAMPLES, 4);  // set antialiasing
        glfwm::WindowManager::setHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        #ifdef __APPLE__
            glfwm::WindowManager::setHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        try {
            // create window and initial context
            this->_window = glfwm::WindowManager::createWindow(
                WINDOW_WIDTH,
                WINDOW_HEIGHT,
                this->_windowName,
                this->getHandledEventTypes()
            );
        } catch(...) {
            const char* description;
            glfwGetError(&description);
            throw std::logic_error(description);
        }

        glfwm::WindowManager::setWaitTimeout(0);
        this->_window->makeContextCurrent();

        // setup glew and ext
        this->_glewSetup();

        // init nuklear
        this->_nuklear.init(this->_window->glfwWindow, &this->_backgroundColor);

        // init engine
        this->_engine.init();

        // ui
        this->_defineWindowIcon();
    }

    ~Application() {
        // terminate window
        glfwm::WindowManager::terminate();
    }

    void run() {
        // bind handlers
        this->_window->bindEventHandler(this->shared_from_this(), 0);  // 0 is the rank among all event handlers bound
        this->_window->bindDrawable(    this->shared_from_this(), 0);  // 0 is the rank among all drawables bound

        // fps count
        this->_fpsTracker.start([&](int fpsEstimated) {
            // update title
            std::string title(_windowName);
            title += " - " + std::to_string(fpsEstimated) + " FPS";
            this->_window->setTitle(title);
        });

        // only check for updates on Windows (TODO(amphaal) MacOS ?)
        #ifdef WIN32
            _updateChecker.start();
        #endif

        // loop
        glfwm::WindowManager::mainLoop();
    }

 private:
    #ifdef _DEBUG
        static inline std::string _windowName = APP_FULL_DENOM + std::string(" - DEBUG");
    #else
        static inline std::string _windowName = APP_FULL_DENOM;
    #endif

    glfwm::WindowPointer _window;
    Utility::Size _framebufferSize;
    glm::vec2 _pointerPos;
    nk_colorf _backgroundColor {0.10f, 0.18f, 0.24f, .5f};

    UI::Engine _engine;
    UI::Nuklear _nuklear;
    UI::FrameTracker _fpsTracker;

    Widget::UpdateCheckerWidget _updateChecker;

    glfwm::EventBaseType getHandledEventTypes() const override {
        return static_cast<glfwm::EventBaseType>(
            glfwm::EventType::FRAMEBUFFERSIZE |
            glfwm::EventType::KEY |
            glfwm::EventType::CURSOR_POSITION |
            glfwm::EventType::SCROLL
        );
    }

    bool handle(const glfwm::EventPointer &e) override {
        switch(e->getEventType()) {
            case glfwm::EventType::FRAMEBUFFERSIZE : {
                return _onFrameBufferSizeChanged();
            }
            break;

            case glfwm::EventType::KEY : {
                auto event = dynamic_cast<glfwm::EventKey*>(e.get());
                return this->_engine.onKeyPress(event);
            }
            break;

            case glfwm::EventType::CURSOR_POSITION : {
                auto event = dynamic_cast<glfwm::EventCursorPosition*>(e.get());
                _pointerPos = { event->getX(), event->getY() };
                return true;
            }
            break;

            case glfwm::EventType::SCROLL : {
                auto event = dynamic_cast<glfwm::EventScroll*>(e.get());
                return this->_engine.onScrollEvent(event);
            };
            break;

            default : {
                return false;
            }
            break;
        }
    }

    void draw(const glfwm::WindowID id) override {
        this->_fpsTracker.recordFrame();

            this->_clear();
            this->_engine.draw();

        this->_fpsTracker.endRecord();
    }

    //
    //
    //

    void _defineWindowIcon() {
        #ifndef __APPLE__  // no window icon for OSX
            // define icon
            auto iconImage = Utility::getIcon();
            GLFWimage wIcon { iconImage.s.width, iconImage.s.height, iconImage.pixels };
            this->_window->setIcon(1, &wIcon);
        #endif
    }

    void _reshape() {
        // use "getFramebufferSize" to handle high DPI screen
        this->_window->getFramebufferSize(_framebufferSize.width, _framebufferSize.height);

        auto &width = _framebufferSize.width;
        auto &height = _framebufferSize.height;

        // reset viewport to default
        glViewport(0, 0, width, height);
    }

    void _clear() {
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

    void _glewSetup() {
        // must be called before GLEW inst
        this->_reshape();

        // hard define exp flag
        glewExperimental = 1;

        // init
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to setup GLEW\n");
            exit(1);
        }

        // activate vsync
        glfwSwapInterval(1);

        // handles transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    bool _onFrameBufferSizeChanged() {
        this->_reshape();
        this->draw(this->_window->getID());
        this->_window->swapBuffers();
        return true;
    }
};

}  // namespace UnderStory
