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

#include <Corrade/Containers/StaticArray.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Timeline.h>

#include <utility>

#include "src/app/widgets/animation/PlayerMatrixAnimator.hpp"

#include "src/app/widgets/base/Constraints.hpp"
#include "src/app/widgets/base/Hoverable.hpp"

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollerHandle : public Hoverable {
 public:
    explicit ScrollerHandle(StickTo stickness, const Magnum::Matrix3* parentMatrix) : _stickness(stickness), _parentMatrix(parentMatrix) {
        _setup();
    }

    void draw() {
        Shaders::rounded
            ->setProjectionMatrix(*_parentMatrix * _contentScrollMatrix)
            .setRectPx(_geomScrollerPx)
            .setColor(_scrollerColor)
            .draw(_meshScroller);
    }

    void updateGeometry() {
        // update geometry and pixel geometry
        Hoverable::_updateGeometry(*_parentMatrix * _contentScrollMatrix);
        _geomScrollerPx = _shapeIntoPixel(geometry());
    }

    void onMouseScroll(const Magnum::Matrix3& scrollMatrix) {
        // TODO animate
        _contentScrollMatrix = scrollMatrix;
        
        //
        updateGeometry();
    }

    void updateSize(const Magnum::Float& handleSize) {
        // determine new scroller size
        auto scrollerShape = shape();
        switch (_stickness) {
            case StickTo::Left :
            case StickTo::Right :
                scrollerShape.min().y() = scrollerShape.topLeft().y() - handleSize;
                break;
            case StickTo::Top :
            case StickTo::Bottom :
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
        updateGeometry();

        // ... and state...
        _updateScrollColor();

        // ... then finally update buffer
        _bufferScroller.setSubData(0, _verticesScroller);
    }

 private:
    static inline Magnum::Color4 SCRLL_COLOR_IDLE = 0xCCCCCCFF_rgbaf;
    static inline Magnum::Color4 SCRLL_COLOR_ACTIVE = 0xFFFFFFFF_rgbaf;

    Magnum::Matrix3 _contentScrollMatrix;
    const Magnum::Matrix3* _parentMatrix;

    StickTo _stickness;

    struct Vertex {
        Magnum::Vector2 position;
    };

    Magnum::GL::Buffer _bufferScroller;
    Corrade::Containers::StaticArray<4, Vertex> _verticesScroller;
    Magnum::GL::Mesh _meshScroller{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Range2D _geomScrollerPx;

    Magnum::Color4 _scrollerColor;

    // if mouse is over placeholder
    void _onHoverChanged(bool isHovered) final {
        _updateScrollColor();
    }

    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final {
        // just set the shape as ph shape for now
        _updateShape(availableSpace);
    }

    void _updateScrollColor() {
        _scrollerColor = isHovered() ? SCRLL_COLOR_ACTIVE : SCRLL_COLOR_IDLE;
    }

    void _setup() {
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
};

}  // namespace Widget

}  // namespace UnderStory
