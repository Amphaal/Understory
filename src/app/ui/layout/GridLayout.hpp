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

#include <spdlog/spdlog.h>

#include <functional>
#include <map>
#include <set>
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
        for(auto & [k, tile] : _tiles) {
            tile->advance();
        }

        // delete
        if(!_tbrTiles.empty()) {
            for(const auto &key : _tbrTiles) {
                _tiles.erase(key);
            }
            _tbrTiles.clear();
        }
    }

    void draw() {
        for(auto & [k, tile] : _tiles) {
            _onTileDrawing(tile);
        }
    }

    void changeColor() {
        for(auto & [k, tile] : _tiles) {
            tile->animateColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        }
    }

    void removeTiles(int howMany) {
        if(howMany <= 0) return;

        for(auto i = _tiles.begin(); i != _tiles.end(); i++) {
            auto &tile = i->second;
            if(tile->beingRemoved) continue;

            tile->animateRemoval([=]() {
                _tbrTiles.insert(i->first);
            });
            howMany--;

            if(!howMany) break;
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
    int _padding = 20;
    Direction _direction = LeftToRight;

    DrawCallback _onTileDrawing;
    std::map<int, std::unique_ptr<GridTile>> _tiles;
    int _tilesIndex = 0;
    std::set<int> _tbrTiles;

    const UnderStory::Utility::Size* _constraints = nullptr;
    const glm::vec2* _pointerPos = nullptr;

    void _addTile() {
        _tiles.emplace(
            _tilesIndex,
            std::make_unique<GridTile>()
        );
        _tilesIndex++;
    }

    int _findColumnFirstBorder(int constraint) {
        auto x = 0;
        auto size = 0;
        int returnVal = 0;
        bool mustBreak = false;

        while(true) {
            size = _squareSize * x + (x + 1) * _padding;

            if(mustBreak) {
                auto borderBothSides = constraint - size;
                returnVal = borderBothSides ? borderBothSides / 2 : borderBothSides;
                break;
            }

            if(constraint == size) {  // if size is exactly the constraint, no border needed
                break;
            } else if(constraint < size) {  // if size is too big, step down 1 tile and return result
                x--;

                // if previous step is no tile, no border needed
                if(x == 0) break;

                // request another trip, but force break
                mustBreak = true;
                continue;
            }

            x++;
        }

        return returnVal;
    }

    void _updateState() {
        // checks
        if(!_onTileDrawing) return;
        if(!_tiles.size()) return;

        // prepare
        const int* constraint = nullptr;
        int* row = nullptr;
        int* column = nullptr;
        int x = 0;
        int y = 0;

        // bind depending on direction
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

        // prepare new line handler
        auto fcb = _findColumnFirstBorder(*constraint);
        auto newLine = [=](bool firstLine = true) {
            *row = firstLine ? fcb : fcb + _padding;
            *column += firstLine ? _padding : _padding + _squareSize;
        };

        // define start values
        newLine();

        // iterate
        auto atLeastOneTile = false;
        for (auto & [k, tile] : _tiles) {
            *row += _padding;

            // check if must be new lined
            if(*row + _squareSize > *constraint && atLeastOneTile) {
                newLine(false);
            }

            if(tile->beingRemoved) continue;

            tile->animateRect({
                *row,                    // p1x
                *column,                 // p1y
                *row + _squareSize,      // p2x
                *column + _squareSize    // p2y
            });

            atLeastOneTile = true;

            *row += _squareSize;
        }
    }
};

}  // namespace UI

}  // namespace UnderStory
