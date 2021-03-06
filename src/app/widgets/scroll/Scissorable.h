// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020-2021 Guillaume Vara <guillaume.vara@gmail.com>

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

#include "Scrollable.hpp"
#include "src/app/widgets/base/Hoverable.hpp"

namespace UnderStory {

namespace Widget {

class ScrollablePanel;

class Scissorable : public Scrollable {
 friend class ScrollablePanel;

 public:
    explicit Scissorable(ScrollablePanel* associatedPanel);

 protected:
    void _draw();
    virtual void _drawInbetweenScissor() = 0;
    virtual float _scrollTick() const;
   
    // returns progression as percent relative to min-max poles
    float _scrollByOffset(const Magnum::Vector2& scrollOffset);
    float _scrollByPercentTick(Magnum::Float prc);

    void _scrollContentChanged(const Magnum::Range2D& content);
    void _scrollCanvasChanged(const Magnum::Range2D &canvas);

    void _updateScissorTarget(const Magnum::Range2D &geometry);

 private:
    ScrollablePanel* _associatedPanel;
    
    Magnum::Float _translationFactor = 0.f;
    Magnum::Float _canvasSize;
    Magnum::Float _contentSize;

    Magnum::Range2Di _scissorTarget;

    static inline std::stack<Magnum::Range2Di> _scissorStack;

    void _applyScissor();
    void _undoScissor();

    float _scrollByContentSize(Magnum::Float contentSizeTick, bool animate);

    static Magnum::Range2Di framebufferSize();
    static Magnum::Vector2i fromGLtoPixel(Magnum::Vector2 GLCoords);
};

}  // namespace Widget

}  // namespace UnderStory
