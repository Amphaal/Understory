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

#include <GL/glew.h>

#include "src/app/Utility.hpp"

namespace UnderStory {

namespace UI {

class Texture {
 public:
    explicit Texture(const Utility::RawImage &rawImage) {
        // load and create a texture
        // -------------------------
        glGenTextures(1, &_texture);

        glBindTexture(GL_TEXTURE_2D, _texture);  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawImage.s.width, rawImage.s.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImage.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        _size = rawImage.s;
    }

    void use() const {
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    ~Texture() {
        glDeleteTextures(1, &_texture);
    }

    Utility::Size size() const { return _size; }

 private:
    GLuint _texture;
    Utility::Size _size;
};

}  // namespace UI

}  // namespace UnderStory
