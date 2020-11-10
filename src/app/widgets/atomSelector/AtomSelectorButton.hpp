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

class AtomSelectorButton : public Animation::PlayerMatrixAnimator<>, public Hoverable, public Toggleable {
 public:
    AtomSelectorButton() : PlayerMatrixAnimator(&_moveAnim, .1f, &_defaultAnimationCallback) {}

    void onViewportChange(Magnum::Range2D& shapeAllowedSpace) final {
        Hoverable::onViewportChange(shapeAllowedSpace);

        //
        auto asXPadding = 15.f;
        auto targetPrcXSize = .025f;

        // update responsive matrix
            auto& ws = constraints().ws();
            auto& baseProjMatrix = constraints().baseProjMatrix();
            auto& pixelSize = constraints().pixelSize();
            auto asSize = ws.x() * (targetPrcXSize / 2);
        _responsiveMatrix = baseProjMatrix *
            Magnum::Matrix3::translation(
                ws * (Magnum::Vector2{-.5f} + Magnum::Vector2{pixelSize.x() * (asSize + asXPadding), 0.f})
            ) *
            Magnum::Matrix3::scaling(Magnum::Vector2{asSize});

        //
        _updateGeometry();
    }

    void draw() {
        Shaders::flat
            ->setTransformationProjectionMatrix(_matrix)
            .setColor(isHovered() || isToggled() ? 0xFFFFFF_rgbf : 0x000000_rgbf)
            .draw(_mesh);
    }

 private:
    Magnum::Matrix3 _responsiveMatrix;
    Magnum::Matrix3 _moveAnim;
    Magnum::Matrix3 _matrix;

    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};

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
        _updateAnimationYTarget();
    }

    void _onToggled(bool isToggled) final {
        //
        _updateAnimationYTarget();
    }

    void _onAnimationProgress() final {
        // update matrix
        _replaceMainMatrix(
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
            target = Magnum::Vector2::yAxis(2.f);
        } else if(isHovered()) {
            target = Magnum::Vector2::yAxis(.5f);
        }

        //
        _updateAnimationAndPlay(_moveAnim.translation(), target);
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
            {{-1.f, -1.f}},
            {{ 1.f, -1.f}},
            {{ 1.f,  1.f}},
            {{-1.f,  1.f}}
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
