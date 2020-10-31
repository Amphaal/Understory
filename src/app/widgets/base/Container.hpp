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
template<class T = Magnum::Range2D>
class Container : public Hoverable<T> {
 public:
    explicit Container() {}

    void bind(std::initializer_list<UnderStory::Widget::Hoverable<T>*> prioritized) {
        _innerShapes = Corrade::Containers::Array<Hoverable<T>*>();
        Magnum::Containers::arrayReserve(_innerShapes, prioritized.size());
        Magnum::Containers::arrayAppend(_innerShapes, prioritized);
    }

    virtual void onViewportChange(const Constraints &wh) {
        Shape<T>::onViewportChange(wh);

        for(auto innerShape : _innerShapes) {
            innerShape->onViewportChange(wh);
        }
    }

    void checkIfMouseOver(const Magnum::Vector2 &cursorPos) final {
       // always check if previously subshape hovered is still hovered or not
       if(_latestHoveredShape && _latestHoveredShape != this) {
           _latestHoveredShape->checkIfMouseOver(cursorPos); 
       }
       
       // check if container is hovered
       Hoverable<T>::checkIfMouseOver(cursorPos);
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

    Hoverable<T>* latestHovered() const {
        return _latestHoveredShape;
    }

 private:
    Corrade::Containers::Array<Hoverable<T>*> _innerShapes;
    Hoverable<T>* _latestHoveredShape = nullptr;

    void _updateLatestHoveredShape(Hoverable<T>* hoverable) {
        if(_latestHoveredShape == hoverable) return;
        _latestHoveredShape = hoverable;
        // Magnum::Debug{} << _latestHoveredShape;
    }
};

}  // namespace Widget

}  // namespace UnderStory
