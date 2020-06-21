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

    explicit GridLayout(const UnderStory::Utility::Size* constraints, const glm::vec2* pointerPos) : _constraints(constraints), _pointerPos(pointerPos) {}

    void advance() {
        _updateState();
        for(auto &tile : _tiles) {
            tile->advance();
        }
    }

    void draw() {
        for(auto &tile : _tiles) {
            _onTileDrawing(tile);
        }
    }

    void changeColor() {
        for(auto &tile : _tiles) {
            tile->animateColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        }
    }

    void removeTiles(int howMany) {
        auto i = 0;
        while(howMany) {
            if(i + 1 > _tiles.size()) break;

            auto &tile = _tiles[i];
            if(tile->beingRemoved) {
                i++;
                continue;
            }

            tile->beingRemoved = true;
            tile->animateRect({});
            howMany--;
        }
    }

    void addTiles(int howMany) {
        while(howMany) {
            _addTile();
            howMany--;
        }
    }

    void setOnTileDrawing(const DrawCallback &cb) {
        _onTileDrawing = cb;
    }

 private:
    int _squareSize = 120;
    int _padding = 2;
    Direction _direction = LeftToRight;

    DrawCallback _onTileDrawing;
    std::vector<std::unique_ptr<GridTile>> _tiles;
    const UnderStory::Utility::Size* _constraints = nullptr;
    const glm::vec2* _pointerPos = nullptr;

    void _addTile() {
        _tiles.emplace_back(std::make_unique<GridTile>());
    }

    void _updateState() {
        if(!_onTileDrawing) return;
        if(!_tiles.size()) return;

        const int* constraint = nullptr;
        int* row = nullptr;
        int* column = nullptr;
        int x = 0;
        int y = 0;

        switch(_direction) {
            case LeftToRight: {
                constraint = &_constraints->width;
                row = &x;
                column = &y;
            }
            break;

            case TopToBottom: {
                constraint = &_constraints->height;
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

            rowVirgin = false;

            if(tile->beingRemoved) continue;

            tile->animateRect({
                *row,                    // p1x
                *column,                 // p1y
                *row + _squareSize,      // p2x
                *column + _squareSize    // p2y
            });
        }
    }
};

}  // namespace UI

}  // namespace UnderStory
