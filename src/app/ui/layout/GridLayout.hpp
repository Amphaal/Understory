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
#include <memory>

#include "src/app/Utility.hpp"

#include "GridTile.hpp"

namespace UnderStory {

namespace UI {

class GridLayout {
 public:
    using DrawCallback = std::function<void(std::unique_ptr<UnderStory::UI::GridTile> &tile)>;
    enum Direction {
        LeftToRight,
        TopToBottom
    };

    GridLayout() {}

    void addTile() {
        _tiles.emplace_back(std::make_unique<GridTile>());
    }

    void progressStep() {
        for(auto &tile : _tiles) {
            tile->advance();
        }
    }

    void changeColor() {
        for(auto &tile : _tiles) {
            tile->animateColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        }
    }

    void addTiles(int howMany) {
        while(howMany) {
            addTile();
            howMany--;
        }
    }

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

            tile->currentRect = glm::vec4(
                *row,                    // p1x
                *column,                 // p1y
                *row + _squareSize,      // p2x
                *column + _squareSize    // p2y
            );

            _onTileDrawing(tile);

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
    std::vector<std::unique_ptr<GridTile>> _tiles;

    bool _animStarted = false;
    rxcpp::composite_subscription _animHandler;
};

}  // namespace UI

}  // namespace UnderStory
