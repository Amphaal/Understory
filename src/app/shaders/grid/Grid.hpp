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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/Assert.h>

#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/GL.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/AbstractShaderProgram.h>

#include <Magnum/Math/Matrix4.h>

#include <Magnum/Shaders/Generic.h>

namespace UnderStory {

namespace Shader {

class Grid : public Magnum::GL::AbstractShaderProgram {
 public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
    typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;
    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Vector2 textureCoordinates;
    };
    explicit Grid(Magnum::NoCreateT): Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {}

    explicit Grid(const Magnum::Utility::Resource &rs) {
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL330);

        Magnum::GL::Shader vert{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Vertex};
        Magnum::GL::Shader frag{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Fragment};

        vert.addSource(rs.get("Grid.vert"));
        frag.addSource(rs.get("Grid.frag"));

        CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

        attachShaders({vert, frag});

        CORRADE_INTERNAL_ASSERT_OUTPUT(link());

        // get uniforms in correct order
        _projectionMatrixUniform = uniformLocation("u_projMatrix");
        _scaleUniform            = uniformLocation("u_scale");
        setUniform(uniformLocation("textureData"), TextureUnit);
    }

    Grid& setProjectionMatrix(const Magnum::Matrix3& projectionMatrix) {
        setUniform(_projectionMatrixUniform, Magnum::Matrix4{projectionMatrix});
        return *this;
    }

    Grid& setScale(Magnum::Float scale) {
        setUniform(_scaleUniform, scale);
        return *this;
    }

    Grid& bindTexture(Magnum::GL::Texture2D& texture) {
        texture.bind(TextureUnit);
        return *this;
    }

 private:
    enum: Magnum::Int { TextureUnit = 0 };

    Magnum::Int _projectionMatrixUniform, _scaleUniform;
};

}  // namespace Shader

}  // namespace UnderStory
