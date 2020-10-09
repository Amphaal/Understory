#pragma once

#include <Magnum/Animation/Easing.h>

#include <Magnum/Platform/Sdl2Application.h>

#include "base/BaseUIHelper.hpp"
#include "base/states/KeyboardMovements.h"

using Magnum::Platform::Sdl2Application;

class KeyboardMoveHelper : public BaseUIHelper {
 public:
     KeyboardMoveHelper(Magnum::Timeline* timeline, Magnum::Matrix3* mainMatrix) : BaseUIHelper(timeline, mainMatrix) {}

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
            auto fd = _timeline->previousFrameDuration();
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
            auto fd = _timeline->previousFrameDuration();
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
