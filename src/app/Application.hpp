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

#include <SDL2/SDL.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <chrono>
#include <string>
#include <memory>

#include "src/base/understory.h"

#include "src/app/ui/debug/FrameTracker.hpp"
#include "src/app/ui/Engine.hpp"

#include "src/app/widgets/UpdateCheckerWidget.hpp"

#include "Utility.hpp"

#include <spdlog/spdlog.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

namespace UnderStory {

class Application : public std::enable_shared_from_this<Application> {
 public:
    Application() {
        // init SDL2
        SDL_SetMainReady();
        if(SDL_Init(SDL_INIT_VIDEO)) throw std::logic_error(SDL_GetError());

        // inst window
        _window = SDL_CreateWindow(APP_FULL_DENOM, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
        if (!_window) {
            SDL_Quit();
            throw std::logic_error(SDL_GetError());
        }

        // setup glew and ext
        this->_glewSetup();

        // init engine
        this->_engine.init();
    }

    ~Application() {}

    void run() {
        // fps count
        this->_fpsTracker.start([&](int fpsEstimated) {
            // update title
            std::string title(_windowName);
            title += " - " + std::to_string(fpsEstimated) + " FPS";
            SDL_SetWindowTitle(_window, title.c_str());
        });

        // only check for updates on Windows (TODO(amphaal) MacOS ?)
        #ifdef WIN32
            _updateChecker.start();
        #endif

        // loop
        while(1) {
            _processEvents();
            _draw();
        }
    }

 private:
    #ifdef _DEBUG
        static inline std::string _windowName = APP_FULL_DENOM + std::string(" - DEBUG");
    #else
        static inline std::string _windowName = APP_FULL_DENOM;
    #endif

    SDL_Window* _window;

    Utility::Size _framebufferSize;
    glm::vec4 _backgroundColor {0.10f, 0.18f, 0.24f, .5f};

    UI::Engine _engine;
    UI::FrameTracker _fpsTracker;

    Widget::UpdateCheckerWidget _updateChecker;

    void _processEvents() {
        /* Our SDL event placeholder. */
        SDL_Event event;

        /* Grab all the events off the queue. */
        while( SDL_PollEvent(&event)) {
            switch( event.type ) {
            case SDL_KEYDOWN:
                /* Handle key presses. */
                _processKeyDown(&event.key.keysym);
                break;
            case SDL_QUIT:
                SDL_Quit();
                exit(0);
                break;
            }
        }
    }

    void _processKeyDown(SDL_Keysym *key) {
        this->_engine.onKeyPress(key);
    }

    void _draw() {
        this->_fpsTracker.recordFrame();

            this->_clear();
            this->_engine.draw(this->_framebufferSize);

        this->_fpsTracker.endRecord();
    }

    //
    //
    //

    void _reshape() {
        SDL_GL_GetDrawableSize(_window, &_framebufferSize.width, &_framebufferSize.height);

        auto &width = _framebufferSize.width;
        auto &height = _framebufferSize.height;

        // Compute aspect ratio of the new window
        if (height == 0) height = 1;                // To prevent divide by 0
        auto aspect = (GLfloat)width / (GLfloat)height;

        // reset viewport to default
        glViewport(0, 0, width, height);

        // Set the aspect ratio of the clipping area to match the viewport
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0f, width, height, 0.0f);
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
        auto context = SDL_GL_CreateContext(_window);
        if(!context) {
            throw std::logic_error(SDL_GetError());
        }

        SDL_GL_MakeCurrent(_window, context);
        this->_reshape();

        // hard define exp flag
        glewExperimental = 1;

        // init
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to setup GLEW\n");
            exit(1);
        }

        // define vsync
        SDL_GL_SetSwapInterval(1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    bool _onFrameBufferSizeChanged() {
        this->_reshape();
        this->_draw();
        SDL_GL_SwapWindow(_window);
        return true;
    }
};

}  // namespace UnderStory
