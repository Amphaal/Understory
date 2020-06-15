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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include <nuklear.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include <string>

#include "nuklear_sdl_gl3.h"

namespace UnderStory {

namespace UI {

class Nuklear {
 public:
    Nuklear() {}
    ~Nuklear() {
        if(!_initd) return;

        nk_glfw3_shutdown(&this->_nk_glfw);
    }

    void init(GLFWwindow* window, nk_colorf * modifiableBgColor) {
        this->_nk_ctx = nk_glfw3_init(&this->_nk_glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

        nk_glfw3_font_stash_begin(&this->_nk_glfw, &this->_nk_atlas);
        nk_glfw3_font_stash_end(&this->_nk_glfw);

        _nk_bg = modifiableBgColor;

        _initd = true;
    }

    void drawModal(const std::string &text) {
        nk_glfw3_new_frame(&this->_nk_glfw);
        nk_flags flags = NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE;

        if (nk_begin(this->_nk_ctx, "Demo", nk_rect(0, 0, 230, 250), flags)) {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(this->_nk_ctx, 30, 80, 1);
            if (nk_button_label(this->_nk_ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(this->_nk_ctx, 30, 2);
            if (nk_option_label(this->_nk_ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(this->_nk_ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
            nk_property_int(this->_nk_ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(this->_nk_ctx, 20, 1);
            nk_label(this->_nk_ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
            if (nk_combo_begin_color(this->_nk_ctx, nk_rgb_cf(*this->_nk_bg), nk_vec2(nk_widget_width(this->_nk_ctx), 400))) {
                nk_layout_row_dynamic(this->_nk_ctx, 120, 1);
                *this->_nk_bg = nk_color_picker(this->_nk_ctx, *this->_nk_bg, NK_RGBA);
                nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                this->_nk_bg->r = nk_propertyf(this->_nk_ctx, "#R:", 0, this->_nk_bg->r, 1.0f, 0.01f, 0.005f);
                this->_nk_bg->g = nk_propertyf(this->_nk_ctx, "#G:", 0, this->_nk_bg->g, 1.0f, 0.01f, 0.005f);
                this->_nk_bg->b = nk_propertyf(this->_nk_ctx, "#B:", 0, this->_nk_bg->b, 1.0f, 0.01f, 0.005f);
                this->_nk_bg->a = nk_propertyf(this->_nk_ctx, "#A:", 0, this->_nk_bg->a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(this->_nk_ctx);
            }

            //
            auto sint = (sin(glfwGetTime()) + 1) / 2;
            auto sint_str = std::string("sin/t: ") + std::to_string(sint);
            nk_label(this->_nk_ctx, sint_str.c_str(), NK_TEXT_LEFT);
        }
        nk_end(this->_nk_ctx);

        nk_glfw3_render(&this->_nk_glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    }

    void drawTest() {
        nk_glfw3_new_frame(&this->_nk_glfw);

            nk_flags flags = NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE;

            if (nk_begin(this->_nk_ctx, "Demo", nk_rect(0, 0, 230, 250), flags)) {
                enum {EASY, HARD};
                static int op = EASY;
                static int property = 20;
                nk_layout_row_static(this->_nk_ctx, 30, 80, 1);
                if (nk_button_label(this->_nk_ctx, "button"))
                    fprintf(stdout, "button pressed\n");

                nk_layout_row_dynamic(this->_nk_ctx, 30, 2);
                if (nk_option_label(this->_nk_ctx, "easy", op == EASY)) op = EASY;
                if (nk_option_label(this->_nk_ctx, "hard", op == HARD)) op = HARD;

                nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                nk_property_int(this->_nk_ctx, "Compression:", 0, &property, 100, 10, 1);

                nk_layout_row_dynamic(this->_nk_ctx, 20, 1);
                nk_label(this->_nk_ctx, "background:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                if (nk_combo_begin_color(this->_nk_ctx, nk_rgb_cf(*this->_nk_bg), nk_vec2(nk_widget_width(this->_nk_ctx), 400))) {
                    nk_layout_row_dynamic(this->_nk_ctx, 120, 1);
                    *this->_nk_bg = nk_color_picker(this->_nk_ctx, *this->_nk_bg, NK_RGBA);
                    nk_layout_row_dynamic(this->_nk_ctx, 25, 1);
                    this->_nk_bg->r = nk_propertyf(this->_nk_ctx, "#R:", 0, this->_nk_bg->r, 1.0f, 0.01f, 0.005f);
                    this->_nk_bg->g = nk_propertyf(this->_nk_ctx, "#G:", 0, this->_nk_bg->g, 1.0f, 0.01f, 0.005f);
                    this->_nk_bg->b = nk_propertyf(this->_nk_ctx, "#B:", 0, this->_nk_bg->b, 1.0f, 0.01f, 0.005f);
                    this->_nk_bg->a = nk_propertyf(this->_nk_ctx, "#A:", 0, this->_nk_bg->a, 1.0f, 0.01f, 0.005f);
                    nk_combo_end(this->_nk_ctx);
                }

                //
                auto sint = (sin(glfwGetTime()) + 1) / 2;
                auto sint_str = std::string("sin/t: ") + std::to_string(sint);
                nk_label(this->_nk_ctx, sint_str.c_str(), NK_TEXT_LEFT);
            }
            nk_end(this->_nk_ctx);

        nk_glfw3_render(&this->_nk_glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    }

 private:
    bool _initd = false;
    nk_font_atlas* _nk_atlas;
    nk_glfw _nk_glfw = {0};
    nk_context* _nk_ctx;
    nk_colorf* _nk_bg;
};

}  // namespace UI

}  // namespace UnderStory
