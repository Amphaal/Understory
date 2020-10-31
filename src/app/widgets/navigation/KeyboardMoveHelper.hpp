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

#include <Magnum/Animation/Easing.h>

#include <Magnum/Platform/Sdl2Application.h>

#include "../animation/BaseUIHelper.h"
#include "KeyboardMovements.hpp"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Navigation {

class KeyboardMoveHelper : public Animation::BaseUIHelper {
 public:
     KeyboardMoveHelper(Magnum::Matrix3* mainMatrix) : BaseUIHelper(mainMatrix) {}

    void stopAnim() override {
        _km = KeyboardMovements();
        _x = AxisMomentum();
        _y = AxisMomentum();
    }

    void advance() override {
        _considerAxis(_x);
        _considerAxis(_y);
    }

    void keyPressEvent(Sdl2Application::KeyEvent& event) { _keyEvent(event, true); }
    void keyReleaseEvent(Sdl2Application::KeyEvent& event) { _keyEvent(event, false); }

 private:
    AxisMomentum _x;
    AxisMomentum _y;

    static inline float MAX_SPEED_PER_SEC = 1.5f;
    static inline float MAX_SPEED_REACHED_IN_MS = .400f;

    KeyboardMovements _km;

    void _considerAxis(AxisMomentum &axis) {
        if(!axis.currentDirection.isZero()) {  // if current direction is set on axis, means acceleration
            //
            auto fd = timeline()->previousFrameDuration();
            if(axis.timePassed < MAX_SPEED_REACHED_IN_MS) {
                axis.timePassed += fd;
                //
                if(axis.timePassed > MAX_SPEED_REACHED_IN_MS)
                    axis.timePassed = MAX_SPEED_REACHED_IN_MS;
            }

            _updateSpeed(axis, false);
            _updateMatrix(axis, fd, false);

        } else if(axis.timePassed) {  // decelerating, still has momentum
            //
            auto fd = timeline()->previousFrameDuration();
            axis.timePassed -= fd;

            //
            if(axis.timePassed < .0f)
                axis.timePassed = 0;

            _updateSpeed(axis, true);
            _updateMatrix(axis, fd, true);
        }
    }

    void _updateMatrix(const AxisMomentum &axis, float frameDuration, bool deceleration) const {
        auto &v = deceleration ? axis.momentumDirection : axis.currentDirection;
        auto translationMatrix = Magnum::Matrix3::translation(v * Magnum::Vector2{axis.speed * frameDuration});
        (*_mainMatrix) = translationMatrix * (*_mainMatrix);
    }

    void _updateSpeed(AxisMomentum &axis, bool deceleration) {
        auto f = deceleration ? Magnum::Animation::Easing::cubicIn: Magnum::Animation::Easing::cubicOut;

        axis.speed = Magnum::Math::lerp(
            .0f,
            MAX_SPEED_PER_SEC,
            f(axis.timePassed / MAX_SPEED_REACHED_IN_MS)
        );
    }

    void _keyEvent(Sdl2Application::KeyEvent& event, bool isPress) {
        //
        if (event.isRepeated()) return;

        //
        auto key = event.key();
        auto isMovement = _km.ackMovement(key, isPress);
        if(isMovement) {
            //
            event.setAccepted();

            //
            _km.updateMomentum(_x, _y, key);
        }
    }
};

}  // namespace Navigation

}  // namespace UnderStory
