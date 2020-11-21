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

#include <vector>

#include "Hoverable.hpp"

namespace UnderStory {

namespace Widget {

// TODO(amphaal) when contained is moving out of cursor (animated / deleted), refresh hover
class Container : public Hoverable {
 public:
    Container() {}

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

    // parent - children hovered : can be 'this' (parent is hovered), 'nullptr' (parent is not hovered), or any child ptr
    const Hoverable* latestHovered() const {
        return _latestHoveredShape;
    }

 protected:
    void _initContaining(std::initializer_list<UnderStory::Widget::Hoverable*> prioritized) {
        for(auto tcc : prioritized) 
            _pushContaining(tcc);
    }

    void _pushContaining(UnderStory::Widget::Hoverable* toBeContained) {
        _innerShapes.push_back(toBeContained);
        toBeContained->_setAsParent(this);
    }

    // same as ::Hoverable, but returns deepest child hovered instead of direct child
    const Hoverable* _checkIfMouseOver(const Magnum::Vector2 &cursorPos) final {
       // always check if previously subshape hovered is still hovered or not
       if(_latestHoveredShape && _latestHoveredShape != this) {
           _latestHoveredShape->_checkIfMouseOver(cursorPos);
       }

       // check if container is hovered
       Hoverable::_checkIfMouseOver(cursorPos);
       if(!this->isHovered()) {
           // if not, reset state and return
           _updateLatestHoveredShape(nullptr);
           return nullptr;
        }

        // iterate through innerShapes
        for(auto innerShape : _innerShapes) {
            // since 'latest' is always checked first, no changes happened
            if(innerShape == _latestHoveredShape && innerShape->isHovered()) 
                return _latestHoveredShape;

            // check
            auto deeperHovered = innerShape->_checkIfMouseOver(cursorPos);

            // found hovering, update state and immediate return
            if(innerShape->isHovered()) {
                _updateLatestHoveredShape(innerShape);
                return deeperHovered;
            }
        }

        // if no shapes fills, set state to self
        _updateLatestHoveredShape(this);
        return this;
    }

 private:
    std::vector<Hoverable*> _innerShapes;
    Hoverable* _latestHoveredShape = nullptr;

    void _updateLatestHoveredShape(Hoverable* hoverable) {
        if(_latestHoveredShape == hoverable) return;
        _latestHoveredShape = hoverable;

        // debug output
        #ifdef _DEBUG
            // _traceHoverable(hoverable);
        #endif
    }
};

}  // namespace Widget

}  // namespace UnderStory
