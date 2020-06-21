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

namespace UnderStory {

namespace UI {

class EngineInternal {
 public:
    static GLuint getProgram() {
        GLuint vShaderId = _compileShaders(vertexShader, GL_VERTEX_SHADER);
        GLuint fShaderId = _compileShaders(fragmentShader, GL_FRAGMENT_SHADER);

        return _linkProgram(vShaderId, fShaderId);
    }

 private:
    static inline std::string vertexShader = R"(
        #version 400 core
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
        #version 400 core
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
};

}  // namespace UI

}  // namespace UnderStory
