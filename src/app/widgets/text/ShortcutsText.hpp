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

#include <utility>

#include "../animation/BaseUIPlayerHelper.hpp"
#include "../base/Hoverable.hpp"
#include "StaticText.hpp"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Widget {

using namespace Magnum::Math::Literals;

struct STHStateComponent {
    Magnum::Vector2 scaling {1.f};
    float textOpacity = .5f;
    float haulerOpacity = .0f;
};

class ShortcutsText : public Animation::BaseUIPlayerHelper<STHStateComponent>, public Hoverable<> {
 public:
    ShortcutsText(StaticText&& associatedText, Magnum::Timeline* timeline, Magnum::Shaders::DistanceFieldVector2D* shader, Magnum::Platform::Application* app) :
        BaseUIPlayerHelper(timeline, &_moveAnim, .2f, &_defaultAnimationCallback),
        Hoverable(app),
        _text(std::move(associatedText)),
        _shader(shader) {
         _updateColors();
    }

    void onViewportChange(const Constraints &wh) final {
        // stick to top right corner + 5 pixels padding
        _responsiveMatrix = wh.baseProjMatrix *
            Magnum::Matrix3::translation(
                wh.ws *
                (Magnum::Vector2 {.5f} - wh.pixelSize * 5)
            );

        // update geometry
        Hoverable::onViewportChange(wh);
    }

    void draw() {
        _shader
            ->setColor(_textColor)
            .setOutlineColor(0xffffffAA_rgbaf)
            .setTransformationProjectionMatrix(_matrix)
            .draw(_text);
    }

    const Magnum::Color4& enlighterColor() const {
        return _enlighterColor;
    }

 private:
    Magnum::Color4 _enlighterColor;
    Magnum::Color4 _textColor;

    Magnum::Matrix3 _matrix;
    Magnum::Matrix3 _moveAnim;
    Magnum::Matrix3 _responsiveMatrix;

    StaticText _text;
    Magnum::Shaders::DistanceFieldVector2D* _shader;

    static void _defaultAnimationCallback(Magnum::Float /*t*/, const float &prc, Animation::State<STHStateComponent>& state) {
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

    void _updateGeometry() final {
        _matrix = _responsiveMatrix * _moveAnim;
        _geometry = _text.geometryFromMatrix(_matrix);
    }

    void _onHoverChanged(bool isHovered) final {
        STHStateComponent to;

        if(isHovered) {
            to.scaling = Magnum::Vector2{2.f};
            to.haulerOpacity = .85f;
            to.textOpacity = 1.f;
        }

        _updateAnimationAndPlay(currentAnim(), to);
    }

    void _onAnimationProgress() final {
        _replaceMainMatrix(Magnum::Matrix3::scaling(currentAnim().scaling));
        _updateColors();
        _updateGeometry();
    }

    void _updateColors() {
        _enlighterColor = {0x00000_rgbf, currentAnim().haulerOpacity};
        _textColor = {0xffffff_rgbf, currentAnim().textOpacity};
    }
};

}  // namespace Widget

}  // namespace UnderStory