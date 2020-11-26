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

#include "ScrollerHandle.h"
#include "ScrollablePanel.h"

UnderStory::Widget::ScrollerHandle::ScrollerHandle(ScrollablePanel* panel, StickTo scrollerStickyness) : Scrollable(scrollerStickyness), _panel(panel) {
    _setup();
}

void UnderStory::Widget::ScrollerHandle::draw() {
    Shaders::rounded
        ->setProjectionMatrix(_matrix)
        .setRectPx(_geomScrollerPx)
        .setColor(_scrollerColor)
        .draw(_meshScroller);
}

void UnderStory::Widget::ScrollerHandle::animateByPercentage(Magnum::Float percentTranslate) {
    auto tr = - _translationGap * percentTranslate;
    _animateScrollByTr(tr);
}

void UnderStory::Widget::ScrollerHandle::inplaceByPercentage(Magnum::Float percentTranslate) {
    auto tr = - _translationGap * percentTranslate;
    _setAnimationKeyframe(tr);
    _onAnimationProgress();
}

// updates handle size
void UnderStory::Widget::ScrollerHandle::updateSize(const Magnum::Float& handleSize, const Magnum::Float& trGap) {
    //
    _translationGap = trGap;
    
    // determine new scroller size
    auto scrollerShape = shape();
    switch (_growableAxis) {
        case GrowableAxis::Height :
            scrollerShape.min().y() = scrollerShape.topLeft().y() - handleSize;
            break;
        case GrowableAxis::Width :
            scrollerShape.max().x() = scrollerShape.topLeft().x() + handleSize;
            break;
    }

    // set scroller vertices
    _verticesScroller[0].position = scrollerShape.bottomLeft();
    _verticesScroller[1].position = scrollerShape.bottomRight();
    _verticesScroller[2].position = scrollerShape.topRight();
    _verticesScroller[3].position = scrollerShape.topLeft();

    // update shape
    _updateShape(scrollerShape);

    // update its geometry
    animateByPercentage(0.f);

    // ... then finally update buffer
    _bufferScroller.setSubData(0, _verticesScroller);
}

void UnderStory::Widget::ScrollerHandle::_onToggled(bool isToggled) {
    Button::_onToggled(isToggled);

    _updateScrollColor();
}

void UnderStory::Widget::ScrollerHandle::_onAnimationProgress() {
    Scrollable::_onAnimationProgress();
    _updateGeometry();
}

// if mouse is over placeholder
void UnderStory::Widget::ScrollerHandle::_onHoverChanged(bool isHovered) {
    if(isPreToggled()) return;

    _updateScrollColor();
}

void UnderStory::Widget::ScrollerHandle::handleLockMoveEvent(MouseMove_EH::EventType &event) {
    // skip if scroll movement is zero
    auto pixelSizeMove = _extractScrollableSize(event.relativePosition());
    if(!pixelSizeMove) return;

    // find tick size
    auto scrollPixelSize = _extractScrollableSize(constraints().pixelSize());
    auto tickSize = pixelSizeMove * scrollPixelSize * 2.f;

    // pass tick as content size
    auto prcHandleTick = tickSize / _translationGap;
    _panel->scrollFromHandle(prcHandleTick);
}

void UnderStory::Widget::ScrollerHandle::_availableSpaceChanged(Magnum::Range2D& availableSpace) {
    // just set the shape as ph shape for now
    _updateShape(availableSpace);
}

void UnderStory::Widget::ScrollerHandle::_updateGeometry() {
    // update geometry and pixel geometry
    if(_parentMatrix) _matrix = *_parentMatrix * _scrollMatrix();
    Hoverable::_updateGeometry(_matrix);
    _geomScrollerPx = _shapeIntoPixel(geometry());
}

void UnderStory::Widget::ScrollerHandle::_updateScrollColor() {
    _scrollerColor = isHovered() ? SCRLL_COLOR_ACTIVE : SCRLL_COLOR_IDLE;
}

const Magnum::Matrix3* UnderStory::Widget::ScrollerHandle::_matrixUpdateRequested(const Magnum::Matrix3* parentMatrix) {
    _parentMatrix = parentMatrix;
    _updateGeometry();
    return parentMatrix;
}

void UnderStory::Widget::ScrollerHandle::_setup() {
    // set default color
    _updateScrollColor();

    // define indices
    Magnum::GL::Buffer scrollerIndices;
    scrollerIndices.setData({
        0, 1, 2,
        2, 3, 0
    });

    // bind buffer
    _bufferScroller.setData(_verticesScroller, Magnum::GL::BufferUsage::DynamicDraw);

    // define meshes
    _meshScroller.setCount(scrollerIndices.size())
            .setIndexBuffer (std::move(scrollerIndices),    0, Magnum::MeshIndexType::UnsignedInt)
            .addVertexBuffer(_bufferScroller,               0, Shader::Rounded::Position{});
}
