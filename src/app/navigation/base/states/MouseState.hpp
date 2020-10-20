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

namespace UnderStory {

namespace Navigation {

struct MouseState {
 public:
    using IsDoubleClick = bool;
    using IsLeftPressed = bool;
    using HasLeftDragged = bool;

    //
    explicit MouseState(int msDoubleClickDelay) : _msDoubleClickDelay(msDoubleClickDelay) {}

    // returns if considered as a double click
    IsDoubleClick leftPressed() {
        //
        _lPressed = true;

        //
        auto now = std::chrono::system_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lPressedT);
        _lPressedT = now;
        _lMoved = false;

        //
        return elapsedMs.count() < _msDoubleClickDelay;
    }

    HasLeftDragged leftReleased() {
        _lPressed = false;
        return _lMoved;
    }

    void rightPressed() {
        _rPressed = true;
    }

    void rightReleased() {
        _rPressed = false;
    }

    // when mouse is dragging
    IsLeftPressed dragging() {
        auto lp = _lPressed;
        if(lp) _lMoved = true;
        return lp;
    }

 private:
    int _msDoubleClickDelay = 0;
    bool _lPressed = false;
    bool _rPressed = false;
    bool _lMoved = false;
    std::chrono::system_clock::time_point _lPressedT;
};

}  // namespace Navigation

}  // namespace UnderStory
