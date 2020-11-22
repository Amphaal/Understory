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

#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Matrix3.h>

namespace UnderStory {

namespace Widget {

enum class StickTo {
    Left,
    Top,
    Right,
    Bottom
};

class Constraints {
 public:
    Constraints() {}

    explicit Constraints(const Magnum::Vector2i &windowSize) :
        _ws(windowSize),
        _pixelProjMatrix(Magnum::Matrix3::projection(_ws)),
        _projMatrix(Magnum::Matrix3::projection(Magnum::Vector2 { _ws.aspectRatio(), 1.f } * 2.f)),
        _pixelSize(Magnum::Vector2 {1.f} / _ws) {}

    const Magnum::Vector2& ws() const {
        return _ws;
    }

    const Magnum::Matrix3& pixelProjMatrix() const {
        return _pixelProjMatrix;
    }

    const Magnum::Matrix3& projMatrix() const {
        return _projMatrix;
    }

    const Magnum::Vector2& pixelSize() const {
        return _pixelSize;
    }

 private:
    Magnum::Vector2 _ws;
    Magnum::Matrix3 _pixelProjMatrix;
    Magnum::Matrix3 _projMatrix;
    Magnum::Vector2 _pixelSize;
};

}  // namespace Widget

}  // namespace UnderStory
