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
#include <memory>

#include "src/app/Utility.hpp"

#include "Texture.hpp"
#include "EngineInternal.hpp"

#include "src/app/ui/layout/GridLayout.hpp"

namespace UnderStory {

namespace UI {

class Engine {
 public:
    explicit Engine(const UnderStory::Utility::Size* constraints, const glm::vec2* pointerPos) : _layout(constraints, pointerPos) {}
    ~Engine() {
        if(!_initd) return;

        glDeleteVertexArrays(_vertexArraysIndexes.size(), _vertexArraysIndexes.data());
        glDeleteBuffers(_buffersIndexes.size(), _buffersIndexes.data());
    }

    bool onKeyPress(glfwm::EventKey* event) {
        if(event->getAction() != glfwm::ActionType::RELEASE) return true;

        switch (event->getKey()) {
            case glfwm::KeyType::KEY_A: {
                _layout.addTiles(1);
            }
            break;

            case glfwm::KeyType::KEY_Z: {
                _layout.removeTiles(1);
            }
            break;

            case glfwm::KeyType::KEY_Q: {
                _layout.changeColor();
            }
            break;

            default:
                break;
        }

        return true;
    }

    void init() {
        this->_programId = EngineInternal::getProgram();

        _layout.setOnTileDrawing([](const std::unique_ptr<GridTile>& tile) {
            glBegin(GL_QUADS);
                glColor4f(tile->currentColor[0], tile->currentColor[1], tile->currentColor[2], tile->currentColor[3]);
                glVertex2f(tile->currentRect[0], tile->currentRect[1]);
                glVertex2f(tile->currentRect[2], tile->currentRect[1]);
                glVertex2f(tile->currentRect[2], tile->currentRect[3]);
                glVertex2f(tile->currentRect[0], tile->currentRect[3]);
            glEnd();
        });
        _layout.addTiles(1);

        this->_loadDataInBuffers();
        _initd = true;
    }

    void draw() {
        _layout.advance();
        _layout.draw();
    }

    // void draw(const Utility::Size &framebufferSize) {
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // glActiveTexture(GL_TEXTURE0);
        // auto &logoTexture = _textures[0];
        // logoTexture.use();

        // glBindVertexArray(_VAO);
        // glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

        // glUseProgram(_programId);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // }

 private:
    bool _initd = false;
    GLuint _programId;
    std::vector<GLuint> _vertexArraysIndexes;
    std::vector<GLuint> _buffersIndexes;
    std::vector<Texture> _textures;
    GridLayout _layout;

    GLuint _VBO, _VAO, _EBO;

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

        _useAsTexture("logo.png");
    }

    Texture& _useAsTexture(const std::string &path) {
        auto rawImg = UnderStory::Utility::getRawImage(path, true);
        return _textures.emplace_back(rawImg);
    }
};

}  // namespace UI

}  // namespace UnderStory
