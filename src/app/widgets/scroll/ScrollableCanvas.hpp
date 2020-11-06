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

#include "ScrollableContent.hpp"

namespace UnderStory {

namespace Widget {

class ScrollableCanvas : public Hoverable {
 public:
    ScrollableCanvas(const Magnum::Matrix3* panelMatrix, const ScrollableContent* content) : _panelMatrix(panelMatrix), _content(content) {
        _setup();
    };
    
    void draw() {
        
        content->draw();

        Shaders::color
            ->setTransformationProjectionMatrix(*_panelMatrix)
            .draw(_mesh);
    }

 private:
    const Magnum::Matrix3* _panelMatrix;
    const ScrollableContent* _content;

    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::GL::Buffer _buffer;

    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Color4 color;
    };
    Corrade::Containers::StaticArray<4, Vertex> _vertices;

    void _geometryUpdateRequested() final {
        // update buffer
        _buffer.setSubData(0, _vertices);

        // update geometry
        _updateGeometry(Magnum::Range2D {
            _panelMatrix->transformPoint(shape().min()),
            _panelMatrix->transformPoint(shape().max())
        });
    }

    void _updateShapeFromConstraints(const Constraints &wh, Magnum::Range2D& shapeAllowedSpace) final {
        // take all remaining space
        _updateShape(shapeAllowedSpace);
        shapeAllowedSpace = {};

        // update vertices
        _vertices[0] = {shape().bottomLeft(),     0xFF000099_rgbaf};
        _vertices[1] = {shape().bottomRight(),    0xFF000099_rgbaf};
        _vertices[2] = {shape().topRight(),       0x00FF0099_rgbaf};
        _vertices[3] = {shape().topLeft(),        0x00FF0099_rgbaf};

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
