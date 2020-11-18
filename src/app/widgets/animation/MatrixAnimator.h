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
#include <Magnum/Timeline.h>
#include <Magnum/Math/Matrix3.h>

#include <vector>

#include "TimelineBound.h"

namespace UnderStory {

namespace Animation {

class MatrixAnimator : public TimelineBound {
 public:
    explicit MatrixAnimator(Magnum::Matrix3* animatedMatrix);

    virtual void stopAnim() = 0;
    virtual void advance() = 0;

    void setExcludedWhenPlaying(std::initializer_list<MatrixAnimator*> list);

 protected:
    Magnum::Matrix3* _animatedMatrix = nullptr;

    // must be overriden and reused
    virtual void _startAnim();

 private:
    std::vector<MatrixAnimator*> _excluded;
};

}  // namespace Animation

}  // namespace UnderStory
