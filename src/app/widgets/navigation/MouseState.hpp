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

#include <chrono>

#include <spdlog/spdlog.h>

namespace UnderStory { namespace Widget { class AppContainer; }}

namespace UnderStory {

namespace Navigation {

class MouseState {
 friend class Widget::AppContainer;

 public:
    MouseState() {}

    bool hasLeftDoubleClick() const {
        return _lDblClick;
    }

    bool isLeftClickDragging() const {
        return _lMoved;
    }

 protected:
    // returns if considered as a double click
    void _leftPressed() {
        //
        _lPressed = true;
        // spdlog::info("Left [Pressed]");

        //
        auto now = std::chrono::system_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lPressedT);
        _lPressedT = now;
        _lMoved = false;

        // define if is double click
        _lDblClick = elapsedMs.count() < DOUBLE_CLICK_DELAY_MS;
    }

    void _leftReleased() {
        // spdlog::info("Left [Released]");
        _lDblClick = false;
        _lPressed = false;
        _lMoved = false;
    }

    void _rightPressed() {
        // spdlog::info("Right [Pressed]");
        _rPressed = true;
    }

    void _rightReleased() {
        // spdlog::info("Right [Released]");
        _rPressed = false;
    }

    void _mayBeDragging() {
        if(_lPressed && !_lMoved) 
            _lMoved = true;
    }

 private:
    static constexpr int DOUBLE_CLICK_DELAY_MS = 200;
    bool _rPressed = false;
    
    bool _lPressed = false;
    bool _lMoved = false;

    bool _lDblClick = false;
    std::chrono::system_clock::time_point _lPressedT;
};

}  // namespace Navigation

}  // namespace UnderStory
