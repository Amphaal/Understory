#pragma once

#include <Magnum/Animation/Track.h>
#include <Magnum/Animation/Easing.h>
#include <Magnum/Animation/Player.h>

#include "BaseUIHelper.hpp"
#include "states/AnimationState.h"

template<class T = Magnum::Vector2>
class BaseUIPlayerHelper : public BaseUIHelper {
 public:
    //
    using AnimationCallback = void(*)(Magnum::Float, const float &, AnimationState<T>&);

    //
    BaseUIPlayerHelper(Magnum::Timeline* timeline, Magnum::Matrix3* mainMatrix, float animDurationInSecs, AnimationCallback animCb = &_defaultAnimationCallback)
    : BaseUIHelper(timeline, mainMatrix),
    _track({
        {.0f, .0f},
        {animDurationInSecs, 1.0f}
    }, Magnum::Animation::Interpolation::Linear) {
        // add callback
        _player.addWithCallback(_track, animCb, _animState);
    }

    void stopAnim() override {
        _player.stop();
    }

    void advance() final {
        _player.advance(_timeline->previousFrameTime());
        if(_player.state() != Magnum::Animation::State::Playing) return;
        _mayUpdateMainMatrix();
    }

 protected:
    void virtual _mayUpdateMainMatrix() = 0;

    const Magnum::Matrix3* mainMatrix() const { return _mainMatrix; }
    const T& currentAnim() const { return _animState.current; }
    const AnimationState<T>& animState() const { return _animState; }

    void _multiplyWithMainMatrix(const Magnum::Matrix3& factorMatrix) {
        (*_mainMatrix) = factorMatrix * (*_mainMatrix);
    }

    void _updateAnimationAndPlay(const T &from, const T &to) {
        _animState.from = from;
        _animState.to = to;
        _animState.current = T();
        _startAnim();
    }

 private:
    AnimationState<T> _animState;

    void _startAnim() final {
        BaseUIHelper::_startAnim();
        _player.play(_timeline->previousFrameTime());
    }

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, AnimationState<T>& state) {
        state.current = Magnum::Math::lerp(
            state.from,
            state.to,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    Magnum::Animation::Player<Magnum::Float> _player;
    Magnum::Animation::Track<Magnum::Float, float> _track;
};
