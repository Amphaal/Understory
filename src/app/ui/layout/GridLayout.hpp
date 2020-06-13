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
#include <vector>

#include "src/app/Utility.hpp"

#include "GridTile.hpp"

namespace UnderStory {

namespace UI {

class GridLayout {
 public:
    using DrawCallback = std::function<void(int squareSize, int x, int y)>;
    enum Direction {
        LeftToRight,
        TopToBottom
    };

    GridLayout() {}

    void draw(const UnderStory::Utility::Size& constraints) {
        if(!_onTileDrawing) return;
        if(!_tiles.size()) return;

        const int* constraint = nullptr;
        int* row = nullptr;
        int* column = nullptr;
        int x = 0;
        int y = 0;

        switch(_direction) {
            case LeftToRight: {
                constraint = &constraints.width;
                row = &x;
                column = &y;
            }
            break;

            case TopToBottom: {
                constraint = &constraints.height;
                row = &y;
                column = &x;
            }
            break;
        }

        *column = _padding;

        bool rowVirgin = true;
        for (auto &tile : _tiles) {
            *row += _padding;

            if(!rowVirgin) {
                *row += _squareSize;
            }

            if(*row + _squareSize > *constraint && !rowVirgin) {
                *row = _padding;
                *column += _padding + _squareSize;
            }

            _onTileDrawing(_squareSize, x, y);

            rowVirgin = false;
        }
    }

    void setOnTileDrawing(const DrawCallback &cb) {
        _onTileDrawing = cb;
    }

 private:
    DrawCallback _onTileDrawing;
    int _squareSize = 120;
    int _padding = 2;
    Direction _direction = LeftToRight;
    std::vector<GridTile> _tiles;
};

}  // namespace UI

}  // namespace UnderStory
