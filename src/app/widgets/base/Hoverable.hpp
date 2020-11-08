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

#include "src/app/utility/AppBound.hpp"
#include "Shape.hpp"

namespace UnderStory {

namespace Widget {

class Hoverable : public Shape, public AppBound {
 public:
    Hoverable() {}

    bool isHovered() const {
        return _isHovered;
    }

    // defaults to update its geometry, but might require
    virtual void onViewportChange(Magnum::Range2D& shapeAllowedSpace) {
        _availableSpaceChanged(shapeAllowedSpace);
    }

    virtual void checkIfMouseOver(const Magnum::Vector2 &cursorPos) {
        // prevent updating if state did not change
        auto hovered = this->geometry().contains(cursorPos);

        // check subelements hovering
        _mouseIsOver(cursorPos);

        // dont go further if state did not change
        if(_isHovered == hovered) return;

        // update state
        _isHovered = hovered;

        //
        _onHoverChanged(_isHovered);
    }

    // shape with matrix transforms applied, allows to determine current position
    const Magnum::Range2D& geometry() const {
        return _geometry;
    }

 protected:
    void _updateGeometry(const Magnum::Matrix3 &matrix) {
        _updateGeometry(Magnum::Range2D {
            matrix.transformPoint(shape().min()),
            matrix.transformPoint(shape().max())
        });
    }

    void _updateGeometry(const Magnum::Range2D& geometry) {
        _geometry = geometry;
    }

    virtual void _onHoverChanged(bool isHovered) {}
    virtual void _mouseIsOver(const Magnum::Vector2 &cursorPos) {}

 private:
    Magnum::Range2D _geometry;
    bool _isHovered = false;
};

}  // namespace Widget

}  // namespace UnderStory
