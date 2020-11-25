    
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

#include "src/app/widgets/animation/PlayerMatrixAnimator.hpp"

namespace UnderStory {

namespace Widget {

enum class GrowableAxis {
    Width,
    Height
};

class Scrollable : public Animation::PlayerMatrixAnimator<float> {
 public:
    using AxisFunction = std::function<Magnum::Vector2(Magnum::Float)>;
    explicit Scrollable(StickTo parentStickness) : 
        PlayerMatrixAnimator(&_scrllMatrix, .2f),
        _axisFn(_getAxisFunction(parentStickness)), 
        _growableAxis(_getGrowableAxis(parentStickness)) {}
 
 protected:
    const GrowableAxis _growableAxis;

    const Magnum::Matrix3& _scrollMatrix() const {
        return _scrllMatrix;
    }

    void _animateScrollByTr(Magnum::Float tr) {
        _updateAnimationAndPlay(currentAnim(), tr);
    }
    
    void _onAnimationProgress() override {
        _scrllMatrix = _scrollTranslate(currentAnim());
    }
    
    Magnum::Float _extractScrollableSize(const Magnum::Range2D &rect) const {
        return _extractScrollableSize(rect.size());
    }

    Magnum::Float _extractScrollableSize(const Magnum::Vector2 &vect) const {
        switch(_growableAxis) {
            case GrowableAxis::Width:
                return vect.x();
            case GrowableAxis::Height:
                return vect.y();
        }
    }

 private:
    const AxisFunction _axisFn;
    Magnum::Matrix3 _scrllMatrix;

    Magnum::Matrix3 _scrollTranslate(Magnum::Float val) {
        return Magnum::Matrix3::translation(_axisFn(val));
    }

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
