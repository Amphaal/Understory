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

template<class T = Magnum::Range2D>
class Container : public Hoverable<T> {
 public:
    explicit Container(std::initializer_list<UnderStory::Widget::Hoverable*> containing = {}) : Hoverable<T>(app) {
        Magnum::Containers::arrayReserve(_innerShapes, containing.size());
        Magnum::Containers::arrayAppend(_innerShapes, containing);
    }

    void checkIfMouseOver(const Magnum::Vector2 &cursorPos) final {
       Hoverable::checkIfMouseOver(cursorPos);
    }

 private:
    Corrade::Containers::Array<Hoverable*> _innerShapes;
    Hoverable* _latestHoveredShape = nullptr;

    void _geometryUpdateRequested() final {

    }

    void _onHoverChanged(bool isHovered) final {
        if(_latestHoveredShape) _latestHoveredShape->checkIfMouseOver
    }
};

}  // namespace Widget

}  // namespace UnderStory
