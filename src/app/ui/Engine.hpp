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

#include <iostream>
#include <string>
#include <vector>
#include <utility>

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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawImage.width, rawImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImage.pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        _height = rawImage.height;
        _width = rawImage.width;
    }

    void use() const {
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    ~Texture() {
        glDeleteTextures(1, &_texture);
    }

    int height() const { return _height; }
    int width() const { return _width; }

 private:
    GLuint _texture;
    int _height;
    int _width;
};

class Engine {
 public:
    Engine() {}
    ~Engine() {
        glDeleteVertexArrays(_vertexArraysIndexes.size(), _vertexArraysIndexes.data());
        glDeleteBuffers(_buffersIndexes.size(), _buffersIndexes.data());
    }

    void init() {
        this->_loadDataInBuffers();

        GLuint vShaderId = _compileShaders(vertexShader, GL_VERTEX_SHADER);
        GLuint fShaderId = _compileShaders(fragmentShader, GL_FRAGMENT_SHADER);

        this->_programId = _linkProgram(vShaderId, fShaderId);
    }

    Texture& useAsTexture(const std::string &path) {
        auto rawImg = UnderStory::Utility::getRawImage(path, true);
        return _textures.emplace_back(rawImg);
    }

    void draw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        _textures[0].use();

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

        glUseProgram(_programId);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

 private:
    GLuint _programId;
    std::vector<GLuint> _vertexArraysIndexes;
    std::vector<GLuint> _buffersIndexes;
    std::vector<Texture> _textures;

    GLuint _VBO, _VAO, _EBO;

    static inline std::string vertexShader = R"(
        #version 410 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;

        out vec3 ourColor;
        out vec2 TexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            ourColor = aColor;
            TexCoord = aTexCoord;
        }
    )";

    static inline std::string fragmentShader = R"(
        #version 410 core
        out vec4 FragColor;
        
        in vec3 ourColor;
        in vec2 TexCoord;

        uniform sampler2D ourTexture;

        void main()
        {
            FragColor = texture(ourTexture, TexCoord);
        }
    )";

    // Compile and create shader object and returns its id
    static GLuint _compileShaders(std::string shader, GLenum type) {
        const char *shaderCode = shader.c_str();
        GLuint shaderId = glCreateShader(type);

        if (shaderId == 0) {  // Error: Cannot create shader object
            std::cout << "Error creating shaders!";
            return 0;
        }

        // Attach source code to this object
        glShaderSource(shaderId, 1, &shaderCode, NULL);
        glCompileShader(shaderId);  // compile the shader object

        GLint compileStatus;

        // check for compilation status
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

        if (!compileStatus) {  // If compilation was not successfull
            int length;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
            char *cMessage = new char[length];

            // Get additional information
            glGetShaderInfoLog(shaderId, length, &length, cMessage);
            std::cout << "Cannot Compile Shader: " << cMessage;
            delete[] cMessage;
            glDeleteShader(shaderId);
            return 0;
        }

        return shaderId;
    }

    // Creates a program containing vertex and fragment shader
    // links it and returns its ID
    static GLuint _linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId) {
        GLuint _programId = glCreateProgram();  // crate a program

        if (_programId == 0) {
            std::cout << "Error Creating Shader Program";
            return 0;
        }

        // Attach both the shaders to it
        glAttachShader(_programId, vertexShaderId);
        glAttachShader(_programId, fragmentShaderId);

        // Create executable of this program
        glLinkProgram(_programId);

        GLint linkStatus;

        // Get the link status for this program
        glGetProgramiv(_programId, GL_LINK_STATUS, &linkStatus);

        if (!linkStatus) {  // If the linking failed
            std::cout << "Error Linking program";
            glDetachShader(_programId, vertexShaderId);
            glDetachShader(_programId, fragmentShaderId);
            glDeleteProgram(_programId);

            return 0;
        }

        return _programId;
    }

    void _loadDataInBuffers() {
        GLfloat vertices[] = {
            // positions            // colors           // texture coords
            1.0f,  1.0f, 0.0f,      1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
            1.0f, -1.0f, 0.0f,      0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
            -1.0f,  1.0f, 0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
        };

        GLuint indices[] = {
            0, 1, 3,  // first triangle
            1, 2, 3   // second triangle
        };
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glGenBuffers(1, &_EBO);

        _vertexArraysIndexes.push_back(_VAO);
        _buffersIndexes.push_back(_VBO);
        _buffersIndexes.push_back(_EBO);

        glBindVertexArray(_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        auto fSize = sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * fSize, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * fSize, reinterpret_cast<void*>(3 * fSize));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * fSize, reinterpret_cast<void*>(6 * fSize));
        glEnableVertexAttribArray(2);

        useAsTexture("logo.png");
    }
};

}  // namespace UI

}  // namespace UnderStory
