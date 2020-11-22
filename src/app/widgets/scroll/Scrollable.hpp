    
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

#include <functional>

#include "src/app/widgets/base/Constraints.hpp"

namespace UnderStory {

namespace Widget {

enum class GrowableAxis {
    Width,
    Height
};

class Scrollable {
 public:
    using AxisFunction = std::function<Magnum::Vector2(Magnum::Float)>;
    explicit Scrollable(StickTo parentStickness) : 
        _axisFn(_getAxisFunction(parentStickness)), 
        _growableAxis(_getGrowableAxis(parentStickness)) {}
 
 protected:
    const AxisFunction _axisFn;
    const GrowableAxis _growableAxis;

 private:
    static AxisFunction _getAxisFunction(StickTo parentStickness) {
        switch (parentStickness) {
            case StickTo::Left :
            case StickTo::Right :
                return Magnum::Vector2::yAxis;
            case StickTo::Top :
            case StickTo::Bottom :
                return Magnum::Vector2::xAxis;
        }
    }
    
    GrowableAxis _getGrowableAxis(StickTo parentStickness) {
        switch (parentStickness) {
            case StickTo::Left :
            case StickTo::Right :
                return GrowableAxis::Height;
            case StickTo::Top :
            case StickTo::Bottom :
                return GrowableAxis::Width;
        }
    }
};

}  // namespace Widget

}  // namespace UnderStory
