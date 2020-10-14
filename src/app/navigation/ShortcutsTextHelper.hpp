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

class ShortcutsTextHelper : public BaseUIPlayerHelper<> {
 public:
     ShortcutsTextHelper(Magnum::Timeline* timeline, Magnum::Matrix3* mainMatrix) : BaseUIPlayerHelper(timeline, mainMatrix, .2f) {}

    void mouseMoveEvent(const Sdl2Application::MouseMoveEvent& event, const Magnum::Platform::Application* app, const Magnum::Range2D &hoverRect) {
        //
        auto normalizedPoint = _trNormalized(event.position(), app->framebufferSize());
        _isCursorInRect = hoverRect.contains(normalizedPoint);

        //
        Magnum::Vector2 scaleVec {_isCursorInRect ? 2.f : 1.f};
        auto haulderOpacity = _isCursorInRect ? .75f : .0f;

        //
        _haulderColor = _isCursorInRect ? 0x00000CC_rgbaf : 0x0000000_rgbaf;
        _textColor = _isCursorInRect ? 0xffffff_rgbf : 0xffffff88_rgbaf;
        _replaceMainMatrix(Magnum::Matrix3::scaling(scaleVec));

        // _updateAnimationAndPlay(currentAnim(), scaleVec);
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

    void _mayUpdateMainMatrix() final {
        // _replaceMainMatrix(Magnum::Matrix3::scaling(currentAnim()));
    }

    static Magnum::Vector2 _trNormalized(const Magnum::Vector2i& moveCoords, const Magnum::Vector2i& framebufferSize) {
        auto originTopRight = Magnum::Vector2 {
            static_cast<float>(framebufferSize.x()) - moveCoords.x(),
            static_cast<float>(moveCoords.y())
        };
        return Magnum::Vector2 {originTopRight.x() / framebufferSize.x(), originTopRight.y() / framebufferSize.y()} * Magnum::Vector2 {-1.f};
    }
};

}  // namespace Navigation

}  // namespace UnderStory
