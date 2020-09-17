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

#include <GLFWM/glfwm.hpp>

#include "src/app/Utility.hpp"

#include "Texture.hpp"
#include "EngineInternal.hpp"

#include "src/app/ui/layout/GridLayout.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace UnderStory {

namespace UI {

class Engine {
 public:
    explicit Engine(const UnderStory::Utility::Size* constraints, const glm::vec2* pointerPos) :
        _constraints(constraints), _assetsGrid(constraints, pointerPos) {}

    ~Engine() {
        if(!_initd) return;

        glDeleteVertexArrays(_vertexArraysIndexes.size(), _vertexArraysIndexes.data());
        glDeleteBuffers(_buffersIndexes.size(), _buffersIndexes.data());
    }

    bool onScrollEvent(glfwm::EventScroll* event) {
        // TODO(amphaal)
        return true;
    }

    bool onKeyPress(glfwm::EventKey* event) {
        if(event->getAction() != glfwm::ActionType::RELEASE) return true;

        return true;
    }

    void init() {
        //
        this->_programId = EngineInternal::getProgram();
        this->_loadDataInBuffers();

        //
        glUseProgram(_programId);

        // _assetsGrid.addTiles(1);
        // _assetsGrid.setOnTileDrawing([=](std::unique_ptr<UnderStory::UI::GridTile> &tile) {
        //     {
        //         glm::mat4 model(1.0);
        //         model = glm::translate(model, glm::vec3(tile->currentPos, 0.0f));

        //         int modelLoc = glGetUniformLocation(_programId, "model");
        //         glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //     }
        //     glDrawArrays(GL_TRIANGLES, 0, 6);
        // });

        onWindowSizeChange();
        updateView();

        _initd = true;
    }

    bool started() const {
        return _initd;
    }

    void draw() {
        // // define matrixes

        // model
        {
            glm::mat4 model(1.0);
            // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            int modelLoc = glGetUniformLocation(_programId, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }

        // // animate and draw
        // _assetsGrid.advance();
        // _assetsGrid.draw();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void onWindowSizeChange() {
        // projection
        {
            glm::mat4 projection(1.0);
            projection = glm::perspective(glm::radians(45.0f), _constraints->wF() / _constraints->hF(), 0.1f, 100.0f);

            int projLoc = glGetUniformLocation(_programId, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }
    }

    void updateView() {
        {
            glm::mat4 view(1.0);
            view = glm::translate(view, glm::vec3(_xWorld, _yWorld, _zWorld));

            int viewLoc = glGetUniformLocation(_programId, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        }
    }

 private:
    bool _initd = false;
    GLuint _programId;

    GLuint _VBO, _VAO, _EBO;
    std::vector<GLuint> _vertexArraysIndexes;
    std::vector<GLuint> _buffersIndexes;
    std::vector<Texture> _textures;

    //
    const UnderStory::Utility::Size* _constraints = nullptr;
    GridLayout _assetsGrid;

    //
    float _xWorld = .0f;
    float _yWorld = .0f;
    float _zWorld = -3.0f;

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

    Texture& _useAsTexture(const std::string &path) {
        auto rawImg = UnderStory::Utility::getRawImage(path, true);
        return _textures.emplace_back(rawImg);
    }
};

}  // namespace UI

}  // namespace UnderStory
