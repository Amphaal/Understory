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

#include <Magnum/Text/Renderer.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Mesh.h>
#include <Magnum/GL/Mesh.h>

#include <utility>
#include <string>

namespace UnderStory {

namespace Widget {

class StaticText : public Magnum::GL::Mesh {
 public:
    StaticText() {}
    StaticText(Magnum::GL::Mesh &&mesh, Magnum::GL::Buffer &&indices, Magnum::GL::Buffer &&vertices, Magnum::Range2D &&geometry) :
        Magnum::GL::Mesh(std::move(mesh)),
        _indices(std::move(indices)),
        _vertices(std::move(vertices)),
        _geometry(std::move(geometry)) {}

    Magnum::Range2D geometryFromMatrix(const Magnum::Matrix3 &matrix) const {
        return {
            matrix.transformPoint(_geometry.min()),
            matrix.transformPoint(_geometry.max())
        };
    }

 private:
    Magnum::GL::Buffer _indices;
    Magnum::GL::Buffer _vertices;
    Magnum::Range2D    _geometry;
};

class StaticTextFactory {
 public:
    StaticTextFactory(Magnum::Text::AbstractFont &font, const Magnum::Text::GlyphCache &cache, Magnum::Float fontSize) :
    _font(&font), _cache(&cache), _fontSize(fontSize) {}

    StaticText generate(const std::string &text, Magnum::Text::Alignment alignment = Magnum::Text::Alignment::MiddleCenter) {
        //
        Magnum::GL::Mesh mesh;
        Magnum::GL::Buffer indices;
        Magnum::GL::Buffer vertices;
        Magnum::Range2D geometry;

        //
        std::tie(mesh, geometry) = Magnum::Text::Renderer2D::render(
            *_font, *_cache,  // font + cache
            _fontSize, text,  // size + text
            vertices, indices,  // buffers
            Magnum::GL::BufferUsage::StaticDraw,  // as static text, buffers do not change
            alignment  // alignment
        );

        // explicit std::move
        return {
            std::move(mesh),
            std::move(indices),
            std::move(vertices),
            std::move(geometry)
        };
    }

 private:
    Magnum::Text::AbstractFont* _font;
    const Magnum::Text::GlyphCache* _cache;
    Magnum::Float _fontSize;
};

}  // namespace Widget

}  // namespace UnderStory
