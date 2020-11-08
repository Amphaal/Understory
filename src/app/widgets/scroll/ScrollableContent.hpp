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

#include "src/app/widgets/base/Hoverable.hpp"

namespace UnderStory {

namespace Widget {

class ScrollableContent : public Hoverable {
 public:
    ScrollableContent(ScrollableContent&& content, const Magnum::Matrix3* panelMatrix, GrowableAxis ga) :
        ScrollableContent(content), _ga(ga), _panelMatrix(panelMatrix) {}

    virtual void draw() = 0;
    virtual Hoverable* asHoverable() = 0;

 private:
    GrowableAxis _ga;
    const Magnum::Matrix3* _panelMatrix;
};

}  // namespace Widget

}  // namespace UnderStory
