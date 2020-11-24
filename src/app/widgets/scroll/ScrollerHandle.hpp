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
#include "Scrollable.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollablePanel;

class ScrollerHandle : public Hoverable, public Scrollable {
 public:
    explicit ScrollerHandle(ScrollablePanel* panel, StickTo scrollerStickyness) : Scrollable(scrollerStickyness), _panel(panel) {
        _setup();
    }

    void draw() {
        Shaders::rounded
            ->setProjectionMatrix(_matrix)
            .setRectPx(_geomScrollerPx)
            .setColor(_scrollerColor)
            .draw(_meshScroller);
    }

    void scrollByPercentage(Magnum::Float percentTranslate) {
        // TODO animate
        auto tr = -_translationGap * percentTranslate;
        _animateScrollByTr(tr);
        
        //
        _updateGeometry();
    }

    // updates handle size
    void updateSize(const Magnum::Float& handleSize, const Magnum::Float& trGap) {
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
        scrollByPercentage(0.f);

        // ... then finally update buffer
        _bufferScroller.setSubData(0, _verticesScroller);
    }

 private:
    static inline Magnum::Color4 SCRLL_COLOR_IDLE = 0xCCCCCCFF_rgbaf;
    static inline Magnum::Color4 SCRLL_COLOR_ACTIVE = 0xFFFFFFFF_rgbaf;

    ScrollablePanel* _panel;

    Magnum::Matrix3 _matrix;
    const Magnum::Matrix3* _parentMatrix = nullptr;
    Magnum::Float _translationGap = .0f;

    struct Vertex {
        Magnum::Vector2 position;
    };

    Magnum::GL::Buffer _bufferScroller;
    Corrade::Containers::StaticArray<4, Vertex> _verticesScroller;
    Magnum::GL::Mesh _meshScroller{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Range2D _geomScrollerPx;

    // if mouse is over placeholder
    void _onHoverChanged(bool isHovered) final {
        _updateScrollColor();
    }

    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final {
        // just set the shape as ph shape for now
        _updateShape(availableSpace);
    }

    void _updateGeometry() {
        // update geometry and pixel geometry
        if(_parentMatrix) _matrix = *_parentMatrix * _scrollMatrix();
        Hoverable::_updateGeometry(_matrix);
        _geomScrollerPx = _shapeIntoPixel(geometry());
    }

    Magnum::Color4 _scrollerColor;
    void _updateScrollColor() {
        _scrollerColor = isHovered() ? SCRLL_COLOR_ACTIVE : SCRLL_COLOR_IDLE;
    }

    const Magnum::Matrix3* _matrixUpdateRequested(const Magnum::Matrix3* parentMatrix) final {
        _parentMatrix = parentMatrix;
        _updateGeometry();
        return parentMatrix;
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
