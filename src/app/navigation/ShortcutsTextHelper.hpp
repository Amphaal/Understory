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
#include <Magnum/Math/Color.h>

#include "base/BaseUIPlayerHelper.hpp"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Navigation {

using namespace Magnum::Math::Literals;

struct STHStateComponent {
    Magnum::Vector2 scaling {1.f};
    float textOpacity = .5f;
    float haulerOpacity = .0f;
};

class ShortcutsTextHelper : public BaseUIPlayerHelper<STHStateComponent> {
 public:
     ShortcutsTextHelper(Magnum::Timeline* timeline, Magnum::Matrix3* mainMatrix) : BaseUIPlayerHelper(timeline, mainMatrix, .2f, &_defaultAnimationCallback) {
         _updateColors();
     }

    void mouseMoveEvent(const Sdl2Application::MouseMoveEvent& event, const Magnum::Platform::Application* app, const Magnum::Range2D &hoverRect) {
        //
        auto normalizedPoint = _trNormalized(event.position(), app->framebufferSize());
        auto isCursorInRect = hoverRect.contains(normalizedPoint);
        auto mustRenewAnim = isCursorInRect != _isCursorInRect;
        _isCursorInRect = isCursorInRect;

        //
        if(mustRenewAnim) _renewAnim();
    }

    static Magnum::Range2D trNormalized(const Magnum::Range2D& rect, const Magnum::Vector2i& framebufferSize) {
        return { rect.min() / Magnum::Vector2{framebufferSize}, rect.max() };
    }

    const Magnum::Color4& textColor() const {
        return _textColor;
    }

    const Magnum::Color4& haulderColor() const {
        return _haulderColor;
    }

 private:
    Magnum::Color4 _haulderColor;
    Magnum::Color4 _textColor;
    bool _isCursorInRect = false;

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, AnimationState<STHStateComponent>& state) {
        //
        state.current.scaling = Magnum::Math::lerp(
            state.from.scaling,
            state.to.scaling,
            Magnum::Animation::Easing::cubicOut(prc)
        );

        //
        state.current.haulerOpacity = Magnum::Math::lerp(
            state.from.haulerOpacity,
            state.to.haulerOpacity,
            Magnum::Animation::Easing::cubicOut(prc)
        );

        //
        state.current.textOpacity = Magnum::Math::lerp(
            state.from.textOpacity,
            state.to.textOpacity,
            Magnum::Animation::Easing::cubicOut(prc)
        );
    }

    void _renewAnim() {
        STHStateComponent to;

        if(_isCursorInRect) {
            to.scaling = Magnum::Vector2{2.f};
            to.haulerOpacity = .85f;
            to.textOpacity = 1.f;
        }

        _updateAnimationAndPlay(currentAnim(), to);
    }

    void _mayUpdateMainMatrix() final {
        _replaceMainMatrix(Magnum::Matrix3::scaling(currentAnim().scaling));
        _updateColors();
    }

    void _updateColors() {
        _haulderColor = {0x00000_rgbf, currentAnim().haulerOpacity};
        _textColor = {0xffffff_rgbf, currentAnim().textOpacity};
    }

    static Magnum::Vector2 _trNormalized(const Magnum::Vector2i& moveCoords, const Magnum::Vector2i& framebufferSize) {
        auto originTopRight = Magnum::Vector2 {
            static_cast<float>(framebufferSize.x()) - moveCoords.x(),
            static_cast<float>(moveCoords.y())
        };
        return Magnum::Vector2 {
            originTopRight.x() / framebufferSize.x(),
            originTopRight.y() / framebufferSize.y()
        } * Magnum::Vector2 {-1.f};
    }
};

}  // namespace Navigation

}  // namespace UnderStory
