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

#include <Magnum/Magnum.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/DefaultFramebuffer.h>

#include <stack>

namespace UnderStory {

namespace Widget {

class Scissorer {
 public:
    Scissorer() {}

 protected:
    void _applyScissor() {
        _scissorStack.push(_scissorTarget);
        Magnum::GL::Renderer::setScissor(_scissorTarget);
    }

    void _undoScissor() {
        _scissorStack.pop();
        Magnum::GL::Renderer::setScissor(
            _scissorStack.size() ? _scissorStack.top() : framebufferSize()
        );
    }

    void _updateScissorTarget(const Magnum::Range2D &geometry) {
        _scissorTarget = {
            fromGLtoPixel(geometry.topLeft()),
            fromGLtoPixel(geometry.bottomRight())
        };
        Magnum::Debug{} << _scissorTarget;
    }

 private:
    Magnum::Range2Di _scissorTarget;

    static inline std::stack<Magnum::Range2Di> _scissorStack;

    static Magnum::Range2Di framebufferSize() {
        return Magnum::GL::defaultFramebuffer.viewport();
    }

    static Magnum::Vector2i fromGLtoPixel(Magnum::Vector2 GLCoords) {
        // reverse Y-axis
        GLCoords.y() *= -1.f;

        // normalize
        GLCoords = (GLCoords + Magnum::Vector2{1.f}) / Magnum::Vector2{2.f};

        // apply to framebuffer size
        GLCoords *= Magnum::Vector2(framebufferSize().max());

        return {
            static_cast<int>(GLCoords.x()),
            static_cast<int>(GLCoords.y())
        };
    }
};

}  // namespace Widget

}  // namespace UnderStory
