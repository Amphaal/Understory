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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include <nuklear.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <chrono>
#include <string>

#include "src/base/understory.h"
#include "Utility.hpp"

#include "src/app/ui/nuklear_glfw_gl3.h"

#include <GLFWM/glfwm.hpp>
#include <rxcpp/rx.hpp>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

namespace UnderStory {

class Application : public glfwm::EventHandler, public glfwm::Drawable, public std::enable_shared_from_this<Application> {
 public:
    Application() {
        // init GLFW
        assert(glfwm::WindowManager::init());

        // define window flags
        glfwm::WindowManager::setHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwm::WindowManager::setHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwm::WindowManager::setHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwm::WindowManager::setHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        // create window and initial context
        this->_window = glfwm::WindowManager::createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, _windowName, this->getHandledEventTypes());
        glfwm::WindowManager::setWaitTimeout(0);
        this->_window->makeContextCurrent();

        // setup glew and opengl
        this->_updateViewportAndClear();
        glewExperimental = 1;
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to setup GLEW\n");
            exit(1);
        }

        // init nuklear
        this->_nk_ctx = nk_glfw3_init(&this->_nk_glfw, this->_window->glfwWindow, NK_GLFW3_INSTALL_CALLBACKS);
        nk_glfw3_font_stash_begin(&this->_nk_glfw, &this->_nk_atlas);
        nk_glfw3_font_stash_end(&this->_nk_glfw);

        // ui
        this->_defineWindowIcon();
    }

    ~Application() {
        // shutdown nuklear
        nk_glfw3_shutdown(&this->_nk_glfw);
    }

    void run() {
        // bind handlers
        this->_window->bindEventHandler(this->shared_from_this(), 0);    // 0 is the rank among all event handlers bound
        this->_window->bindDrawable(this->shared_from_this(), 0);       // 0 is the rank among all drawables bound

        // fps count
        rxcpp::observable<>::interval(std::chrono::milliseconds(200))
        .subscribe_on(rxcpp::observe_on_new_thread())
        .subscribe([&](int) {
            this->_updateFPSCount = true;
        });

        // loop
        glfwm::WindowManager::mainLoop();
        glfwm::WindowManager::terminate();
    }

 private:
    static inline char const *_windowName = APP_FULL_DENOM;
    static inline double _updateFPSCountEveryMs = 333;

    glfwm::WindowPointer _window;

    nk_font_atlas* _nk_atlas;
    nk_glfw _nk_glfw = {0};
    nk_context* _nk_ctx;
    nk_colorf _nk_bg {0.10f, 0.18f, 0.24f, 1.0f};
    int _winWidth;
    int _winHeight;

    bool _updateFPSCount = false;
    double _frameRenderDur_Ms;
    int _fpsEstimated;

    glfwm::EventBaseType getHandledEventTypes() const override {
        return static_cast<glfwm::EventBaseType>(glfwm::EventType::FRAMEBUFFERSIZE);
    }

    bool handle(const glfwm::EventPointer &e) override {
        switch(e->getEventType()) {
            case glfwm::EventType::FRAMEBUFFERSIZE : {
                this->draw(this->_window->getID());
                this->_window->swapBuffers();
                return true;
            }
            break;

            default : {
                return false;
            }
            break;
        }
    }

    void draw(const glfwm::WindowID id) override {
        auto start = std::chrono::steady_clock::now();

            this->_updateViewportAndClear();
            this->_drawUI();

        auto end = std::chrono::steady_clock::now();
        this->_frameRenderDur_Ms = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000;
        this->_fpsEstimated = 1000 / this->_frameRenderDur_Ms;

        this->_mayUpdateFPSDisplay();
    }

    void _drawUI() {
        nk_glfw3_new_frame(&this->_nk_glfw);

            if (nk_begin(this->_nk_ctx, "Demo", nk_rect(0, 0, 230, 250),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
                enum {EASY, HARD};
                static int op = EASY;
                static int property = 20;
                nk_layout_row_static(this->_nk_ctx, 30, 80, 1);
                if (nk_button_label(this->_nk_ctx, "button"))
                    fprintf(stdout, "button pressed\n");

                nk_layout_row_dynamic(this->_nk_ctx, 30, 2);
                if (nk_option_label(this->_nk_ctx, "easy", op == EASY)) op = EASY;
                if (nk_option_label(this->_nk_ctx, "hard", op == HARD)) op = HARD;

                nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                nk_property_int(this->_nk_ctx, "Compression:", 0, &property, 100, 10, 1);

                nk_layout_row_dynamic(this->_nk_ctx, 20, 1);
                nk_label(this->_nk_ctx, "background:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                if (nk_combo_begin_color(this->_nk_ctx, nk_rgb_cf(this->_nk_bg), nk_vec2(nk_widget_width(this->_nk_ctx), 400))) {
                    nk_layout_row_dynamic(this->_nk_ctx, 120, 1);
                    this->_nk_bg = nk_color_picker(this->_nk_ctx, this->_nk_bg, NK_RGBA);
                    nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                    this->_nk_bg.r = nk_propertyf(this->_nk_ctx, "#R:", 0, this->_nk_bg.r, 1.0f, 0.01f, 0.005f);
                    this->_nk_bg.g = nk_propertyf(this->_nk_ctx, "#G:", 0, this->_nk_bg.g, 1.0f, 0.01f, 0.005f);
                    this->_nk_bg.b = nk_propertyf(this->_nk_ctx, "#B:", 0, this->_nk_bg.b, 1.0f, 0.01f, 0.005f);
                    this->_nk_bg.a = nk_propertyf(this->_nk_ctx, "#A:", 0, this->_nk_bg.a, 1.0f, 0.01f, 0.005f);
                    nk_combo_end(this->_nk_ctx);
                }
            }
            nk_end(this->_nk_ctx);

        nk_glfw3_render(&this->_nk_glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    }

    void _mayUpdateFPSDisplay() {
        if(!this->_updateFPSCount) return;

        std::string title(_windowName);
        title += " - " + std::to_string(this->_fpsEstimated) + " FPS";
        this->_window->setTitle(title);

        this->_updateFPSCount = false;
    }

    void _defineWindowIcon() {
        #ifndef __APPLE__  // no window icon for OSX
            // define icon
            auto iconImage = Utility::getIcon();
            GLFWimage wIcon { iconImage.x, iconImage.y, iconImage.pixels };
            this->_window->setIcon(1, &wIcon);
        #endif
    }

    void _updateViewportAndClear() {
        this->_window->getSize(_winWidth, _winHeight);
        glViewport(0, 0, _winWidth, _winHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(this->_nk_bg.r, this->_nk_bg.g, this->_nk_bg.b, this->_nk_bg.a);
    }
};

}  // namespace UnderStory
