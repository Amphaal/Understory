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
#include <Magnum/GL/Sampler.h>
#include <Magnum/GL/Texture.h>

#include <Magnum/Math/Math.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>

#include <Magnum/Shaders/Flat.h>

#include <Magnum/Mesh.h>

#include <utility>

namespace UnderStory {

namespace Widget {

namespace Helper {

class Enlighter {
 public:
    explicit Enlighter(Magnum::Shaders::Flat2D *shader) : _shader(shader) {
        // define indices
        Magnum::GL::Buffer indices, vertices;
        indices.setData({
            0, 1, 2,
            0, 2, 3
        });

        // define vertices
        struct EnlighterVertex {
            Magnum::Vector2 position;
        };
        const EnlighterVertex vertex[]{
            {{-1.f, -1.f}},
            {{ 1.f, -1.f}},
            {{ 1.f,  1.f}},
            {{-1.f,  1.f}}
        };

        // bind buffer
        vertices.setData(vertex, Magnum::GL::BufferUsage::StaticDraw);

        // define mesh
        _mesh.setCount(indices.size())
                .setIndexBuffer (std::move(indices),        0, Magnum::MeshIndexType::UnsignedInt)
                .addVertexBuffer(std::move(vertices),       0, Magnum::Shaders::Flat2D::Position{});
    }

    void draw(const Magnum::Color4 &color) {
        _shader
            ->setTransformationProjectionMatrix({})
            .setColor(color)
            .draw(_mesh);
    }

 private:
    Magnum::Shaders::Flat2D* _shader;
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::TriangleFan};
};

}  // namespace Helper

}  // namespace Widget

}  // namespace UnderStory
