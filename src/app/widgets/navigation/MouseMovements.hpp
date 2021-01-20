// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020-2021 Guillaume Vara <guillaume.vara@gmail.com>

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

#include <deque>
#include <utility>
#include <chrono>

using Magnum::Platform::Sdl2Application;

namespace UnderStory {

namespace Navigation {

class MouseMovements {
 public:
    static Magnum::Vector2 translatedVector(const Sdl2Application::MouseMoveEvent& event, const Magnum::Vector2i& framebufferSize) {
        return _translatedVector(event.relativePosition(), framebufferSize);
    }

    static Magnum::Vector2 translatedVector(Sdl2Application::MouseScrollEvent& event, const Magnum::Vector2i& framebufferSize) {
        return _translatedVectorAbs(event.position(), framebufferSize);
    }

    static Magnum::Vector2 translatedVector(const Sdl2Application::MouseEvent& event, const Magnum::Vector2i& framebufferSize) {
        return _translatedVectorAbs(event.position(), framebufferSize);
    }

    void addToAccelerationBuffer(const Magnum::Vector2& tr) {
        // update timestamps
        _updateAccelerationTime();

        // update state
        _accelCumulator += tr;
        _accelBufferCount++;
        _accelBuffer.push_back({_accelMsPassedLast, tr});

        // try to reduce buffer
        _mayReduceAccelerationBuffer();
    }

    bool isComputing() const {
        return _accelMsPassedTotal;
    }

    Magnum::Vector2 computeAccelerationBuffer() {
        // update state
        _updateAccelerationTime();
        _mayReduceAccelerationBuffer();

        // define output
        auto out = _getAccelerationTranslation();

        // reset
        resetAccelerationBuffer();

        // returns output
        return out;
    }

    void resetAccelerationBuffer() {
        _accelBuffer.clear();
        _accelCumulator = Magnum::Vector2();
        _accelBufferCount = 0;
        _accelMsPassedTotal = 0;
        _accelMs = 0;
        _accelMsPassedLast = 0;
        _timeCredits = 0;
    }

 private:
    std::deque<std::pair<int, Magnum::Vector2>> _accelBuffer;
    std::chrono::system_clock::time_point _accelLatestUpdate;
    Magnum::Vector2 _accelCumulator;
    int _accelMs = 0;
    int _accelBufferCount = 0;
    int _accelMsPassedTotal = 0;
    int _accelMsPassedLast = 0;
    int _timeCredits = 0;

    static inline int ACCELERATION_THRESHOLD_MS = 200;

    void _updateAccelerationTime() {
        auto now = std::chrono::system_clock::now();

        // if not first insert, compare with previous
        if(_accelBufferCount) {
            _accelMsPassedLast = std::chrono::duration_cast<std::chrono::milliseconds>(now - _accelLatestUpdate).count();
            _accelMsPassedTotal += _accelMsPassedLast;
            _accelMs += _accelMsPassedLast;
        }

        // update state
        _accelLatestUpdate = now;
    }

    Magnum::Vector2 _getAccelerationTranslation() const {
        // check buffer size
        auto bufSize = _accelBuffer.size();
        if(!bufSize || !_accelMs) return Magnum::Vector2();

        // define sample strengh
        auto sampleStrength = static_cast<float>(bufSize) / 100;
        if(sampleStrength > 1) sampleStrength = 1;

        // define acceleration factor (maximum sampling time / cumulated sampled time)
        auto accelerationFactor = sqrt(static_cast<float>(ACCELERATION_THRESHOLD_MS) / _accelMs);

        // define output
        auto definitiveAccel = accelerationFactor * sampleStrength;
        return _accelCumulator * Magnum::Vector2{definitiveAccel};
    }

    void _mayReduceAccelerationBuffer() {
        // if buffer threshold reached
        if(_accelMsPassedTotal > ACCELERATION_THRESHOLD_MS) {
            // add current to credits
            _timeCredits += _accelMsPassedLast;

            // remove values from cumulator
            while (!_accelBuffer.empty()) {
                // if toBePoped timestamp exeeds timeCredits, wait for more inputs
                auto &[t, vec] = _accelBuffer.front();
                if (t > _timeCredits) break;

                // else, remove from buffer and update
                _timeCredits -= t;
                _accelMs -= t;
                _accelCumulator -= vec;
                _accelBuffer.pop_front();
            }
        }
    }

    static Magnum::Vector2 _translatedVector(const Magnum::Vector2i& pixelCoords, const Magnum::Vector2i& framebufferSize) {
        return Magnum::Vector2 {pixelCoords} / Magnum::Vector2 {static_cast<float>(framebufferSize.y())} / Magnum::Vector2 {1.f, -1.f};
    }

    static Magnum::Vector2 _translatedVectorAbs(const Magnum::Vector2i& moveCoords, const Magnum::Vector2i& framebufferSize) {
        auto toCoordsGL = (moveCoords - (framebufferSize / 2)) / Magnum::Vector2i {-1, 1};
        return Magnum::Vector2 {toCoordsGL} / Magnum::Vector2 {static_cast<float>(framebufferSize.y())};
    }
};

}  // namespace Navigation

}  // namespace UnderStory
