
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

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/app/ui/base/Shader.hpp"

namespace UnderStory {

namespace UI {

class GridShader : public Shader {
 public:
    GridShader() : Shader(
        R"(
        #version 330 core

        layout (location = 0) in vec2 aPos;

        out vec3 ourColor;
  
        uniform mat4 model;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * model * vec4(aPos, 0.0f, 1.0f);
        }
    )",
    R"(
        #version 330 core

        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )"
    ) {
        _loadDataInBuffers();
    }

    ~GridShader() {
        if(_vertexArraysIndexes.size()) glDeleteVertexArrays(_vertexArraysIndexes.size(), _vertexArraysIndexes.data());
        if(_buffersIndexes.size()) glDeleteBuffers(_buffersIndexes.size(), _buffersIndexes.data());
    }

    void defineView(float x, float y, float z) const {
        // translate
        glm::mat4 view(1.0);
        view = glm::translate(view, glm::vec3(x, y, z));

        // apply
        int viewLoc = glGetUniformLocation(getProgram(), "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

 private:
    GLuint _VBO, _VAO, _EBO;
    std::vector<GLuint> _vertexArraysIndexes;
    std::vector<GLuint> _buffersIndexes;

    void _loadDataInBuffers() {
        GLfloat vertices[] = {
            // 2D positions
            1.0f,  1.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f,
            -1.0f,  1.0f
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * fSize, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
    }
};

}  // namespace UI

}  // namespace UnderStory
