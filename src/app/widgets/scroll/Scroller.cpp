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

#include "Scroller.h"
#include "src/app/widgets/scroll/ScrollablePanel.h"

UnderStory::Widget::Scroller::Scroller(ScrollablePanel* panel) :
    _associatedPanel(panel),
    _stickness(_stickynessFromPanel(panel)), 
    _handle(panel, _stickynessFromPanel(panel)) {
    _initContaining({&_handle});
    _setup();
}

void UnderStory::Widget::Scroller::mayDraw() {
    //
    if(!_contentBigEnough) return;

    // ph
    Shaders::rounded
        ->setProjectionMatrix(_matrix)
        .setRectPx(_geomPhPx)
        .setColor(PH_COLOR)
        .draw(_meshPh);

    // handle
    _handle.draw();
}

UnderStory::Widget::ScrollerHandle& UnderStory::Widget::Scroller::handle() {
    return _handle;
}

// scroller position within panel
UnderStory::Widget::StickTo UnderStory::Widget::Scroller::_stickynessFromPanel(const ScrollablePanel* panel) {
    switch (panel->stickyness()) {
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

void UnderStory::Widget::Scroller::onContentRatioChanged(const Magnum::Float& contentRatio) {
    // determine placeholder size
    Magnum::Float phSize;
    switch (_stickness) {
        case StickTo::Left :
        case StickTo::Right :
            phSize = shape().y().size();
            break;
        case StickTo::Top :
        case StickTo::Bottom :
            phSize = shape().x().size();
            break;
    }

    // check if scroller is required
    _contentBigEnough = contentRatio < 1.f;
    if(!_contentBigEnough) return;

    // signals handle to update its size
    auto handleSize = phSize * contentRatio;
    _handle.updateSize(handleSize, phSize - handleSize);
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
    this->_updateShape(
        ph.padded({
            - (pixelSize.x() * PADDING_PX),
            - (pixelSize.y() * PADDING_PX)
        })
    );

    // placeholder
    _verticesPh[0].position = shape().bottomLeft();
    _verticesPh[1].position = shape().topLeft();
    _verticesPh[2].position = shape().topRight();
    _verticesPh[3].position = shape().bottomRight();
    _bufferPh.setSubData(0, _verticesPh);

    // update geometry
    _updateGeometry();
}

void UnderStory::Widget::Scroller::_updateGeometry() {
    // update geometry and pixel geometry
    Hoverable::_updateGeometry(_matrix);
    _geomPhPx = _shapeIntoPixel(geometry());
}

const Magnum::Matrix3* UnderStory::Widget::Scroller::_matrixUpdateRequested(const Magnum::Matrix3* parentMatrix) {
    _matrix = *parentMatrix;
    _updateGeometry();
    return parentMatrix;
}

void UnderStory::Widget::Scroller::_setup() {
    // define indices
    Magnum::GL::Buffer phIndices;
    phIndices.setData({
        0, 1, 2,
        2, 3, 0
    });

    // bind buffer
    _bufferPh.setData(_verticesPh,       Magnum::GL::BufferUsage::DynamicDraw);

    // define meshes
    _meshPh.setCount(phIndices.size())
            .setIndexBuffer (std::move(phIndices),          0, Magnum::MeshIndexType::UnsignedInt)
            .addVertexBuffer(_bufferPh,                     0, Shader::Rounded::Position{});
}
