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

#include "base/Constraints.hpp"
#include "src/app/navigation/base/BaseUIPlayerHelper.hpp"
#include "base/Hoverable.hpp"
#include "base/Toggleable.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class Panel : public Navigation::BaseUIPlayerHelper<Magnum::Float>, public Hoverable<>, public Toggleable {
 public:
    Panel(Magnum::Timeline* timeline, Magnum::Shaders::Flat2D* shader, Magnum::Platform::Application* app) :
        BaseUIPlayerHelper(timeline, &_moveAnim, .2f, &_defaultAnimationCallback),
        Hoverable(app),
        _shader(shader) {
        _definePanelPosition(-X_PANEL_SIZE);
        _setup();
    }

    void mayDraw() {
        //
        if(!isToggled() && !isAnimationPlaying()) return;

        //
        _shader->setTransformationProjectionMatrix(_moveAnim)
            .setColor(0x000000AA_rgbaf)
            .draw(_mesh);
    }

 private:
    static constexpr float X_PANEL_SIZE = .8f;
    static constexpr float Y_PANEL_SIZE = 1.9f;
    static constexpr Magnum::Vector2 BL_START {-1.f};
    static constexpr Magnum::Vector2 BL_END {BL_START.x() + X_PANEL_SIZE, BL_START.y() + Y_PANEL_SIZE};

    Magnum::Matrix3 _moveAnim;

    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Shaders::Flat2D* _shader = nullptr;

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Navigation::AnimationState<Magnum::Float>& state) {
        //
        state.current = Magnum::Math::lerp(
            state.from,
            state.to,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    void _onAnimationProgress() final {
        _definePanelPosition(currentAnim());
        _updateGeometry();
    }

    void _onToggled(bool isToggled) final {
        if(isToggled)
            _updateAnimationAndPlay(-X_PANEL_SIZE, 0.f);
        else
            _updateAnimationAndPlay(0.f, -X_PANEL_SIZE);
    }

    // no geometry to update
    void _updateGeometry() final {
        _geometry = Magnum::Range2D {
            _moveAnim.transformPoint(BL_START),
            _moveAnim.transformPoint(BL_END)
        };
    }

    void _definePanelPosition(const Magnum::Float &xPos) {
        _replaceMainMatrix(
            Magnum::Matrix3::translation(
                Magnum::Vector2::xAxis(xPos)
            )
        );
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
            {BL_START},
            {{ BL_END.x(),   BL_START.y() }},
            {BL_END},
            {{ BL_START.x(),  BL_END.y() }}
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
