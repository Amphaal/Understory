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

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <memory>

#include <GLFWM/glfwm.hpp>

#include "src/app/Utility.hpp"

#include "src/app/ui/base/Texture.hpp"
#include "src/app/ui/layout/GridLayout.hpp"

namespace UnderStory {

namespace UI {

class Engine {
 public:
    explicit Engine(const UnderStory::Utility::Size* constraints, const glm::vec2* pointerPos) :
        _constraints(constraints), _assetsGrid(constraints, pointerPos) {}

    bool onScrollEvent(glfwm::EventScroll* event) {
        // TODO(amphaal)
        return true;
    }

    bool onKeyPress(glfwm::EventKey* event) {
        if(event->getAction() != glfwm::ActionType::RELEASE) return true;

        return true;
    }

    void draw() {
        // animate and draw
        _assetsGrid.advance();
        _assetsGrid.draw();
    }

    void onWindowSizeChange() {
        // TODO
    }

 private:
    std::vector<Texture> _textures;

    float _aspectRatio() const {
        return _constraints->wF() / _constraints->hF();
    }

    //
    const UnderStory::Utility::Size* _constraints = nullptr;
    GridLayout _assetsGrid;

    Texture& _useAsTexture(const std::string &path) {
        auto rawImg = UnderStory::Utility::getRawImage(path, true);
        return _textures.emplace_back(rawImg);
    }
};

}  // namespace UI

}  // namespace UnderStory
