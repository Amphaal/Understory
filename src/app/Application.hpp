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
#include <nuklear_glfw_gl3.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "src/base/understory.h"
#include "Utility.hpp"

#include <GLFWM/glfwm.hpp>

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
        this->_window = glfwm::WindowManager::createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_FULL_DENOM, this->getHandledEventTypes());
        this->_window->makeContextCurrent();

        // setup glew and opengl
        this->_updateViewportAndClear();
        glewExperimental = 1;
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to setup GLEW\n");
            exit(1);
        }

        // nuklear
        this->_nk_ctx = nk_glfw3_init(&this->_nk_glfw, this->_window->glfwWindow, NK_GLFW3_INSTALL_CALLBACKS);
        nk_glfw3_font_stash_begin(&_nk_glfw, &_nk_atlas);
        nk_glfw3_font_stash_end(&_nk_glfw);

        // ui
        this->_defineWindowIcon();
    }

    ~Application() {
        nk_glfw3_shutdown(&_nk_glfw);
    }

    void run() {
        this->_window->bindEventHandler(this->shared_from_this(), 0);    // 0 is the rank among all event handlers bound
        this->_window->bindDrawable(this->shared_from_this(), 0);       // 0 is the rank among all drawables bound

        glfwm::WindowManager::mainLoop();
        glfwm::WindowManager::terminate();
    }

 private:
    glfwm::WindowPointer _window;

    nk_font_atlas* _nk_atlas;
    nk_glfw _nk_glfw = {0};
    nk_context* _nk_ctx;
    nk_colorf _nk_bg {0.10f, 0.18f, 0.24f, 1.0f};
    int _winWidth;
    int _winHeight;

    void _defineWindowIcon() {
        #ifndef __APPLE__  // no window icon for OSX
            // define icon
            auto iconImage = Utility::getIcon();
            GLFWimage wIcon { iconImage.x, iconImage.y, iconImage.pixels };
            this->_window->setIcon(1, &wIcon);
        #endif
    }

    glfwm::EventBaseType getHandledEventTypes() const override {
        return static_cast<glfwm::EventBaseType>(glfwm::EventType::MOUSE_BUTTON | glfwm::EventType::CURSOR_POSITION);
    }

    bool handle(const glfwm::EventPointer &e) override {
        if (e->getEventType() == glfwm::EventType::MOUSE_BUTTON) {
            // TODO(amphaal) handle mouse
            return true;
        }
        return true;
    }

    void draw(const glfwm::WindowID id) override  {
        nk_glfw3_new_frame(&_nk_glfw);
        if (nk_begin(_nk_ctx, "Demo", nk_rect(0, 0, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(_nk_ctx, 30, 80, 1);
            if (nk_button_label(_nk_ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(_nk_ctx, 30, 2);
            if (nk_option_label(_nk_ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(_nk_ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(_nk_ctx, 25, 1);
            nk_property_int(_nk_ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(_nk_ctx, 20, 1);
            nk_label(_nk_ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(_nk_ctx, 25, 1);
            if (nk_combo_begin_color(_nk_ctx, nk_rgb_cf(_nk_bg), nk_vec2(nk_widget_width(_nk_ctx), 400))) {
                nk_layout_row_dynamic(_nk_ctx, 120, 1);
                _nk_bg = nk_color_picker(_nk_ctx, _nk_bg, NK_RGBA);
                nk_layout_row_dynamic(_nk_ctx, 25, 1);
                _nk_bg.r = nk_propertyf(_nk_ctx, "#R:", 0, _nk_bg.r, 1.0f, 0.01f, 0.005f);
                _nk_bg.g = nk_propertyf(_nk_ctx, "#G:", 0, _nk_bg.g, 1.0f, 0.01f, 0.005f);
                _nk_bg.b = nk_propertyf(_nk_ctx, "#B:", 0, _nk_bg.b, 1.0f, 0.01f, 0.005f);
                _nk_bg.a = nk_propertyf(_nk_ctx, "#A:", 0, _nk_bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(_nk_ctx);
            }
        }
        nk_end(_nk_ctx);

        this->_updateViewportAndClear();

        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
        * with blending, scissor, face culling, depth test and viewport and
        * defaults everything back into a default state.
        * Make sure to either a.) save and restore or b.) reset your own state after
        * rendering the UI. */
        nk_glfw3_render(&_nk_glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    }

    void _updateViewportAndClear() {
        this->_window->getSize(_winWidth, _winHeight);
        glViewport(0, 0, _winWidth, _winHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(_nk_bg.r, _nk_bg.g, _nk_bg.b, _nk_bg.a);
    }
};

}  // namespace UnderStory
