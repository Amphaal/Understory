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

#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/ArrayView.h>

#include <Magnum/Math/Range.h>

#include <functional>

#include "SelectionRect.hpp"

namespace UnderStory {

namespace Navigation {

struct SelectionRectState {
 public:
    SelectionRectState() {}
    SelectionRectState(Shader::SelectionRect* shader, Magnum::GL::Buffer* buffer) : _associatedShader(shader), _associatedBuffer(buffer) {}

    const Corrade::Containers::StaticArray<4, Shader::SelectionRect::Vertex>& vertexes() const {
        return _vertexes;
    }

    Magnum::Math::Range2D<float> asRectangle() const {
        return { _vertexes[0].position, _vertexes[2].position };
    }

    bool isSelecting() const {
        return _isSelecting;
    }

    void init(const Sdl2Application::MouseEvent& event, const Magnum::Vector2i& framebufferSize) {
        // vert
        _p1() = _trVert(event.position(), framebufferSize);
        _p2() = _p1();

        // frag
        _p1_px = _trFrag(event.position(), framebufferSize);
        _p2_px = _p1_px;

        //
        _updateShaderData();

        // define as selecting
        _isSelecting = true;
    }

    void update(const Sdl2Application::MouseMoveEvent& event, const Magnum::Vector2i& framebufferSize) {
        _updatefromPoint(event.position(), framebufferSize);
    }

    void end(const Sdl2Application::MouseEvent& event, const Magnum::Vector2i& framebufferSize) {
        _updatefromPoint(event.position(), framebufferSize);
        _isSelecting = false;
    }

    void onViewportChange() {
        _isSelecting = false;  // reset selection
    }

 private:
    static constexpr int BORDER_PX_WIDTH = 1;

    template<class T>
    static Magnum::Range2D _getStandardizedRect(const Magnum::Math::Vector2<T> &p1, const Magnum::Math::Vector2<T> &p2) {
        // find topLeft and bottomRight corners
        Magnum::Vector2 topLeft;
        Magnum::Vector2 bottomRight;

        // distrib function
        auto distrib = [&topLeft, &bottomRight](T val1, T val2, std::function<Magnum::Vector2(Magnum::Float)> func) {
            if(val1 < val2) {
                topLeft += func(static_cast<float>(val1));
                bottomRight += func(static_cast<float>(val2));
            } else {
                topLeft += func(static_cast<float>(val2));
                bottomRight += func(static_cast<float>(val1));
            }
        };

        // distribute
        distrib(p1.x(), p2.x(), Magnum::Vector2::xAxis);
        distrib(p1.y(), p2.y(), Magnum::Vector2::yAxis);

        // return
        return { topLeft, bottomRight };
    }

    static Magnum::Vector4 _padded(const Magnum::Range2D &rect, Magnum::Float padding) {
        Magnum::Vector4 out;

        // x
        {
            auto xPadded = rect.x().padded(padding);
            if(xPadded.min() > xPadded.max()) {
                out[0] = rect.centerX();
                out[2] = out[0];
            } else {
                out[0] = xPadded.min();
                out[2] = xPadded.max();
            }
        }

        // y
        {
            auto yPadded = rect.y().padded(padding);
            if(yPadded.min() > yPadded.max()) {
                out[1] = rect.centerY();
                out[3] = out[1];
            } else {
                out[1] = yPadded.min();
                out[3] = yPadded.max();
            }
        }

        return out;
    }

    // fragment shader coordinates
    static Magnum::Vector2i _trFrag(const Magnum::Vector2i& moveCoords, const Magnum::Vector2i& framebufferSize) {
        return {moveCoords.x(), framebufferSize.y() - moveCoords.y()};
    }

    // vertex shader coordinates
    static Magnum::Vector2 _trVert(const Magnum::Vector2i& moveCoords, const Magnum::Vector2i& framebufferSize) {
        auto toCoordsGL = (moveCoords - (framebufferSize / 2)) / Magnum::Vector2i {1, -1};
        return Magnum::Vector2 {toCoordsGL} / Magnum::Vector2 {static_cast<float>(framebufferSize.y())};
    }

    //
    Corrade::Containers::StaticArray<4, Shader::SelectionRect::Vertex> _vertexes;
    Shader::SelectionRect* _associatedShader = nullptr;
    Magnum::GL::Buffer* _associatedBuffer = nullptr;

    //
    Magnum::Vector2& _p1() { return _vertexes[0].position; }
    Magnum::Vector2& _p2() { return _vertexes[2].position; }
    Magnum::Vector2i _p1_px, _p2_px;
    bool _isSelecting = false;

    void _updatefromPoint(const Magnum::Vector2i& point, const Magnum::Vector2i& framebufferSize) {
        // set new coordinates
        _p2() =  _trVert(point, framebufferSize);
        _p2_px = _trFrag(point, framebufferSize);

        // update intermediate
        _updateShaderData();
    }

    void _updateShaderData() {
        // update intermediate vertexes points
        _vertexes[1] = {{ _vertexes[2].position.x(), _vertexes[0].position.y() }};
        _vertexes[3] = {{ _vertexes[0].position.x(), _vertexes[2].position.y() }};

        // update buffer data
        _associatedBuffer->setSubData(0, _vertexes);

        // define inner rect
        auto innerRect = _padded(
            _getStandardizedRect(_p1_px, _p2_px),
            -BORDER_PX_WIDTH
        );
        _associatedShader->setInnerRect(innerRect);
    }
};

}  // namespace Navigation

}  // namespace UnderStory
