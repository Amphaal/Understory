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

#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Mesh.h>

#include <utility>

#include "WidgetHelper.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class AtomSelectorButton {
 public:
    explicit AtomSelectorButton(Magnum::NoCreateT) {}
    explicit AtomSelectorButton(Magnum::Shaders::Flat2D * shader) : _shader(shader) {
        // define indices
        Magnum::GL::Buffer bIndices, bVertexes;
        bIndices.setData({
            0, 1, 2,
            2, 3, 0
        });

        // define vertices
        struct Vertex {
            Magnum::Vector2 position;
        };
        const Vertex vertexes[]{
            {{-1.f, -1.f}},
            {{ 1.f, -1.f}},
            {{ 1.f,  1.f}},
            {{-1.f,  1.f}}
        };

        // bind buffer
        bVertexes.setData(vertexes, Magnum::GL::BufferUsage::StaticDraw);

        // define mesh
        _mesh.setCount(bIndices.size())
                .setIndexBuffer (std::move(bIndices),  0, Magnum::MeshIndexType::UnsignedInt)
                .addVertexBuffer(std::move(bVertexes), 0, Magnum::Shaders::Flat2D::Position{});
    }

    void onViewportChange(const Helper &wh) {
        auto asSize = wh.ws.x() * (.05f / 2);
        auto asXPadding = 15.f;

        _matrix = wh.baseProjMatrix *
            Magnum::Matrix3::translation(wh.ws * (Magnum::Vector2{-.5f} + Magnum::Vector2{wh.pixelSize.x() * (asSize + asXPadding), 0.f})) *
            Magnum::Matrix3::scaling(Magnum::Vector2{asSize});
    }

    void onMouseMove(const Magnum::Vector2 &cursorPos) {
        Magnum::Range2D d {
            _matrix.transformPoint({-1.f, -1.f}),
            _matrix.transformPoint({1.f, 1.f})
        };
    }

    void draw() {
        _shader->setTransformationProjectionMatrix(_matrix)
            .setColor(0xFFFFFF_rgbf)
            .draw(_mesh);
    }

 private:
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Matrix3 _matrix;
    Magnum::Shaders::Flat2D* _shader = nullptr;
};

}  // namespace Widget

}  // namespace UnderStory
