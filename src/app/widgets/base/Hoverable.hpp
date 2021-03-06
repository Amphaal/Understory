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

#include <spdlog/spdlog.h>

#include "src/base/Debug.hpp"

#include "src/app/utility/AppBound.hpp"
#include "Shape.hpp"

namespace UnderStory {namespace Widget { class Container; }}

namespace UnderStory {

namespace Widget {

class Hoverable : public Shape, public AppBound {
 friend class Container;

 public:
    Hoverable() {}

    bool isHovered() const {
        return _isHovered;
    }

    // defaults to update its geometry, but might require
    virtual void onViewportChange(Magnum::Range2D& shapeAllowedSpace) {
        _availableSpaceChanged(shapeAllowedSpace);
    }

    // shape with matrix transforms applied, allows to determine current position
    const Magnum::Range2D& geometry() const {
        return _geometry;
    }

    Container* parent() const {
        return _parent;
    }

 protected:
    void _setAsParent(Container* parent) {
        _parent = parent;
    }

    void _updateGeometry(const Magnum::Matrix3 &matrix) {
        _updateGeometry(Magnum::Range2D {
            matrix.transformPoint(shape().min()),
            matrix.transformPoint(shape().max())
        });
    }

    void _updateGeometry(const Magnum::Range2D& geometry) {
        _geometry = geometry;
    }

    // update current matrix and returns a reference of it
    virtual const Magnum::Matrix3* _matrixUpdateRequested(const Magnum::Matrix3* parentMatrix) {
        return parentMatrix;
    }

    // returns 'this' if mouse is over, else 'nullptr'
    virtual Hoverable* _checkIfMouseOver(const Magnum::Vector2 &cursorPos) {
        // check geom
        auto hovered = this->geometry().contains(cursorPos);

        // if state changed
        if(_isHovered != hovered) {
            // update state
            _isHovered = hovered;

            // callback...
            _onHoverChanged(_isHovered);
        }

        //
        return hovered ? this : nullptr;
    }

    // reimplement this callback to know when hover changed
    virtual void _onHoverChanged(bool isHovered) {}

    #ifdef _DEBUG
        void _traceSelf() const {
            auto self = Debug::demanglePtr(this);
            self = "[" + self + "]";
            spdlog::debug(self);
        }

        void _traceHoverable(Hoverable* hoverable) const {
            auto self = Debug::demanglePtr(this);
            self = "[" + self + "]";

            //
            if(!hoverable) {
                spdlog::debug("{} : Out-of-frame", self);
            } else {
                spdlog::debug("{} : {}", self, Debug::demanglePtr(hoverable));
            }
        }
    #endif

 private:
    Container* _parent = nullptr;
    Magnum::Range2D _geometry;
    bool _isHovered = false;
};

}  // namespace Widget

}  // namespace UnderStory
