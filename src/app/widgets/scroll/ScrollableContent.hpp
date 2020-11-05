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

#include "../base/Hoverable.hpp"

namespace UnderStory {

namespace Widget {

class ScrollableContent : public Hoverable {
 public:
    ScrollableContent(const Magnum::Matrix3* panelMatrix) : _panelMatrix(panelMatrix) {};
    
    void draw() {
        // TODO
    }

 private:
    const Magnum::Matrix3* _panelMatrix;

    void _geometryUpdateRequested() final {
        _updateGeometry(Magnum::Range2D {
            _panelMatrix->transformPoint(shape().min()),
            _panelMatrix->transformPoint(shape().max())
        });
    }

    void _updateShapeFromConstraints(const Constraints &wh, Magnum::Range2D& shapeAllowedSpace) final {
        // take all remaining space
        _updateShape(shapeAllowedSpace);
        shapeAllowedSpace = {};
    }
};

}  // namespace Widget

}  // namespace UnderStory
