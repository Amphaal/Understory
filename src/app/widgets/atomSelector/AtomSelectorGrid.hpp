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

#include <Magnum/Magnum.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Color.h>

#include <Corrade/Containers/StaticArray.h>

#include <stdexcept>
#include <utility>

#include "src/app/shaders/Shaders.hpp"
#include "src/app/widgets/base/Hoverable.hpp"
#include "src/app/widgets/scroll/Scissorable.h"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class AtomSelectorGrid : public Hoverable, public Scissorable {
 public:
    explicit AtomSelectorGrid(ScrollablePanel* associatedPanel) : Scissorable(associatedPanel) {
        _bindToPanel();
        _setup();
    }

 private:
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::GL::Buffer _buffer;

    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Color4 color;
    };
    Corrade::Containers::StaticArray<4, Vertex> _vertices;

    void _drawInbetweenScissor() final {
        Shaders::color
            ->setTransformationProjectionMatrix(_panelMatrix() * scrollMatrix())
            .draw(_mesh);
    }

    void _updateGeometry() {
        // update buffer
        _buffer.setSubData(0, _vertices);

        // update geometry
        Hoverable::_updateGeometry(_panelMatrix());

        // update scissor
        _updateScissorTarget(geometry());
    }

    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final {
        // take all remaining space
        switch(_growableAxis()) {
            case GrowableAxis::Width:
                availableSpace.max().x() *= 2.f;
            break;
            case GrowableAxis::Height:
                availableSpace.min().y() *= 2.f;
            break;
        }
        _updateShape(availableSpace);

        // update vertices
        _vertices[0] = {shape().bottomLeft(),     0xFF000099_rgbaf};
        _vertices[1] = {shape().bottomRight(),    0xFF000099_rgbaf};
        _vertices[2] = {shape().topRight(),       0x00FF0099_rgbaf};
        _vertices[3] = {shape().topLeft(),        0x00FF0099_rgbaf};

        //
        _updateGeometry();

        //
        Magnum::Float size;
        switch(_growableAxis()) {
            case GrowableAxis::Width:
                size = availableSpace.x().size();
            break;
            case GrowableAxis::Height:
                size = availableSpace.y().size();
            break;
        }

        //
        _signalContentSizeChanged(size);
        availableSpace = {};
    }

    void _setup() {
        // define indices
        Magnum::GL::Buffer bIndices;
        bIndices.setData({
            0, 1, 2,
            2, 3, 0
        });

        // bind buffer
        _buffer.setData(_vertices, Magnum::GL::BufferUsage::DynamicDraw);

        // define panel mesh
        _mesh.setCount(bIndices.size())
                .setIndexBuffer (std::move(bIndices),   0, Magnum::MeshIndexType::UnsignedInt)
                .addVertexBuffer(_buffer,               0, Magnum::Shaders::Flat2D::Position{}, Magnum::Shaders::Flat2D::Color4{});
    }
};

}  // namespace Widget

}  // namespace UnderStory
