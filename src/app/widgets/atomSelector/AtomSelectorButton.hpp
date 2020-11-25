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

#include "src/app/widgets/animation/PlayerMatrixAnimator.hpp"

#include "src/app/widgets/base/Constraints.hpp"
#include "src/app/widgets/base/Hoverable.hpp"
#include "src/app/widgets/base/Toggleable.hpp"

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class AtomSelectorButton : 
    public Animation::PlayerMatrixAnimator<>,
    public Hoverable,
    public Button {
 public:
    AtomSelectorButton() : PlayerMatrixAnimator(&_moveAnim, .1f, &_defaultAnimationCallback) {
        _updateCurrentColor();
    }

    void onViewportChange(Magnum::Range2D& shapeAllowedSpace) final {
        Hoverable::onViewportChange(shapeAllowedSpace);

        // update responsive matrix
            auto& projMatrix = constraints().projMatrix();
            auto& pixelSize = constraints().pixelSize();
            auto pixelLeftPadding = 15.f;

        _responsiveMatrix =
            Magnum::Matrix3::translation(Magnum::Vector2{-1.f, -1.f}) *  // anchor to BottomLeft
            Magnum::Matrix3::translation(Magnum::Vector2::xAxis(pixelSize.x() * 2.f * pixelLeftPadding)) *  // add pixel relative padding
            Magnum::Matrix3::translation(-_halfButtonSizeHeightAxis()) *  // duck half button size beneath window
            projMatrix;

        //
        _updateGeometry();
    }

    void draw() {
        Shaders::flat
            ->setTransformationProjectionMatrix(_matrix)
            .setColor(_currentColor)
            .draw(_mesh);
    }

 private:
    Magnum::Matrix3 _responsiveMatrix;
    Magnum::Matrix3 _moveAnim;
    Magnum::Matrix3 _matrix;

    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};

    Magnum::Color4 _currentColor;
    void _updateCurrentColor() {
        _currentColor = isHovered() || isToggled() ? 0xFFFFFF_rgbf : 0x000000_rgbf;
    }

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Animation::State<Magnum::Vector2>& state) {
        //
        state.current = Magnum::Math::lerp(
            state.from,
            state.to,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    void _onHoverChanged(bool isHovered) final {
        // apply changes
        if(isHovered) {
            app()->setCursor(Magnum::Platform::Sdl2Application::Cursor::Hand);
        } else {
            app()->setCursor(Magnum::Platform::Sdl2Application::Cursor::Arrow);
        }

        //
        _updateCurrentColor();

        //
        _updateAnimationYTarget();
    }

    void _onToggled(bool isToggled) final {
        Button::_onToggled(isToggled);
        
        //
        _updateCurrentColor();

        //
        _updateAnimationYTarget();
    }

    void _onAnimationProgress() final {
        // update matrix
        _updateAnimatedMatrix(
            Magnum::Matrix3::translation(
                currentAnim()
            )
        );

        // update geom
        _updateGeometry();
    }

    void _updateAnimationYTarget() {
        //
        Magnum::Vector2 target;
        if(isToggled()) {
            target = _halfButtonSizeHeightAxis() * 2.f;
        } else if(isHovered()) {
            target = _halfButtonSizeHeightAxis() * .5f;
        }

        //
        _updateAnimationAndPlay(_moveAnim.translation(), target);
    }

    const Magnum::Vector2 _halfButtonSizeHeightAxis() const {
        return Magnum::Vector2::yAxis(shape().size().y() * .5f);
    }

    void _updateGeometry() {
        // update matrix
        _matrix = _responsiveMatrix * _moveAnim;

        // update geom
        Hoverable::_updateGeometry(_matrix);
    }

    bool _setupDone = false;
    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final {
        //
        if(_setupDone) return;

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
            {{  0.f, 0.f }},
            {{ .1f,  0.f }},
            {{ .1f,  .1f }},
            {{  0.f, .1f }}
        };

        // bind buffer
        bVertices.setData(vertices, Magnum::GL::BufferUsage::StaticDraw);

        // define mesh
        _mesh.setCount(bIndices.size())
                .setIndexBuffer (std::move(bIndices),  0, Magnum::MeshIndexType::UnsignedInt)
                .addVertexBuffer(std::move(bVertices), 0, Magnum::Shaders::Flat2D::Position{});

        //
        _updateShape({vertices[0].position, vertices[2].position});
        _updateGeometry();

        //
        _setupDone = true;
    }
};

}  // namespace Widget

}  // namespace UnderStory
