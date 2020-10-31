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

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Mesh.h>

#include <utility>

#include "src/app/shaders/selectRect/SelectionRectState.hpp"

namespace UnderStory {

namespace Widget {

namespace Helper {

class SelectionRectangle : public Navigation::SelectionRectState {
 public:
    explicit SelectionRectangle(const Magnum::Utility::Resource &rs) :
        Navigation::SelectionRectState(&_selectRectShader, &_selectRectBuffer),
        _selectRectShader(rs) {
        // define indices
        Magnum::GL::Buffer indices;
        indices.setData({
            0, 1, 2,
            0, 2, 3
        });

        // bind buffer
        _selectRectBuffer.setData(vertices(), Magnum::GL::BufferUsage::DynamicDraw);

        // define mesh
        _selectRect.setCount(indices.size())
                .addVertexBuffer(_selectRectBuffer, 0, Shader::SelectionRect::Position{})
                .setIndexBuffer(std::move(indices), 0, Magnum::MeshIndexType::UnsignedInt);
    }

    void mayDraw(const Magnum::Matrix3 &projectionMatrix) {
        //
        if(!isSelecting()) return;

        // draw
        _selectRectShader
            .setProjectionMatrix(projectionMatrix)
            .draw(_selectRect);
    }

 private:
    Shader::SelectionRect _selectRectShader{Magnum::NoCreate};
    Magnum::GL::Buffer _selectRectBuffer;
    Magnum::GL::Mesh _selectRect{Magnum::GL::MeshPrimitive::Triangles};
};

}  // namespace Helper

}  // namespace Widget

}  // namespace UnderStory
