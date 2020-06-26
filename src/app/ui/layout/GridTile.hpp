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

#include <tweeny.h>

#include <glm/glm.hpp>

namespace UnderStory {

namespace UI {

class GridTile {
 public:
    GridTile() {
        animateColor({ 0.0f, 1.0f, 0.0f, 1.0f });
    }

    void advance() {
        auto step = 7;  // TODO(amphaal) refresh rate sync ?
        _animateColorTw.step(step);
        _animateRectTw.step(step);
    }

    void setHovered(bool isHovered) {
        _isHovered = isHovered;
        _animateColor(_isHovered ? _hoverColor : _unhoverColor);
    }

    void animateColor(glm::vec4 to) {
        _unhoverColor = to;
        return _animateColor(to);
    }

    bool isPointInTile(glm::vec2 point) {
        auto xOk = point[0] >= _destRect[0] && point[0] <= _destRect[2];
        auto yOk = point[1] >= _destRect[1] && point[1] <= _destRect[3];
        return xOk && yOk;
    }

    void animateRemoval(std::function<void(void)> onRemoval) {
        _destRect = { };
        this->beingRemoved = true;

        _animateRectTw =
        tweeny::from(currentRect[0], currentRect[1], currentRect[2], currentRect[3])
               .to(_destRect[0], _destRect[1], _destRect[2], _destRect[3])
               .during(200)
               .via(tweeny::easing::cubicIn);

        auto cb = [=](tweeny::tween<float, float, float, float> & t, float x1, float y1, float x2, float y2) {
            currentRect[0] = x1;
            currentRect[1] = y1;
            currentRect[2] = x2;
            currentRect[3] = y2;

            if(t.progress() == 1.0f) {
                onRemoval();
            }

            return false;
        };

        _animateRectTw.onStep(cb);
    }

    void animateRect(glm::vec4 to) {
        if(to == _destRect) return;

        _destRect = to;

        _animateRectTw =
        tweeny::from(currentRect[0], currentRect[1], currentRect[2], currentRect[3])
               .to(_destRect[0], _destRect[1], _destRect[2], _destRect[3])
               .during(200)
               .via(tweeny::easing::cubicIn);

        auto cb = [=](tweeny::tween<float, float, float, float> & t, float x1, float y1, float x2, float y2) {
            currentRect[0] = x1;
            currentRect[1] = y1;
            currentRect[2] = x2;
            currentRect[3] = y2;
            if(t.progress() == 1.0f) return true;
            return false;
        };

        _animateRectTw.onStep(cb);
    }

    glm::vec4 currentColor { 0.0f, 1.0f, 0.0f, 0.0f };
    glm::vec4 currentRect  { 0.0f, 0.0f, 0.0f, 0.0f };

    bool beingRemoved = false;

 private:
    tweeny::tween<float, float, float, float> _animateColorTw;
    tweeny::tween<float, float, float, float> _animateRectTw;
    glm::vec4 _destColor;
    glm::vec4 _hoverColor { 1.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 _unhoverColor;
    glm::vec4 _destRect;

    bool _isHovered = false;

    void _animateColor(glm::vec4 to) {
        if(to == _destColor) return;

        _destColor = to;

        _animateColorTw =
        tweeny::from(currentColor[0], currentColor[1], currentColor[2], currentColor[3])
               .to(_destColor[0], _destColor[1], _destColor[2], _destColor[3])
               .during(300)
               .via(tweeny::easing::linear);

        auto cb = [=](tweeny::tween<float, float, float, float> & t, float r, float g, float b, float a) {
            currentColor[0] = r;
            currentColor[1] = g;
            currentColor[2] = b;
            currentColor[3] = a;
            if(t.progress() == 1.0f) return true;
            return false;
        };

        _animateColorTw.onStep(cb);
    }
};

}  // namespace UI

}  // namespace UnderStory
