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

#include <Magnum/Animation/Track.h>
#include <Magnum/Animation/Easing.h>
#include <Magnum/Animation/Player.h>

#include "BaseUIHelper.hpp"
#include "State.hpp"

namespace UnderStory {

namespace Animation {

template<class T = Magnum::Vector2>
class BaseUIPlayerHelper : public BaseUIHelper {
 public:
    //
    using AnimationCallback = void(*)(Magnum::Float, const float &, State<T>&);

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
        if(!isAnimationPlaying()) return;
        _onAnimationProgress();
    }

    bool isAnimationPlaying() const {
        return _player.state() == Magnum::Animation::State::Playing;
    }

 protected:
    void virtual _onAnimationProgress() = 0;

    const Magnum::Matrix3* mainMatrix() const { return _mainMatrix; }
    const T& currentAnim() const { return _animState.current; }
    const State<T>& animState() const { return _animState; }

    void _multiplyWithMainMatrix(const Magnum::Matrix3& factorMatrix) {
        (*_mainMatrix) = factorMatrix * (*_mainMatrix);
    }

    void _replaceMainMatrix(const Magnum::Matrix3& replacingMatrix) {
        (*_mainMatrix) = replacingMatrix;
    }

    void _updateAnimationAndPlay(const T &from, const T &to) {
        _animState.from = from;
        _animState.to = to;
        _animState.current = T();
        _startAnim();
    }

 private:
    State<T> _animState;

    void _startAnim() final {
        BaseUIHelper::_startAnim();
        _player.play(_timeline->previousFrameTime());
    }

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, State<T>& state) {
        state.current = Magnum::Math::lerp(
            state.from,
            state.to,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    Magnum::Animation::Player<Magnum::Float> _player;
    Magnum::Animation::Track<Magnum::Float, float> _track;
};

}  // namespace Animation

}  // namespace UnderStory
