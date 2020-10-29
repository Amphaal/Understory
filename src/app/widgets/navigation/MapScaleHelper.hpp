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

#include <utility>

#include "../animation/BaseUIPlayerHelper.hpp"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Navigation {

struct ScrollAnimStateComponent {
    Magnum::Vector2 translation;
    Magnum::Vector2 scaling;
};

class MapScaleHelper : public Animation::BaseUIPlayerHelper<ScrollAnimStateComponent> {
 public:
    explicit MapScaleHelper(Magnum::Matrix3* mainMatrix)
    : BaseUIPlayerHelper(mainMatrix, SMOOTHING_AS_SECONDS, &_defaultAnimationCallback) {}

    void keyPressEvent(Sdl2Application::KeyEvent& event) {
        switch (event.key()) {
            case Sdl2Application::KeyEvent::Key::NumAdd:
                _instantZoom(1);
                break;
            case Sdl2Application::KeyEvent::Key::NumSubtract:
                _instantZoom(-1);
                break;
            default:
                break;
        }
    }

    void snapshotZoom(const Magnum::Range2D &zoomRect) {
        _defineZoomAccuracy(ZoomAccuracy::Snapshot);

        // setup animation
        ScrollAnimStateComponent from, to;

        auto scalingFactor = 1.f / abs(zoomRect.sizeY());
        from.scaling = mainMatrix()->scaling();
        to.scaling = from.scaling * scalingFactor;
        _thresholdScaling(to.scaling);

        // translate to rectangle center point
        to.translation = zoomRect.center() * Magnum::Vector2 {-1.f, -1.f};

        // reset / play animation
        _updateAnimationAndPlay(from, to);
    }

    void mouseScrollEvent(Sdl2Application::MouseScrollEvent& event, const Magnum::Vector2i& framebufferSize) {
        // if no y offset, no need to handle
        auto direction = event.offset().y();
        if (!direction) return;

        // setup animation
        ScrollAnimStateComponent from, to;

        // determine scaling
        from.scaling = mainMatrix()->scaling();
        to.scaling = _scalingVectorFromDirection(direction);

        // define transform only on zoom-in
        if (direction > 0) {
            _defineZoomAccuracy(ZoomAccuracy::Scroll);
            to.translation = MouseMovements::translatedVector(event, framebufferSize);
        }

        // accept event
        event.setAccepted();

        // reset / play animation
        _updateAnimationAndPlay(from, to);
    }

 private:
    static constexpr float DELTA_BY_OFFLINE = 2.f;
    static constexpr float MAX_SCALE = 32.f;
    static constexpr float MIN_SCALE = .03125f;
    static constexpr float SMOOTHING_AS_SECONDS = .2f;

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Animation::State<ScrollAnimStateComponent>& state) {
        //
        if(!state.to.translation.isZero()) {
            state.previous.translation = state.current.translation;
            state.current.translation = Magnum::Math::lerp(
                state.from.translation,
                state.to.translation,
                Magnum::Animation::Easing::exponentialOut(prc)
            );
        }

        //
        state.current.scaling = Magnum::Math::lerp(
            state.from.scaling,
            state.to.scaling,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    Magnum::Vector2 _getScrollDeltaVector(Magnum::Float direction) {
        auto delta = direction < 0 ? 1.f / DELTA_BY_OFFLINE : DELTA_BY_OFFLINE;
        return Magnum::Vector2{delta};
    }

    void _thresholdScaling(Magnum::Vector2 &targetScale) {
        if(targetScale.x() < MIN_SCALE) {
            targetScale = Magnum::Vector2{MIN_SCALE};
        } else if(targetScale.x() > MAX_SCALE) {
            targetScale = Magnum::Vector2{MAX_SCALE};
        }
    }

    Magnum::Vector2 _scalingVectorFromDirection(Magnum::Float direction, bool asFactor = false) {
        const auto fromScaling = mainMatrix()->scaling();
        auto targetScale = fromScaling * _getScrollDeltaVector(direction);

        _thresholdScaling(targetScale);

        return asFactor ? targetScale / fromScaling : targetScale;
    }

    void _instantZoom(int direction) {
        // stop any anims
        stopAnim();

        // get and apply factor without anim
        const auto scaleFactor = _scalingVectorFromDirection(direction, true);
        _multiplyWithMainMatrix(Magnum::Matrix3::scaling(scaleFactor));
    }

    void _onAnimationProgress() final {
        // scaling
        auto currentAnimSc = currentAnim().scaling;
        if (!currentAnimSc.isZero()) {
            auto scalingFactor = currentAnimSc / mainMatrix()->scaling();
            _multiplyWithMainMatrix(Magnum::Matrix3::scaling(scalingFactor));
        }

        // translate
        auto &ca_Tr = currentAnim().translation;
        if (!ca_Tr.isZero()) {
            // compute step
            auto step = ca_Tr - animState().previous.translation;
            step /= animState().from.scaling;

            // move
            _multiplyWithMainMatrix(Magnum::Matrix3::translation(step * currentAnimSc * _zoomAccuracy));
        }
    }

    //////////
    // ZOOM //
    //////////

    enum ZoomAccuracy {
        Scroll,
        Snapshot
    };
    static constexpr float getZoomAccuracy(const ZoomAccuracy &accuracyType) {
        switch (accuracyType) {
            case Scroll:
                return .5f;
                break;
            default:
                return 1.f;
                break;
        }
    }
    float _zoomAccuracy = getZoomAccuracy(ZoomAccuracy::Scroll);
    void _defineZoomAccuracy(const ZoomAccuracy &accuracyType) { _zoomAccuracy = getZoomAccuracy(accuracyType);}
};

}  // namespace Navigation

}  // namespace UnderStory
