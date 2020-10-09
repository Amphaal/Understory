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

class Grid : public Magnum::GL::AbstractShaderProgram {
 public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
    typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;
    struct Vertex {
        Magnum::Vector2 position;
        Magnum::Vector2 textureCoordinates;
    };
    explicit Grid(Magnum::NoCreateT): Magnum::GL::AbstractShaderProgram{Magnum::NoCreate} {}

    Grid() {
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL330);

        /* Load and compile shaders from compiled-in resource */
        Magnum::Utility::Resource rs("data");

        Magnum::GL::Shader vert{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Vertex};
        Magnum::GL::Shader frag{Magnum::GL::Version::GL330, Magnum::GL::Shader::Type::Fragment};

        vert.addSource(rs.get("Grid.vert"));
        frag.addSource(rs.get("Grid.frag"));

        CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

        attachShaders({vert, frag});

        CORRADE_INTERNAL_ASSERT_OUTPUT(link());

        /* Matrices: view, transformation, projection, normal */
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
