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

#include "base/BaseUIPlayerHelper.hpp"
#include "base/states/MouseMovements.h"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Navigation {

class MouseMoveHelper : public BaseUIPlayerHelper<> {
 public:
     MouseMoveHelper(Magnum::Timeline* timeline, Magnum::Matrix3* mainMatrix) : BaseUIPlayerHelper(timeline, mainMatrix, .2f) {}

    void stopAnim() final {
       BaseUIPlayerHelper::stopAnim();
       if(_mm.isComputing()) _mm.resetAccelerationBuffer();
    }

    void mousePressEvent() { stopAnim(); }
    void mouseMoveEvent(const Sdl2Application::MouseMoveEvent& event, const Magnum::Platform::Application* app) {
        //
        auto moveTr = MouseMovements::translatedVector(event, app);

        //
        _mm.addToAccelerationBuffer(moveTr);

        //
        _translateToAbs(moveTr);
    }
    void mouseDoubleClickEvent(const Sdl2Application::MouseEvent& event, const Magnum::Platform::Application* app) {
        auto moveTr = MouseMovements::translatedVector(event, app);
        _updateAnimationFromMoveVectorAndPlay(moveTr);
    }
    void mouseMoveReleaseEvent() {
        auto moveTr = _mm.computeAccelerationBuffer();
        _updateAnimationFromMoveVectorAndPlay(moveTr);
    }

 private:
    MouseMovements _mm;

    void _mayUpdateMainMatrix() final {
         _translateTo(currentAnim());
    }

    void _updateAnimationFromMoveVectorAndPlay(const Magnum::Vector2 &moveTr) {
        auto from = mainMatrix()->translation();
        BaseUIPlayerHelper::_updateAnimationAndPlay(from, from + moveTr);
    }

    void _translateTo(const Magnum::Vector2 &translationVector) {
        _multiplyWithMainMatrix(Magnum::Matrix3::translation(translationVector - mainMatrix()->translation()));
    }

    void _translateToAbs(const Magnum::Vector2 &translationVector) {
        _multiplyWithMainMatrix(Magnum::Matrix3::translation(translationVector));
    }
};

}  // namespace Navigation

}  // namespace UnderStory
