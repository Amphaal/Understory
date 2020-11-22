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

#include "Scissorable.h"
#include "ScrollablePanel.h"

UnderStory::Widget::Scissorable::Scissorable(ScrollablePanel* associatedPanel) :
    _associatedPanel(associatedPanel), _grwblAxis(_getGrowableAxis()) {}

void UnderStory::Widget::Scissorable::_applyScissor() {
    _scissorStack.push(_scissorTarget);
    Magnum::GL::Renderer::setScissor(_scissorTarget);
}

void UnderStory::Widget::Scissorable::_draw() {
    // TODO
    // _applyScissor();
        _drawInbetweenScissor();
    // _undoScissor();
}

void UnderStory::Widget::Scissorable::_translateView(const Magnum::Vector2& scrollOffset) {   
    // TODO stop at min/max
    // _scrollMatrix = _scrollMatrix *
    //     Magnum::Matrix3::translation(
    //         _axisFn(-scrollOffset.y() / 10)
    //     );
}

const Magnum::Matrix3& UnderStory::Widget::Scissorable::_scrollMatrix() const {
    return _scrllMatrix;
}

const UnderStory::Widget::GrowableAxis UnderStory::Widget::Scissorable::_growableAxis() const {
    return _grwblAxis;
}

void UnderStory::Widget::Scissorable::_signalContentSizeChanged(const Magnum::Float& newContentSize) {
    _associatedPanel->scroller().onContentSizeChanged(newContentSize);
}

const UnderStory::Widget::GrowableAxis UnderStory::Widget::Scissorable::_getGrowableAxis() const {
    switch (_associatedPanel->stickyness()) {
        case StickTo::Left :
        case StickTo::Right :
            return GrowableAxis::Height;
        case StickTo::Top :
        case StickTo::Bottom :
            return GrowableAxis::Width;
    }
}

void UnderStory::Widget::Scissorable::_undoScissor() {
    _scissorStack.pop();
    Magnum::GL::Renderer::setScissor(
        _scissorStack.size() ? _scissorStack.top() : framebufferSize()
    );
}

void UnderStory::Widget::Scissorable::_updateScissorTarget(const Magnum::Range2D &geometry) {
    _scissorTarget = {
        fromGLtoPixel(geometry.topLeft()),
        fromGLtoPixel(geometry.bottomRight())
    };
}

Magnum::Range2Di UnderStory::Widget::Scissorable::framebufferSize() {
    return Magnum::GL::defaultFramebuffer.viewport();
}

Magnum::Vector2i UnderStory::Widget::Scissorable::fromGLtoPixel(Magnum::Vector2 GLCoords) {
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
