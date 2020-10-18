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

#include <Magnum/Animation/Track.h>
#include <Magnum/Animation/Easing.h>
#include <Magnum/Animation/Player.h>

#include <utility>

#include "WidgetHelper.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class AtomSelectorButton {
 public:
    explicit AtomSelectorButton(Magnum::NoCreateT) {}
    AtomSelectorButton(Magnum::Shaders::Flat2D* shader, Magnum::Timeline* timeline) :
    _shader(shader), _timeline(timeline) {
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

        // define animation
        _track = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>({
            {0.f, Magnum::Vector2::yAxis(0.0f)},
            {.2f, Magnum::Vector2::yAxis(1.0f)}
        }, Magnum::Math::lerp);
        _player.add(_track, _moveAnim);
    }

    void onViewportChange(const Helper &wh) {
        auto asSize = wh.ws.x() * (.05f / 2);
        auto asXPadding = 15.f;

        _matrix = wh.baseProjMatrix *
            Magnum::Matrix3::translation(wh.ws * (Magnum::Vector2{-.5f} + Magnum::Vector2{wh.pixelSize.x() * (asSize + asXPadding), 0.f})) *
            Magnum::Matrix3::scaling(Magnum::Vector2{asSize});

        _pos = Magnum::Range2D {
            _matrix.transformPoint({-1.f, -1.f}),
            _matrix.transformPoint({1.f, 1.f})
        };
    }

    void onMouseMove(const Magnum::Vector2 &cursorPos) {        
        if(!_pos.contains(cursorPos)) {
            _color = {0xFFFFFF_rgbf};
            return;
        }

        _color = {0x000000_rgbf};
    }

    void advance() {
        _player.advance(_timeline->previousFrameTime());
    }

    void draw() {
        Magnum::Debug{} << _moveAnim;
        _shader->setTransformationProjectionMatrix(_matrix * Magnum::Matrix3::translation(_moveAnim))
            .setColor(_color)
            .draw(_mesh);
    }

 private:
    Magnum::Animation::Track<Magnum::Float, Magnum::Vector2> _track;
    Magnum::Animation::Player<Magnum::Float> _player;
    Magnum::Vector2 _moveAnim;
    Magnum::Range2D _pos;
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Matrix3 _matrix;
    Magnum::Color4 _color{0xFFFFFF_rgbf};
    Magnum::Shaders::Flat2D* _shader = nullptr;
    Magnum::Timeline* _timeline = nullptr;
};

}  // namespace Widget

}  // namespace UnderStory
