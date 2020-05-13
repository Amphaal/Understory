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

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/GlfwApplication.h>

#include "src/base/understory.h"

#include "Utility.hpp"

namespace UnderStory {

class Application: public Magnum::Platform::Application {
 public:
    explicit Application(const Arguments& arguments): Magnum::Platform::Application{arguments} {
        this->setWindowTitle(APP_FULL_DENOM);
        this->defineIcon(Utility::getIcon());
    }

 private:
    void drawEvent() override {
        Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

        /* TODO: Add your drawing code here */

        swapBuffers();
    }

    void defineIcon(const Utility::RawImage &iconImage) {
        // cast
        GLFWimage wIcon { iconImage.x, iconImage.y, iconImage.pixels };
        // define
        glfwSetWindowIcon(this->window(), 1, &wIcon);
        // clear
        delete iconImage.pixels;
    }
};

}  // namespace UnderStory
