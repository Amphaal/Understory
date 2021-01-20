// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020-2021 Guillaume Vara <guillaume.vara@gmail.com>

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
#include "src/app/widgets/base/Container.hpp"

#include "src/app/shaders/Shaders.hpp"

#include "ScrollerHandle.h"

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

class ScrollablePanel;

class Scroller : public Container {
 public:
    explicit Scroller(ScrollablePanel* panel);

    void mayDraw();

    ScrollerHandle& handle();

    // callback when content ratio to canvas changed
    void onContentRatioChanged(const Magnum::Float& contentRatio);

 private:
    static constexpr float THICKNESS_PX = 20.f;
    static constexpr float PADDING_PX = 10.f;
    static inline Magnum::Color4 PH_COLOR = 0xFFFFFF44_rgbaf;

    Magnum::Matrix3 _matrix;

    ScrollablePanel* _associatedPanel;
    ScrollerHandle _handle;
    StickTo _stickness;

    bool _contentBigEnough = false;

    struct Vertex {
        Magnum::Vector2 position;
    };

    Magnum::GL::Buffer _bufferPh;
    Corrade::Containers::StaticArray<4, Vertex> _verticesPh;
    Magnum::GL::Mesh _meshPh{Magnum::GL::MeshPrimitive::Triangles};
    Magnum::Range2D _geomPhPx;

    // scroller position within panel
    static StickTo _stickynessFromPanel(const ScrollablePanel* panel);

    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final;
    const Magnum::Matrix3* _matrixUpdateRequested(const Magnum::Matrix3* parentMatrix) final;

    void _updateGeometry();

    void _setup();
};

}  // namespace Widget

}  // namespace UnderStory
