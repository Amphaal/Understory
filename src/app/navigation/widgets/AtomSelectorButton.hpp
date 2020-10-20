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

#include <Magnum/Platform/Sdl2Application.h>

#include <Magnum/Shaders/Flat.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Timeline.h>

#include <Magnum/Animation/Track.h>
#include <Magnum/Animation/Easing.h>
#include <Magnum/Animation/Player.h>

#include <utility>

#include "base/WidgetHelper.hpp"
#include "src/app/navigation/base/BaseUIPlayerHelper.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class AtomSelectorButton : public Navigation::BaseUIPlayerHelper<> {
 public:
    AtomSelectorButton(Magnum::Timeline* timeline, Magnum::Shaders::Flat2D* shader) :
        BaseUIPlayerHelper(timeline, &_moveAnim, .2f, &_defaultAnimationCallback),
        _shader(shader) {
        _setup();
    }

    void onViewportChange(const Helper &wh) {
        auto asXPadding = 15.f;
        auto targetPrcXSize = .025f;

        // update responsive matrix
        auto asSize = wh.ws.x() * (targetPrcXSize / 2);
        _responsiveMatrix = wh.baseProjMatrix *
            Magnum::Matrix3::translation(
                wh.ws * (Magnum::Vector2{-.5f} + Magnum::Vector2{wh.pixelSize.x() * (asSize + asXPadding), 0.f})
            ) *
            Magnum::Matrix3::scaling(Magnum::Vector2{asSize});

        // update pos
        _updatePos();
    }

    bool onMouseMove(const Magnum::Vector2 &cursorPos, Magnum::Platform::Application* app) {
        // prevent updating if state did not change
        auto hovered = _pos.contains(cursorPos);
        if(_isHovered == hovered) return _isHovered;

        // update state
        _isHovered = hovered;

        // apply changes
        if(hovered) {
            _color = {0xFFFFFF_rgbf};
            app->setCursor(Magnum::Platform::Sdl2Application::Cursor::Hand);
        } else {
            _color = {0x000000_rgbf};
            app->setCursor(Magnum::Platform::Sdl2Application::Cursor::Arrow);
        }

        //
        _updateAnimationYTarget();

        //
        return _isHovered;
    }

    void onMouseClick() {
        if(!_isHovered) return;
        _isOpen = !_isOpen;
        _updateAnimationYTarget();
    }

    void draw() {
        _shader->setTransformationProjectionMatrix(_matrix)
            .setColor(_color)
            .draw(_mesh);
    }

 private:
    bool _isHovered = false;
    bool _isOpen = false;

    Magnum::Matrix3 _responsiveMatrix;
    Magnum::Matrix3 _moveAnim;
    Magnum::Matrix3 _matrix;

    Magnum::Range2D _pos;
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Color4 _color{0x000000_rgbf};
    Magnum::Shaders::Flat2D* _shader = nullptr;

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Navigation::AnimationState<Magnum::Vector2>& state) {
        //
        state.current = Magnum::Math::lerp(
            state.from,
            state.to,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    void _onAnimationProgress() final {
        _replaceMainMatrix(Magnum::Matrix3::translation(currentAnim()));
        _updatePos();
    }

    void _updateAnimationYTarget() {
        //
        Magnum::Vector2 target;
        if(_isOpen) {
            target = Magnum::Vector2::yAxis(2.f);
        } else if(_isHovered) {
            target = Magnum::Vector2::yAxis(.5f);
        }

        //
        _updateAnimationAndPlay(_moveAnim.translation(), target);
    }

    void _updatePos() {
        _matrix = _responsiveMatrix * _moveAnim;

        _pos = Magnum::Range2D {
            _matrix.transformPoint({-1.f, -1.f}),
            _matrix.transformPoint({1.f, 1.f})
        };
    }

    void _setup() {
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
};

}  // namespace Widget

}  // namespace UnderStory
