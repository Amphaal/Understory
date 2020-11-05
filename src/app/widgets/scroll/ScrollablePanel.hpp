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

#include "../animation/PlayerMatrixAnimator.hpp"

#include "../base/Constraints.hpp"
#include "../base/Container.hpp"
#include "../base/Toggleable.hpp"

#include "Scroller.hpp"
#include "ScrollableContent.hpp"

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollablePanel : public Animation::PlayerMatrixAnimator<Magnum::Vector2>, public Container<>, public Morphable<>, public Toggleable {
 public:
    ScrollablePanel(const Shape<>* parent, StickTo stickness = StickTo::Left, float thickness = .6f) :
        PlayerMatrixAnimator(&_matrix, .2f, &_defaultAnimationCallback),
        Morphable(parent),
        _stickness(stickness),
        _thickness(thickness),
        _scroller(this, &_matrix, &_content, _scrollerStickyness()) 
        {
        // set collapsed state as default
        _definePanelPosition(_collapsedTransform());

        //
        bind({&_scroller, &_content});

        // 
        _setup();
    }

    void mayDraw() {
        //
        if(!isToggled() && !isAnimationPlaying()) return;

        // draw background
        Shaders::flat
            ->setTransformationProjectionMatrix(_matrix)
            .setColor(0x000000AA_rgbaf)
            .draw(_mesh);

        // draw scroller
        _scroller.draw();
    }

    void onMouseScroll(const Magnum::Vector2& scrollOffset) {
        // TODO
        _scroller.onMouseScroll(scrollOffset);
    }

    void _onHoverChanged(bool isHovered) final {
        if(isHovered) {
            _scroller.reveal();
        } else {
            _scroller.fade();
        }
    }

 private:
    StickTo _stickness;
    float _thickness;

    Scroller _scroller;
    ScrollableContent _content;

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
        _updateGeometry(Magnum::Range2D {
            _matrix.transformPoint(shape().min()),
            _matrix.transformPoint(shape().max())
        });
    }

    void _definePanelPosition(const Magnum::Vector2 &pos) {
        _replaceMainMatrix(
            Magnum::Matrix3::translation(pos)
        );
    }

    // scroller position within panel
    const StickTo _scrollerStickyness() const {
        switch (_stickness) {
            case StickTo::Left :
                return StickTo::Right;
            case StickTo::Top :
                return StickTo::Bottom;
            case StickTo::Right :
                return StickTo::Right;
            case StickTo::Bottom :
                return StickTo::Bottom;
        }
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

    void _setup() {
        // as shape is immuable, set it once
        {
            Magnum::Vector2 pStart, pEnd;
            auto &masterShape = this->parent()->shape();

            // determine shape and position
            switch (_stickness) {
                case StickTo::Left :
                    pStart = masterShape.min();
                    pEnd = {pStart.x() + _thickness, masterShape.max().y()};
                    break;
                case StickTo::Top :
                    pStart = {masterShape.min().x(), masterShape.max().y() - _thickness};
                    pEnd = masterShape.max();
                    break;
                case StickTo::Right :
                    pStart = {masterShape.max().x() - _thickness, masterShape.min().y()};
                    pEnd = masterShape.max();
                    break;
                case StickTo::Bottom :
                    pStart = masterShape.min();
                    pEnd = {masterShape.max().x(), pStart.y() + _thickness};
                    break;
            }

            // update shape
            this->_updateShape({pStart, pEnd});
        }

        // define vertices
        struct Vertex {
            Magnum::Vector2 position;
        };

        const Vertex vertices[] {
            {shape().bottomLeft()},
            {shape().bottomRight()},
            {shape().topRight()},
            {shape().topLeft()}
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
