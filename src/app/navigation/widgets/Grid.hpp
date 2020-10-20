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

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Mesh.h>
#include <Magnum/GL/Sampler.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Math.h>

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/GL/TextureFormat.h>

#include <utility>

#include "src/app/shaders/grid/Grid.hpp"

namespace UnderStory {

namespace Widget {

// TODO(amphaal) grid sized to DPI
class Grid {
 public:
    explicit Grid(const Magnum::Utility::Resource &rs, float mapSize, int windowHeight) : _shader(rs) {
        // load texture
        Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;
        auto importer = manager.loadAndInstantiate("PngImporter");
        if(!importer) std::exit(1);
        if(!importer->openData(rs.getRaw("grid.png"))) std::exit(2);

        // set texture
        auto image = importer->image2D(0);
        auto levels = Magnum::Math::log2(image->size().x()) + 1;
        CORRADE_INTERNAL_ASSERT(image);
        _texture
            .setWrapping(Magnum::GL::SamplerWrapping::Repeat)
            .setMagnificationFilter(Magnum::GL::SamplerFilter::Nearest)
            .setMinificationFilter(Magnum::GL::SamplerFilter::Nearest)
            .setMaxAnisotropy(Magnum::GL::Sampler::maxMaxAnisotropy())
            .setStorage(levels, Magnum::GL::textureFormat(image->format()), image->size())
            .setSubImage(0, {}, *image)
            .generateMipmap();

        // compile shader
        _shader = Shader::Grid{rs};

        // define indices
        Magnum::GL::Buffer indices;
        indices.setData({
            0, 1, 2,
            2, 3, 0
        });

        // define data and structure
        auto squareSize = Magnum::Vector2 {image->size()} * (1.f / static_cast<float>(windowHeight));
        auto gridRadius = squareSize * mapSize / 2.f;
        const Shader::Grid::Vertex gridVData[]{
            { {-gridRadius.x(),  gridRadius.y()}, {.0f,         mapSize} },
            { { gridRadius.x(),  gridRadius.y()}, {mapSize,    mapSize} },
            { { gridRadius.x(), -gridRadius.y()}, {mapSize,    .0f} },
            { {-gridRadius.x(), -gridRadius.y()}, {.0f,         .0f} }
        };

        // bind to buffer
        Magnum::GL::Buffer gridVertices;
        gridVertices.setData(gridVData);

        // define mesh
        _mesh.setCount(indices.size())
            .addVertexBuffer(std::move(gridVertices), 0, Shader::Grid::Position{}, Shader::Grid::TextureCoordinates{})
            .setIndexBuffer (std::move(indices),      0, Magnum::MeshIndexType::UnsignedInt);
    }

    void draw(const Magnum::Matrix3& matrix, const Magnum::Float& scale) {
        _shader.bindTexture(_texture);
        _shader
            .setProjectionMatrix(matrix)
            .setScale(scale)
            .draw(_mesh);
    }

 private:
    Magnum::GL::Texture2D _texture;
    Shader::Grid _shader{Magnum::NoCreate};
    Magnum::GL::Mesh _mesh{Magnum::GL::MeshPrimitive::Triangles};
};

}  // namespace Widget

}  // namespace UnderStory
