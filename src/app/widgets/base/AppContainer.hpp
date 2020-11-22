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

#include "EventHandlers.hpp"
#include "Container.hpp"

namespace UnderStory {

namespace Widget {

// Container with static shape and geometry
class AppContainer : public Container, public ScrollEventHandler {
 public:
    AppContainer() {
        Magnum::Range2D bounds {
            {-1.f, -1.f},
            {1.f, 1.f}
        };

        _updateShape(bounds);
        _updateGeometry(bounds);
    }

 protected:
    void _traverseForHovered(const Magnum::Vector2 &cursorPos) {
        auto deepHovered = _checkIfMouseOver(cursorPos);
        if(_deepestHoveredShape != deepHovered) {
            _deepestHoveredShape = deepHovered;
            // _traceHoverable(_deepestHoveredShape);
        }
    }

    void _propagateScrollEvent(ScrollEventHandler::EventType &event) {
        _propagateEvent<ScrollEventHandler, ScrollEventHandler::EventType>(_deepestHoveredShape, event);
    }

 private:
    // get the 'latestHovered', but searching through the deepest in the stack
    Hoverable* _deepestHoveredShape = nullptr;

    template<class Hndlr, class EvT>
    void _propagateEvent(Hoverable* target, EvT &event) {
        if(!target) return;
        if(auto e = dynamic_cast<Hndlr*>(target)) {
            e->_handleEvent(event);
            return;
        }
        _propagateEvent<Hndlr, EvT>(target->parent(), event);
    }
};

}  // namespace Widget

}  // namespace UnderStory
