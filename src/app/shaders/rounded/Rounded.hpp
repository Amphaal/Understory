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

class Rounded : public Magnum::GL::AbstractShaderProgram {
 public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
    struct Vertex {
        Magnum::Vector2 position;
    };
    explicit Rounded(Magnum::NoCreateT): Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {}

    explicit Rounded(const Magnum::Utility::Resource &rs) {
            MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL330);

            Magnum::GL::Shader vert{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Vertex};
            Magnum::GL::Shader frag{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Fragment};

            vert.addSource(rs.get("Rounded.vert"));
            frag.addSource(rs.get("Rounded.frag"));

            CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

            attachShaders({vert, frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());

            // get uniforms in correct order
            _projMatrix         = uniformLocation("u_projMatrix");
            _color              = uniformLocation("u_color");
            _rectBottomLeftPx   = uniformLocation("u_rectBottomLeftPx");
            _rectSizePx         = uniformLocation("u_rectSizePx");
            _resolutionPx       = uniformLocation("u_resolutionPx");
        }

        Rounded& setProjectionMatrix(const Magnum::Matrix3& projectionMatrix) {
            setUniform(_projMatrix, Magnum::Matrix4{projectionMatrix});
            return *this;
        }
        
        Rounded& setColor(const Magnum::Color4& color) {
            setUniform(_color, color);
            return *this;
        }

        Rounded& setRectPx(const Magnum::Range2D& rectInPx) {
            setUniform(_rectBottomLeftPx, rectInPx.min());
            setUniform(_rectSizePx, rectInPx.size());
            return *this;
        }

        Rounded& setResolution(const Magnum::Vector2& resolutionInPx) {
            setUniform(_resolutionPx, resolutionInPx);
            return *this;
        }

 private:
    Magnum::Int _projMatrix, _color, _rectBottomLeftPx, _rectSizePx, _resolutionPx;
};

}  // namespace Shader

}  // namespace UnderStory
