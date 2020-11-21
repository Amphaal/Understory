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

#include "src/app/widgets/base/Constraints.hpp"

namespace UnderStory {

namespace Widget {

class Container;

class Shape {
 friend class Container;

 public:
    // no constructor, since shapes are set within screen constraints
    Shape() {}

    // inplaced shape (eg within OpenGL original coordinates, mostly {-1.f} <> {1.f})
    const Magnum::Range2D& shape() const {
        return _shape;
    }

    void setConstraints(const Magnum::Vector2i &windowSize) {
        _currentConstraints = Widget::Constraints { windowSize };
    }

 protected:
    void _updateShape(const Magnum::Range2D& shape) {
        _shape = shape;
    }

    // to be overriden if size is relative to a parent Container size
    virtual void _availableSpaceChanged(Magnum::Range2D& availableSpace) {}

    static Constraints& constraints() {
        return _currentConstraints;
    }

    static const Magnum::Range2D _shapeIntoPixel(Magnum::Range2D shape) {
        _vectorIntoPixel(shape.min());
        _vectorIntoPixel(shape.max());
        return shape;
    }

 private:
    Magnum::Range2D _shape;

    static inline Constraints _currentConstraints;

    static void _vectorIntoPixel(Magnum::Vector2 &vector) {
        vector = (vector + Magnum::Vector2{1.f}) / 2.f / constraints().pixelSize();
    }
};

}  // namespace Widget

}  // namespace UnderStory
