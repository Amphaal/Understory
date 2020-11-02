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

#include "animation/BaseUIPlayerHelper.hpp"

#include "base/Constraints.hpp"
#include "base/Hoverable.hpp"

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class Scroller : public Hoverable<> {
 public:
    Scroller(
        const Magnum::Matrix3* panelMatrix, 
        const Magnum::Range2D* panelBounds, 
        const Magnum::Float* contentSize, 
        StickTo stickness
    ) :
        _panelBounds(panelBounds),
        _panelMatrix(panelMatrix),
        _contentSize(contentSize),
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
    const Magnum::Range2D* _panelBounds = nullptr;
    const Magnum::Float* _contentSize = nullptr;

    Magnum::GL::Buffer _buffer;
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};

    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Color4 color;
    };
    Corrade::Containers::StaticArray<4, Vertex> _vertices;

    virtual void onViewportChange(const Constraints &wh) {
        _updateVertices(wh.pixelSize);
        Hoverable<>::onViewportChange(wh);
    }

    void _geometryUpdateRequested() final {
        // update buffer
        _buffer.setSubData(0, _vertices);

        // update geometry
        _geometry = Magnum::Range2D {
            _panelMatrix->transformPoint(_vertices[0].position),
            _panelMatrix->transformPoint(_vertices[2].position)
        };
    }

    void _onHoverChanged(bool isHovered) final {
        if(isHovered) {
            app()->setCursor(Magnum::Platform::Sdl2Application::Cursor::Hand);
        } else {
            app()->setCursor(Magnum::Platform::Sdl2Application::Cursor::Arrow);
        }
    }

    void _updateVertices(const Magnum::Vector2& pixelSize) {

        // start and begin of scroller placeholder
        auto pStart = _panelBounds->min();
        auto pEnd = _panelBounds->max();

        // define vertices from parent shape
        switch (_stickness) {
            case StickTo::Left :
                pEnd.x() = pStart.x() + (pixelSize.x() * THICKNESS_PX);
                break;
            case StickTo::Top :
                pStart.y() = pEnd.y() - (pixelSize.y() * THICKNESS_PX);
                break;
            case StickTo::Right :
                pStart.x() = pEnd.x() - (pixelSize.x() * THICKNESS_PX);
                break;
            case StickTo::Bottom :
                pEnd.y() = pStart.y() + (pixelSize.y() * THICKNESS_PX);
                break;
        }

        // define shape and pad extremities
        auto shape = Magnum::Range2D{pStart, pEnd};
        shape = shape.padded({ 0.f, - (pixelSize.y() * PADDING_PX) });

        // lateral padding
        switch (_stickness) {
            case StickTo::Left :
                shape = shape.translated({PADDING_PX * pixelSize.x(), 0.f});
                break;
            case StickTo::Top :
                shape = shape.translated({0.f, PADDING_PX * pixelSize.y()});
                break;
            case StickTo::Right :
                shape = shape.translated({-PADDING_PX * pixelSize.x(), 0.f});
                break;
            case StickTo::Bottom :
                shape = shape.translated({0.f, -PADDING_PX * pixelSize.y()});
                break;
        }

        // placeholder
        _vertices[0] = {shape.bottomLeft(),     0xFFFFFF66_rgbaf};
        _vertices[1] = {shape.bottomRight(),    0xFFFFFF66_rgbaf};
        _vertices[2] = {shape.topRight(),       0xFFFFFF66_rgbaf};
        _vertices[3] = {shape.topLeft(),        0xFFFFFF66_rgbaf};
    }

    void _setup() {
        // define indices
        Magnum::GL::Buffer bIndices;
        bIndices.setData({
            // placeholder
            0, 1, 2,
            2, 3, 0
            // // scroller
            // 4, 5, 6,
            // 6, 7, 4
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
