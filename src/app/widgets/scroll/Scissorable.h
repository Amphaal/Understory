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

#include "src/app/widgets/base/Hoverable.hpp"

namespace UnderStory {

namespace Widget {

class ScrollablePanel;

class Scissorable {
 public:
    explicit Scissorable(ScrollablePanel* associatedPanel);

    void draw();
    void onMouseScroll(const Magnum::Vector2& scrollOffset);
    const Magnum::Matrix3& scrollMatrix() const;

 protected:
    virtual void _drawInbetweenScissor() = 0;

    void _bindToPanel();

    void _signalContentSizeChanged(const Magnum::Float& newContentSize);
    const Magnum::Matrix3& _panelMatrix() const;
    const GrowableAxis _growableAxis() const;

    void _updateScissorTarget(const Magnum::Range2D &geometry);

 private:
    ScrollablePanel* _associatedPanel;
    GrowableAxis _grwblAxis;
    Magnum::Matrix3 _scrollMatrix;
    const GrowableAxis _getGrowableAxis() const;

    Magnum::Range2Di _scissorTarget;

    static inline std::stack<Magnum::Range2Di> _scissorStack;

    void _applyScissor();
    void _undoScissor();

    static Magnum::Range2Di framebufferSize();
    static Magnum::Vector2i fromGLtoPixel(Magnum::Vector2 GLCoords);
};

}  // namespace Widget

}  // namespace UnderStory
