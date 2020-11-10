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

#include "Scroller.h"
#include "src/app/widgets/scroll/ScrollablePanel.h"

UnderStory::Widget::Scroller::Scroller(const ScrollablePanel* panel) : _associatedPanel(panel), _stickness(_scrollerStickyness()) {
    _setup();
}

void UnderStory::Widget::Scroller::mayDraw() {
    //
    if(!_contentBigEnough) return;

    //
    Shaders::color
        ->setTransformationProjectionMatrix(_associatedPanel->matrix())
        .draw(_mesh);
}

void UnderStory::Widget::Scroller::onMouseScroll(const Magnum::Vector2& scrollOffset) {
    // TODO
}

// scroller position within panel
const UnderStory::Widget::StickTo UnderStory::Widget::Scroller::_scrollerStickyness() const {
    switch (_associatedPanel->stickyness()) {
        case StickTo::Left :
            return StickTo::Right;
        case StickTo::Top :
            return StickTo::Bottom;
        case StickTo::Right :
            return StickTo::Right;
        case StickTo::Bottom :
            return StickTo::Bottom;
    }
}

void UnderStory::Widget::Scroller::onContentSizeChanged(const Magnum::Float& newContentSize) {
    // determine placeholder size
    Magnum::Float phSize;
    switch (_stickness) {
        case StickTo::Left :
        case StickTo::Right :
            phSize = _phShape.y().size();
            break;
        case StickTo::Top :
        case StickTo::Bottom :
            phSize = _phShape.x().size();
            break;
    }

    // check if scroller is required
    Magnum::Float prcSize = 1.f;
    if(phSize && newContentSize) {
        prcSize = phSize / newContentSize;
    }
    _contentBigEnough = prcSize < 1.f;
    if(!_contentBigEnough) return;
    auto scrollerSize = phSize * prcSize;

    // determine new scroller size
    auto scrollerShape = _phShape;
    switch (_stickness) {
        case StickTo::Left :
        case StickTo::Right :
            scrollerShape.min().y() = scrollerShape.topLeft().y() - scrollerSize;
            break;
        case StickTo::Top :
        case StickTo::Bottom :
            scrollerShape.max().x() = scrollerShape.topLeft().x() + scrollerSize;
            break;
    }

    // set scroller vertices
    _vertices[4].position = scrollerShape.bottomLeft();
    _vertices[5].position = scrollerShape.bottomRight();
    _vertices[6].position = scrollerShape.topRight();
    _vertices[7].position = scrollerShape.topLeft();

    // update scroller shape...
    _scrollerShape = {
        _vertices[4].position,
        _vertices[6].position
    };

    // ... and state...
    _updateScrollColor();

    // ... then finally update buffer
    _buffer.setSubData(0, _vertices);
}

void UnderStory::Widget::Scroller::reveal() {
    // TODO
}

void UnderStory::Widget::Scroller::fade() {
    // TODO
}

void UnderStory::Widget::Scroller::_updateGeometry() {
    // update geometry
    Hoverable::_updateGeometry(_associatedPanel->matrix());
}

// if mouse is over placeholder
void UnderStory::Widget::Scroller::_mouseIsOver(const Magnum::Vector2 &cursorPos) {
    // check if over scroller
    auto isScrollerHovered = _scrollerShape.contains(cursorPos);
    if(isScrollerHovered == _isScrollerHovered) return;

    // change state
    _isScrollerHovered = isScrollerHovered;
    _updateScrollColor();

    // update buffer
    _buffer.setSubData(0, _vertices);
}

void UnderStory::Widget::Scroller::_availableSpaceChanged(Magnum::Range2D& availableSpace) {
    // start and begin of scroller placeholder
    auto &pixelSize = constraints().pixelSize();
    auto ph = availableSpace;

    // size with padding
    auto size = THICKNESS_PX + PADDING_PX * 2;

    // define space of placeholder
    // and update remaining space
    switch (_stickness) {
        case StickTo::Left :
            ph.max().x() = ph.min().x() + (pixelSize.x() * size);
            availableSpace.min().x() = ph.max().x();
            break;
        case StickTo::Top :
            ph.min().y() = ph.max().y() - (pixelSize.y() * size);
            availableSpace.max().y() = ph.min().y();
            break;
        case StickTo::Right :
            ph.min().x() = ph.max().x() - (pixelSize.x() * size);
            availableSpace.max().x() = ph.min().x();
            break;
        case StickTo::Bottom :
            ph.max().y() = ph.min().y() + (pixelSize.y() * size);
            availableSpace.min().y() = ph.max().y();
            break;
    }

    // update shape with placeholder
    this->_updateShape(ph);

    // reduce size with padding
    {
        Magnum::Range2D phMarginless = ph;
        phMarginless = ph.padded({
            - (pixelSize.x() * PADDING_PX),
            - (pixelSize.y() * PADDING_PX)
        });
        _phShape = phMarginless;
    }

    // placeholder
    _vertices[0] = {_phShape.bottomLeft(),     PH_COLOR};
    _vertices[1] = {_phShape.bottomRight(),    PH_COLOR};
    _vertices[2] = {_phShape.topRight(),       PH_COLOR};
    _vertices[3] = {_phShape.topLeft(),        PH_COLOR};
    _buffer.setSubData(0, _vertices);

    //
    _updateGeometry();
}

void UnderStory::Widget::Scroller::_updateScrollColor() {
    Magnum::Color4 color = _isScrollerHovered ? SCRLL_COLOR_ACTIVE : SCRLL_COLOR_IDLE;
    _vertices[4].color = color;
    _vertices[5].color = color;
    _vertices[6].color = color;
    _vertices[7].color = color;
}

void UnderStory::Widget::Scroller::_setup() {
    // define indices
    Magnum::GL::Buffer bIndices;
    bIndices.setData({
        // placeholder
        0, 1, 2,
        2, 3, 0,
        // scroller
        4, 5, 6,
        6, 7, 4
    });

    // bind buffer
    _buffer.setData(_vertices, Magnum::GL::BufferUsage::DynamicDraw);

    // define panel mesh
    _mesh.setCount(bIndices.size())
            .setIndexBuffer (std::move(bIndices),   0, Magnum::MeshIndexType::UnsignedInt)
            .addVertexBuffer(_buffer,               0, Magnum::Shaders::Flat2D::Position{},
                                                        Magnum::Shaders::Flat2D::Color4{}
            );
}
