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

#include "EventHandlers.hpp"
#include "Container.hpp"

namespace UnderStory {

namespace Widget {

// Container with static shape and geometry
class AppContainer : 
    public Container, 
    public Scroll_EH, 
    public MousePress_EH, 
    public MouseRelease_EH, 
    public MouseMove_EH {

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
    void _propagateScrollEvent(Scroll_EH::EventType &event) {
        _propagateEvent<Scroll_EH>(_deepestHoveredShape, event);
    }

    void _propagateMousePressEvent(MousePress_EH::EventType &event) {
        // update mouse state
        switch (event.button()) {
            case Magnum::Platform::Sdl2Application::MouseEvent::Button::Left:
                _mouseState._leftPressed();
            break;

            case Magnum::Platform::Sdl2Application::MouseEvent::Button::Right:
                _mouseState._rightPressed();
            break;

            default:
            break;
        }

        // update lock context
        _lockContext = _deepestHoveredShape;

        // handle event from it only
        _tryHandleEvent<MousePress_EH>(_lockContext, event);
    }

    void _propagateMouseMoveEvent(MouseMove_EH::EventType &event, const Magnum::Vector2 &cursorPos) {
        // find hovered
        _traverseForHovered(cursorPos);
        
        // may update mouse state as dragging
        _mouseState._mayBeDragging();

        // handle lock context only event
        _tryHandleEvent<MouseMove_EH>(_lockContext, event);
    }

    void _propagateMouseReleaseEvent(MouseRelease_EH::EventType &event) {
        // handle event from lock context only
        _tryHandleEvent<MouseRelease_EH>(_lockContext, event);

        // update mouse state
        switch (event.button()) {
            case Magnum::Platform::Sdl2Application::MouseEvent::Button::Left:
                _mouseState._leftReleased();
            break;

            case Magnum::Platform::Sdl2Application::MouseEvent::Button::Right:
                _mouseState._rightReleased();
            break;

            default:
            break;
        }

        // then clears lock context
        _lockContext = nullptr;
    }

 private:
    // get the 'latestHovered', but searching through the deepest in the stack
    Hoverable* _deepestHoveredShape = nullptr;

    void _traverseForHovered(const Magnum::Vector2 &cursorPos) {
        auto deepHovered = _checkIfMouseOver(cursorPos);
        if(_deepestHoveredShape != deepHovered) {
            _deepestHoveredShape = deepHovered;
            // _traceHoverable(_deepestHoveredShape);
        }
    }

    // locked hoverable context
    Hoverable* _lockContext = nullptr;

    template<class Hndlr, class EvT>
    void _propagateEvent(Hoverable* target, EvT &event) {
        if(!target) return;
        if(_tryHandleEvent<Hndlr, EvT>(target, event)) return;
        _propagateEvent<Hndlr, EvT>(target->parent(), event);
    }

    // returns true if event has been handled 
    template<class Hndlr, class EvT>
    bool _tryHandleEvent(Hoverable* target, EvT &event) {
        if(auto e = dynamic_cast<Hndlr*>(target)) {
            e->_handleEvent(event);
            return true;
        }

        return false;
    }
};

}  // namespace Widget

}  // namespace UnderStory
