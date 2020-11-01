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

    void mayDraw() {
        //
        Shaders::flat
            ->setTransformationProjectionMatrix(*_panelMatrix)
            .setColor(0xFF0000_rgbf)
            .draw(_mesh);
    }

    void onMouseScroll(const Magnum::Vector2& scrollOffset) {
        // TODO
    }

    void onContentSizeChange() {

    }

    void reveal() {

    }

    void fade() {

    }

 private:
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    const Magnum::Matrix3* _panelMatrix = nullptr;
    const Magnum::Range2D* _panelBounds = nullptr;
    const Magnum::Float* _contentSize = nullptr;
    Magnum::Range2D _bounds;
    StickTo _stickness;

    void _geometryUpdateRequested() final {
        _geometry = Magnum::Range2D {
            _panelMatrix->transformPoint(_bounds.min()),
            _panelMatrix->transformPoint(_bounds.max())
        };
    }

    void _onHoverChanged(bool isHovered) final {
        if(isHovered) {
            app()->setCursor(Magnum::Platform::Sdl2Application::Cursor::Hand);
        } else {
            app()->setCursor(Magnum::Platform::Sdl2Application::Cursor::Arrow);
        }
    }

    void _setup() {
        // define indices
        Magnum::GL::Buffer bIndices, bVertices;
        bIndices.setData({
            0, 1, 2,
            2, 3, 0
        });

        // define vertices
        struct Vertex {
            Magnum::Vector2 position;
        };
        const Vertex vertices[]{
            {{-1.f, -1.f}},
            {{ 1.f, -1.f}},
            {{ 1.f,  1.f}},
            {{-1.f,  1.f}}
        };

        // bind buffer
        bVertices.setData(vertices, Magnum::GL::BufferUsage::StaticDraw);

        // define panel mesh
        _mesh.setCount(bIndices.size())
                .setIndexBuffer (std::move(bIndices),  0, Magnum::MeshIndexType::UnsignedInt)
                .addVertexBuffer(std::move(bVertices), 0, Magnum::Shaders::Flat2D::Position{});
    }
};

}  // namespace Widget

}  // namespace UnderStory
