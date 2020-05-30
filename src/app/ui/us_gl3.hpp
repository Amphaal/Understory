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
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <string>

class USEngine {
 public:
    static void init() {
        GLuint vboId = _loadDataInBuffers();

        GLuint vShaderId = _compileShaders(vertexShader, GL_VERTEX_SHADER);
        GLuint fShaderId = _compileShaders(fragmentShader, GL_FRAGMENT_SHADER);

        GLuint programId = _linkProgram(vShaderId, fShaderId);

        // Get the 'pos' variable location inside this program
        GLuint posAttributePosition = glGetAttribLocation(programId, "pos");

        GLuint vaoId;
        glGenVertexArrays(1, &vaoId);  // Generate VAO  (Vertex Array Object)

        // Bind it so that rest of vao operations affect this vao
        glBindVertexArray(vaoId);

        // buffer from which 'pos' will receive its data and the format of that data
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glVertexAttribPointer(posAttributePosition, 3, GL_FLOAT, false, 0, 0);

        // Enable this attribute array linked to 'pos'
        glEnableVertexAttribArray(posAttributePosition);

        // Use this program for rendering.
        glUseProgram(programId);
    }

 private:
    static inline std::string vertexShader = R"(
        #version 330
        in vec3 pos;
        void main()
        {
            gl_Position = vec4(pos, 1);
        }
    )";

    static inline std::string fragmentShader = R"(
        #version 330
        void main() 
        {
            gl_FragColor = vec4(1, 1, .5, 1); // white color
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
        GLuint programId = glCreateProgram();  // crate a program

        if (programId == 0) {
            std::cout << "Error Creating Shader Program";
            return 0;
        }

        // Attach both the shaders to it
        glAttachShader(programId, vertexShaderId);
        glAttachShader(programId, fragmentShaderId);

        // Create executable of this program
        glLinkProgram(programId);

        GLint linkStatus;

        // Get the link status for this program
        glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

        if (!linkStatus) {  // If the linking failed
            std::cout << "Error Linking program";
            glDetachShader(programId, vertexShaderId);
            glDetachShader(programId, fragmentShaderId);
            glDeleteProgram(programId);

            return 0;
        }

        return programId;
    }

    // Load data in VBO (Vertex Buffer Object) and return the vbo's id
    static GLuint _loadDataInBuffers() {
        GLfloat vertices[] = {  // triangle vertex coordinates
                            -0.5, -0.5, 0,
                            0.5, -0.5, 0,
                            0, 0.5, 0 };

        GLuint vboId;

        // allocate buffer sapce and pass data to it
        glGenBuffers(1, &vboId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // unbind the active buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return vboId;
    }
};
