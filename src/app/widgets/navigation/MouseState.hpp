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
    class USApplication;
}

namespace UnderStory {

namespace Navigation {

class MouseState {
 friend class UnderStory::USApplication;

 public:
    using IsDoubleClick = bool;
    using IsLeftPressed = bool;
    using HasLeftDragged = bool;

    //
    explicit MouseState(int msDoubleClickDelay) : _ms_lDoubleClickDelay(msDoubleClickDelay) {}

    IsLeftPressed hasLeftClick() const {
        return _lPressed;
    }

    HasLeftDragged hasLeftDragged() const {
        return _lMoved;
    }

 protected:
    // returns if considered as a double click
    IsDoubleClick _leftPressed() {
        //
        _lPressed = true;

        //
        auto now = std::chrono::system_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lPressedT);
        _lPressedT = now;
        _lMoved = false;

        //
        _lDblClick = elapsedMs.count() < _ms_lDoubleClickDelay;
        return _lDblClick;
    }

    HasLeftDragged _leftReleased() {
        _lDblClick = false;
        _lPressed = false;
        return _lMoved;
    }

    void _rightPressed() {
        _rPressed = true;
    }

    void _rightReleased() {
        _rPressed = false;
    }

    // when mouse is dragging
    IsLeftPressed _dragging() {
        auto lp = _lPressed;
        if(lp) _lMoved = true;
        return lp;
    }

 private:
    bool _rPressed = false;
    
    bool _lPressed = false;
    bool _lMoved = false;

    bool _lDblClick = false;
    int _ms_lDoubleClickDelay = 0;
    std::chrono::system_clock::time_point _lPressedT;
};

}  // namespace Navigation

}  // namespace UnderStory
