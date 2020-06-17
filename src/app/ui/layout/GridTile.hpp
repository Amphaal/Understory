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
    enum AnimationState {
        Idle,
        Moving,
        Hovered,
        Inserting,
        Removing
    };

    GridTile() {
        animateColor({ 0.0f, 1.0f, 0.0f, 1.0f });
    }

    void advance() {
        auto step = 7;  // TODO(amphaal) refresh rate sync ?
        _animateColor.step(step);
        _animateRect.step(step);
    }

    void animateColor(glm::vec4 to) {
        _destColor = to;

        _animateColor =
        tweeny::from(currentColor[0], currentColor[1], currentColor[2], currentColor[3])
               .to(_destColor[0], _destColor[1], _destColor[2], _destColor[3])
               .during(1000)
               .via(tweeny::easing::linear);

        auto cb = [=](float r, float g, float b, float a) {
            currentColor[0] = r;
            currentColor[1] = g;
            currentColor[2] = b;
            currentColor[3] = a;
            return false;
        };

        _animateColor.onStep(cb);
    }

    void animateRect(glm::vec4 to) {
        if(to == _destRect) return;

        _destRect = to;

        _animateRect = tweeny::from(currentRect[0], currentRect[1], currentRect[2], currentRect[3])
               .to(_destRect[0], _destRect[1], _destRect[2], _destRect[3])
               .during(1000)
               .via(tweeny::easing::linear);

        auto cb = [=](float x1, float y1, float x2, float y2) {
            currentRect[0] = x1;
            currentRect[1] = y1;
            currentRect[2] = x2;
            currentRect[3] = y2;
            return false;
        };

        _animateRect.onStep(cb);
    }

    glm::vec4 currentColor { 0.0f, 1.0f, 0.0f, 0.0f };
    glm::vec4 currentRect  { 0.0f, 0.0f, 0.0f, 0.0f };

 private:
    tweeny::tween<float, float, float, float> _animateColor;
    tweeny::tween<float, float, float, float> _animateRect;
    glm::vec4 _destColor;
    glm::vec4 _destRect;
};

}  // namespace UI

}  // namespace UnderStory
