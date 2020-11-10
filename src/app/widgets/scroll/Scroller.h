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

#include "src/app/shaders/Shaders.hpp"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollablePanel;

class Scroller : public Hoverable {
 public:
    explicit Scroller(const ScrollablePanel* panel);

    void mayDraw();

    void onMouseScroll(const Magnum::Vector2& scrollOffset);

    void onContentSizeChanged(const Magnum::Float& newContentSize);

    void reveal();

    void fade();

 private:
    const ScrollablePanel* _associatedPanel;
    StickTo _stickness;
    static constexpr float THICKNESS_PX = 20.f;
    static constexpr float PADDING_PX = 10.f;
    static inline Magnum::Color4 PH_COLOR = 0xFFFFFF44_rgbaf;
    static inline Magnum::Color4 SCRLL_COLOR_IDLE = 0xCCCCCCFF_rgbaf;
    static inline Magnum::Color4 SCRLL_COLOR_ACTIVE = 0xFFFFFFFF_rgbaf;
    bool _contentBigEnough = false;

    Magnum::GL::Buffer _buffer;
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};

    bool _isScrollerHovered = false;
    Magnum::Range2D _scrollerShape;
    Magnum::Range2D _phShape;

    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Color4 color;
    };
    Corrade::Containers::StaticArray<8, Vertex> _vertices;

    // scroller position within panel
    const StickTo _scrollerStickyness() const;

    void _updateGeometry();

    // if mouse is over placeholder
    void _mouseIsOver(const Magnum::Vector2 &cursorPos) final;

    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final;

    void _updateScrollColor();

    void _setup();
};

}  // namespace Widget

}  // namespace UnderStory
