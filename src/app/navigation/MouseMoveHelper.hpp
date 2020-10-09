#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include "base/BaseUIPlayerHelper.hpp"
#include "base/states/MouseMovements.h"

using Magnum::Platform::Sdl2Application;

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
