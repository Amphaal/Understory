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

#include "animation/BaseUIPlayerHelper.hpp"

#include "base/Constraints.hpp"
#include "base/Container.hpp"
#include "base/Toggleable.hpp"

#include "Scroller.hpp"

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollablePanel : public Animation::BaseUIPlayerHelper<Magnum::Vector2>, public Container<>, public Toggleable {
 public:
    ScrollablePanel(const Magnum::Range2D* bounds, StickTo stickness = StickTo::Left, float thickness = .6f) :
        BaseUIPlayerHelper(&_matrix, .2f, &_defaultAnimationCallback),
        _bounds(bounds),
        _stickness(stickness),
        _thickness(thickness),
        _scroller(&_matrix, &_geometry, stickness) 
        {
        // set collapsed state as default
        _definePanelPosition(_collapsedTransform());

        // 
        _setup();
    }

    void mayDraw() {
        //
        if(!isToggled() && !isAnimationPlaying()) return;

        //
        // _scroller.mayDraw();

        //
        Shaders::flat
            ->setTransformationProjectionMatrix(_matrix)
            .setColor(0x000000AA_rgbaf)
            .draw(_mesh);
    }

    void onMouseScroll(const Magnum::Vector2& scrollOffset) {
        // TODO
        _scroller.onMouseScroll(scrollOffset);
    }

 private:
    float _thickness;
    StickTo _stickness;
    const Magnum::Range2D* _bounds;
    Magnum::Vector2 _pStart;
    Magnum::Vector2 _pEnd;

    Scroller _scroller;

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

    void _onAnimationProgress() final {
        _definePanelPosition(currentAnim());
        _geometryUpdateRequested();
    }

    void _onToggled(bool isToggled) final {
        if(isToggled)
            _updateAnimationAndPlay(_collapsedTransform(), {});
        else
            _updateAnimationAndPlay({}, _collapsedTransform());
    }

    void _geometryUpdateRequested() final {
        _geometry = Magnum::Range2D {
            _matrix.transformPoint(_pStart),
            _matrix.transformPoint(_pEnd)
        };
    }

    const Magnum::Vector2 _collapsedTransform() const {
        switch (_stickness) {
            case StickTo::Left :
                return {-_thickness, .0f};
            case StickTo::Top :
                return {.0f, _thickness};
            case StickTo::Right :
                return {_thickness, .0f};
            case StickTo::Bottom :
                return {.0f, -_thickness};
        }
    }

    void _definePanelPosition(const Magnum::Vector2 &pos) {
        _replaceMainMatrix(
            Magnum::Matrix3::translation(pos)
        );
    }

    void _setup() {
        // define vertices
        struct Vertex {
            Magnum::Vector2 position;
        };

        switch (_stickness) {
            case StickTo::Left :
                _pStart = _bounds->min();
                _pEnd = {_pStart.x() + _thickness, _bounds->max().y()};
                break;
            case StickTo::Top :
                _pStart = {_bounds->min().x(), _bounds->max().y() - _thickness};
                _pEnd = _bounds->max();
                break;
            case StickTo::Right :
                _pStart = {_bounds->max().x() - _thickness, _bounds->min().y()};
                _pEnd = _bounds->max();
                break;
            case StickTo::Bottom :
                _pStart = _bounds->min();
                _pEnd = {_bounds->max().x(), _pStart.y() + _thickness};
                break;
        }

        const Vertex vertices[4]{
            {_pStart},
            {{ _pEnd.x(),   _pStart.y() }},
            {_pEnd},
            {{ _pStart.x(),  _pEnd.y() }}
        };

        // define indices
        Magnum::GL::Buffer bIndices, bVertices;
        bIndices.setData({
            0, 1, 2,
            2, 3, 0
        });

        // bind buffer
        bVertices.setData(vertices, Magnum::GL::BufferUsage::StaticDraw);

        // define panel mesh
        _mesh.setCount(bIndices.size())
                .setIndexBuffer (std::move(bIndices),  0, Magnum::MeshIndexType::UnsignedInt)
                .addVertexBuffer(std::move(bVertices), 0, Magnum::Shaders::Flat2D::Position{});
    }
};

}  // namespace Widget

}  // namespace UnderStory
