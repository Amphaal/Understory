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

#include <functional>

#include <rxcpp/rx.hpp>

namespace UnderStory {

namespace UI {

class FrameTracker {
 public:
    FrameTracker() {}

    void start(std::function<void(int)> onFPSRead) {
        _cb = onFPSRead;

        rxcpp::observable<>::interval(std::chrono::milliseconds(_updateFPSCountEveryMs))
            .subscribe_on(rxcpp::observe_on_new_thread())
            .subscribe([&](int) {
                this->_updateFPSCount = true;
            });
    }

    void recordFrame() {
        this->_start = std::chrono::steady_clock::now();
    }

    void endRecord() {
        this->_end = std::chrono::steady_clock::now();
        this->_frameRenderDur_Ms = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count()) / 1000;
        this->_fpsEstimated = 1000 / this->_frameRenderDur_Ms;

        this->_mayUpdateFPSDisplay();
    }

 private:
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _end;

    bool _updateFPSCount = false;
    double _frameRenderDur_Ms;
    int _fpsEstimated;

    std::function<void(int)> _cb;

    static inline int _updateFPSCountEveryMs = 200;

    void _mayUpdateFPSDisplay() {
        if(!this->_updateFPSCount) return;

        _cb(this->_fpsEstimated);

        this->_updateFPSCount = false;
    }
};

}  // namespace UI

}  // namespace UnderStory
