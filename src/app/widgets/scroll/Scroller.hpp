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

#include "../animation/PlayerMatrixAnimator.hpp"

#include "../base/Constraints.hpp"
#include "../base/Hoverable.hpp"

#include "src/app/shaders/Shaders.hpp"

#include "ScrollableContent.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class Scroller : public Hoverable<>, public Morphable<> {
 public:
    Scroller(
        const Shape<>* parent,
        const Magnum::Matrix3* panelMatrix, 
        const ScrollableContent* content, 
        StickTo stickness
    ) : Morphable(parent),
        _panelMatrix(panelMatrix),
        _content(content),
        _stickness(stickness) {
        //
        _setup();
    }

    void draw() {
        //
        Shaders::color
            ->setTransformationProjectionMatrix(*_panelMatrix)
            .draw(_mesh);
    }

    void onMouseScroll(const Magnum::Vector2& scrollOffset) {
        // TODO
    }

    void onContentSizeChange() {
        // TODO
    }

    void reveal() {
        // TODO
    }

    void fade() {
        // TODO
    }

 private:
    StickTo _stickness;
    static constexpr float THICKNESS_PX = 20.f;
    static constexpr float PADDING_PX = 10.f;

    const Magnum::Matrix3* _panelMatrix = nullptr;
    const ScrollableContent* _content = nullptr;

    Magnum::GL::Buffer _buffer;
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};

    bool _isScrollerHovered = false;
    Magnum::Range2D _scrollerPos;

    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Color4 color;
    };
    Corrade::Containers::StaticArray<8, Vertex> _vertices;

    void _geometryUpdateRequested() final {
        // update buffer
        _buffer.setSubData(0, _vertices);

        // update geometry
        _updateGeometry(Magnum::Range2D {
            _panelMatrix->transformPoint(_vertices[0].position),
            _panelMatrix->transformPoint(_vertices[2].position)
        });
    }

    // if mouse is over placeholder
    void _mouseIsOver(const Magnum::Vector2 &cursorPos) final {
        // check if over scroller
        auto isScrollerHovered = _scrollerPos.contains(cursorPos);
        if(isScrollerHovered == _isScrollerHovered) return;

        // change state
        _isScrollerHovered = isScrollerHovered;
        _updateScrollerState();

        // update buffer
        _buffer.setSubData(0, _vertices);
    }

    void _shapeUpdateRequested(const Constraints &wh) final {
        // start and begin of scroller placeholder
        auto &pixelSize = wh.pixelSize;
        auto phStart = parent()->shape().min();
        auto phEnd = parent()->shape().max();

        // define vertices from parent phShape
        switch (_stickness) {
            case StickTo::Left :
                phEnd.x() = phStart.x() + (pixelSize.x() * THICKNESS_PX);
                break;
            case StickTo::Top :
                phStart.y() = phEnd.y() - (pixelSize.y() * THICKNESS_PX);
                break;
            case StickTo::Right :
                phStart.x() = phEnd.x() - (pixelSize.x() * THICKNESS_PX);
                break;
            case StickTo::Bottom :
                phEnd.y() = phStart.y() + (pixelSize.y() * THICKNESS_PX);
                break;
        }

        // define phShape and pad extremities
        auto phShape = Magnum::Range2D{phStart, phEnd};
        phShape = phShape.padded({ 0.f, - (pixelSize.y() * PADDING_PX) });

        // lateral padding
        switch (_stickness) {
            case StickTo::Left :
                phShape = phShape.translated({PADDING_PX * pixelSize.x(), 0.f});
                break;
            case StickTo::Top :
                phShape = phShape.translated({0.f, PADDING_PX * pixelSize.y()});
                break;
            case StickTo::Right :
                phShape = phShape.translated({-PADDING_PX * pixelSize.x(), 0.f});
                break;
            case StickTo::Bottom :
                phShape = phShape.translated({0.f, -PADDING_PX * pixelSize.y()});
                break;
        }

        // placeholder
        _vertices[0] = {phShape.bottomLeft(),     0xFFFFFF66_rgbaf};
        _vertices[1] = {phShape.bottomRight(),    0xFFFFFF66_rgbaf};
        _vertices[2] = {phShape.topRight(),       0xFFFFFF66_rgbaf};
        _vertices[3] = {phShape.topLeft(),        0xFFFFFF66_rgbaf};

        // scroller
        _vertices[4].position = phShape.bottomLeft();
        _vertices[5].position = phShape.bottomRight();
        _vertices[6].position = phShape.topRight();
        _vertices[7].position = phShape.topLeft();
        _scrollerPos = { _vertices[4].position, _vertices[6].position };
        _updateScrollerState();
    }

    void _updateScrollerState() {
        Magnum::Color4 color = _isScrollerHovered ? 0xFFFFFFFF_rgbaf : 0xCCCCCCFF_rgbaf;
        _vertices[4].color = color;
        _vertices[5].color = color;
        _vertices[6].color = color;
        _vertices[7].color = color;
    }

    void _setup() {
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
};

}  // namespace Widget

}  // namespace UnderStory
