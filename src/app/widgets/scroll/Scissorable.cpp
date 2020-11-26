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
    Scrollable(associatedPanel->stickyness()),
    _associatedPanel(associatedPanel) {}

void UnderStory::Widget::Scissorable::_applyScissor() {
    _scissorStack.push(_scissorTarget);
    Magnum::GL::Renderer::setScissor(_scissorTarget);
}

void UnderStory::Widget::Scissorable::_draw() {
    // TODO(amphaal) use scissors
    // _applyScissor();
        _drawInbetweenScissor();
    // _undoScissor();
}

void UnderStory::Widget::Scissorable::_scrollCanvasChanged(const Magnum::Range2D &canvas) {
    _canvasSize = _extractScrollableSize(canvas);
}

void UnderStory::Widget::Scissorable::_scrollContentChanged(const Magnum::Range2D& content) {
    // reset scroll matrix
    _translationFactor = 0.f;
    _animateScrollByTr(_translationFactor);
    
    // update cached content size
    _contentSize = _extractScrollableSize(content);

    // define content ratio and pass it to the scroller
    auto contentRatio = _canvasSize / _contentSize;
    _associatedPanel->scroller().onContentRatioChanged(contentRatio);
}

float UnderStory::Widget::Scissorable::_scrollTick() const {
    return 1.f;
}

float UnderStory::Widget::Scissorable::_scrollByOffset(const Magnum::Vector2& scrollOffset) {   
    // apply factor
    auto step = scrollOffset.y() * _scrollTick();
    return _scrollByContentSize(-step, true);
}

float UnderStory::Widget::Scissorable::_scrollByPercentTick(Magnum::Float prc) {
    auto step = (_contentSize - _canvasSize) * prc;
    return _scrollByContentSize(step, false);
}

float UnderStory::Widget::Scissorable::_scrollByContentSize(Magnum::Float contentSizeTick, bool animate) {
    _translationFactor += contentSizeTick;
    auto maxTranslate = _contentSize - _canvasSize;

    // stop at min/max
    if(_translationFactor < 0) {   // prevent backward
        _translationFactor = 0;
    }
    else if(_translationFactor > maxTranslate) {   // prevent going too far
        _translationFactor = maxTranslate; 
    }
    
    // update scroll matrix
    if(animate) {
        _animateScrollByTr(_translationFactor);
    } else {
        _setAnimationKeyframe(_translationFactor);
        _onAnimationProgress();
    }
    
    // prc eq
    auto scrollPrc = _translationFactor / maxTranslate;
    return scrollPrc;
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
