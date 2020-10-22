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

#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Magnum.h>

#include <utility>

namespace UnderStory {

namespace Widget {

template<class T = Magnum::Range2D>
class Hoverable {
 public:
    explicit Hoverable(Magnum::Platform::Application* app) : _app(app) {}

    bool isHovered() const {
        return _isHovered;
    }

    virtual void onViewportChange(const Constraints &wh) {
        _updateGeometry();
    }

    virtual void onMouseMove(const Magnum::Vector2 &cursorPos) {
        // prevent updating if state did not change
        auto hovered = _geometry.contains(cursorPos);
        if(_isHovered == hovered) return;

        // update state
        _isHovered = hovered;

        //
        _onHoverChanged(_isHovered);
    }

 protected:
    Magnum::Platform::Application* _app;

    T _geometry;
    virtual void _updateGeometry() = 0;

    virtual void _onHoverChanged(bool isHovered) {}

 private:
    bool _isHovered = false;
};

}  // namespace Widget

}  // namespace UnderStory
