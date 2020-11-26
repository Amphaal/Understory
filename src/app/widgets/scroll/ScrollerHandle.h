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

#include <Corrade/Containers/StaticArray.h>

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
#include "Scrollable.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollablePanel;

class ScrollerHandle : 
    public Hoverable, 
    public Scrollable, 
    public Button, 
    public MouseMove_EH {
 public:
    explicit ScrollerHandle(ScrollablePanel* panel, StickTo scrollerStickyness);

    void draw();

    void animateByPercentage(Magnum::Float percentTranslate);
    void inplaceByPercentage(Magnum::Float percentTranslate);

    // updates handle size
    void updateSize(const Magnum::Float& handleSize, const Magnum::Float& trGap);

 private:
    static inline Magnum::Color4 SCRLL_COLOR_IDLE = 0xCCCCCCFF_rgbaf;
    static inline Magnum::Color4 SCRLL_COLOR_ACTIVE = 0xFFFFFFFF_rgbaf;

    ScrollablePanel* _panel;

    Magnum::Matrix3 _matrix;
    const Magnum::Matrix3* _parentMatrix = nullptr;
    Magnum::Float _translationGap = .0f;

    struct Vertex {
        Magnum::Vector2 position;
    };

    void _onToggled(bool isToggled) final;

    void _onAnimationProgress() final;

    Magnum::GL::Buffer _bufferScroller;
    Corrade::Containers::StaticArray<4, Vertex> _verticesScroller;
    Magnum::GL::Mesh _meshScroller{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Range2D _geomScrollerPx;

    // if mouse is over placeholder
    void _onHoverChanged(bool isHovered) final;

    void handleLockMoveEvent(MouseMove_EH::EventType &event) final;

    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final;

    void _updateGeometry();

    Magnum::Color4 _scrollerColor;
    void _updateScrollColor();

    const Magnum::Matrix3* _matrixUpdateRequested(const Magnum::Matrix3* parentMatrix) final;

    void _setup();
};

}  // namespace Widget

}  // namespace UnderStory
