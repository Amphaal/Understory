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

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Platform/GlfwApplication.h>

#include <utility>

#include "src/base/understory.h"

#include "Utility.hpp"

using namespace Magnum;

namespace UnderStory {

class Application: public Platform::Application {
 public:
    explicit Application(const Arguments& arguments): Platform::Application{arguments} {
        this->setWindowTitle(APP_FULL_DENOM);
        this->defineIcon(Utility::getIcon());

        // GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        // GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::enable(GL::Renderer::Feature::Blending);

        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        using namespace Math::Literals;

        struct TriangleVertex {
            Vector2 position;
            Color3 color;
        };
        const TriangleVertex data[]{
            {{-0.5f, -0.5f}, 0xff0000_rgbf},    /* Left vertex, red color */
            {{ 0.5f, -0.5f}, 0x00ff00_rgbf},    /* Right vertex, green color */
            {{ 0.0f,  0.5f}, 0x0000ff_rgbf}     /* Top vertex, blue color */
        };

        GL::Buffer buffer;
        buffer.setData(data);

        _mesh.setCount(3)
            .addVertexBuffer(std::move(buffer), 0,
                Shaders::VertexColor2D::Position{},
                Shaders::VertexColor2D::Color3{});
    }

 private:
    GL::Mesh _mesh;
    Shaders::VertexColor2D _shader;

    void drawEvent() override {
        // GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

        _mesh.draw(_shader);

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
