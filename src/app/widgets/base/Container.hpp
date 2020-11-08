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

#include "Hoverable.hpp"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>

namespace UnderStory {

namespace Widget {

// TODO(amphaal) when contained is moving out of cursor (animated / deleted), refresh hover
class Container : public Hoverable {
 public:
    Container() {}

    void bind(std::initializer_list<UnderStory::Widget::Hoverable*> prioritized) {
        _innerShapes = Corrade::Containers::Array<Hoverable*>();
        Magnum::Containers::arrayReserve(_innerShapes, prioritized.size());
        Magnum::Containers::arrayAppend(_innerShapes, prioritized);
    }

    virtual void onViewportChange(Magnum::Range2D& shapeAllowedSpace) {
        // might update shape
        Hoverable::onViewportChange(shapeAllowedSpace);

        // get new shape of container
        auto shape = this->shape();

        // apply new shape to innerShapes
        for(auto innerShape : _innerShapes) {
            innerShape->onViewportChange(shape);
        }
    }

    void checkIfMouseOver(const Magnum::Vector2 &cursorPos) final {
       // always check if previously subshape hovered is still hovered or not
       if(_latestHoveredShape && _latestHoveredShape != this) {
           _latestHoveredShape->checkIfMouseOver(cursorPos);
       }

       // check if container is hovered
       Hoverable::checkIfMouseOver(cursorPos);
       if(!this->isHovered()) {
           // if not, reset state and return
           _updateLatestHoveredShape(nullptr);
           return;
        }

        // iterate through innerShapes
        for(auto innerShape : _innerShapes) {
            //
            if(innerShape == _latestHoveredShape && innerShape->isHovered()) return;

            // check
            innerShape->checkIfMouseOver(cursorPos);

            // found hovering, update state and immediate return
            if(innerShape->isHovered()) {
                _updateLatestHoveredShape(innerShape);
                return;
            }
        }

        // if no shapes fills, set state to self
        _updateLatestHoveredShape(this);
    }

    Hoverable* latestHovered() const {
        return _latestHoveredShape;
    }

 private:
    Corrade::Containers::Array<Hoverable*> _innerShapes;
    Hoverable* _latestHoveredShape = nullptr;

    void _updateLatestHoveredShape(Hoverable* hoverable) {
        if(_latestHoveredShape == hoverable) return;
        _latestHoveredShape = hoverable;
    }
};

// Container with static shape and geometry
class AppContainer : public Container {
 public:
    AppContainer() {
        Magnum::Range2D bounds {
            {-1.f, -1.f},
            {1.f, 1.f}
        };

        _updateShape(bounds);
        _updateGeometry(bounds);
    }
};

}  // namespace Widget

}  // namespace UnderStory
