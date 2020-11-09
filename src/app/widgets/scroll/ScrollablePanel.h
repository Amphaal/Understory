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
#include "src/app/widgets/base/Container.hpp"
#include "src/app/widgets/base/Toggleable.hpp"

#include "Scroller.hpp"

#include "src/app/shaders/Shaders.hpp"

#include "Scissorable.h"

namespace UnderStory {

namespace Widget {

class ScrollablePanel : public Animation::PlayerMatrixAnimator<Magnum::Vector2>, public Container, public Toggleable {
 public:
    explicit ScrollablePanel(StickTo stickness = StickTo::Left, float thickness = .6f);

    void mayDraw();

    void _bindContent(Scissorable* content);

    const Magnum::Matrix3& matrix() const;

    void onMouseScroll(const Magnum::Vector2& scrollOffset);

    const GrowableAxis _contentGrowableAxis() const;

 private:
    StickTo _stickness;
    float _thickness;

    Scroller _scroller;
    Scissorable* _content;

    Magnum::Matrix3 _matrix;

    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Animation::State<Magnum::Vector2>& state);

    void _onHoverChanged(bool isHovered) final;

    void _onAnimationProgress() final;

    void _onToggled(bool isToggled) final;

    // helper
    void _updateGeometry();

    void onViewportChange(Magnum::Range2D& shapeAllowedSpace) final;

    void _definePanelPosition(const Magnum::Vector2 &pos);

    // scroller position within panel
    const StickTo _scrollerStickyness() const;

    const Magnum::Vector2 _collapsedTransform() const;

    bool _setupDone = false;
    void _availableSpaceChanged(Magnum::Range2D& availableSpace) final;
};

}  // namespace Widget

}  // namespace UnderStory
