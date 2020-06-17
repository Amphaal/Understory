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

    glm::vec4 currentRect;

    GridTile() {
        animateOpacity = tweeny::from(currentColor[3])
            .to(destColor[3])
            .during(2000)
            .via(tweeny::easing::linear);

        animateOpacity.onStep([=](float v) {
            this->currentColor[3] = v;
            return v == 1 ? true : false;
        });
    }

    void advance() {
        animateOpacity.step(7);
    }

    glm::vec4 destColor { 0.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 currentColor { 0.0f, 1.0f, 0.0f, 0.0f };

    AnimationState state = Inserting;

    tweeny::tween<float> animateOpacity;
};

}  // namespace UI

}  // namespace UnderStory
