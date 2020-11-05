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

#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Magnum.h>

#include <utility>

namespace UnderStory {

namespace Widget {

class Container;

class Shape {
 public:
    // no constructor, since shapes are set within screen constraints
    Shape() {}
    friend class Container;

    // defaults to update its geometry, but might require
    virtual void onViewportChange(const Constraints &wh) {
        _geometryUpdateRequested();
    }

    // inplaced shape (eg within OpenGL original coordinates, mostly {-1.f} <> {1.f})
    const Magnum::Range2D& shape() const {
        return _shape;
    }

    // shape with matrix transforms applied, allows to determine current position
    const Magnum::Range2D& geometry() const {
        return _geometry;
    }

 protected:
    virtual void _geometryUpdateRequested() = 0;

    void _updateShape(const Magnum::Range2D& shape) {
        _shape = shape;
    }
    void _updateGeometry(const Magnum::Range2D& geometry) {
        _geometry = geometry;
    }

    // can be called from Container, must update "shapeAllowedSpace" with remaining space
    virtual void _updateShapeFromConstraints(const Constraints &wh, Magnum::Range2D& shapeAllowedSpace) {};

 private:
    Magnum::Range2D _geometry;
    Magnum::Range2D _shape;
};

}  // namespace Widget

}  // namespace UnderStory
