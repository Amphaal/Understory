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

#include "Container.hpp"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Widget {

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

    // get the 'latestHovered', but searching through the deepest in the stack
    const Hoverable* deepestHovered() const {
        return _deepestHoveredShape;
    }

    void traverseForHovered(const Magnum::Vector2 &cursorPos) {
        _deepestHoveredShape = _checkIfMouseOver(cursorPos);
    }

    void dispatchEvent(Sdl2Application::MouseScrollEvent& event) {
        
    }

 private:
    const Hoverable* _deepestHoveredShape = nullptr;
};

}  // namespace Widget

}  // namespace UnderStory
