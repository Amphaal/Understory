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

#include <stb_image.h>

#include <vector>
#include <string>

// resources
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(appResources);

namespace UnderStory {

class Utility {
 public:
    struct Size {
        int width;
        int height;

        float wF() const {
            return static_cast<float>(width);
        }

        float hF() const {
            return static_cast<float>(height);
        }
    };

    struct RawImage {
        Size s;
        stbi_uc * pixels = nullptr;
        int channels;

        ~RawImage() {
            if(pixels) delete pixels;
        }
    };

    static const RawImage getIcon() {
        return getRawImage("logo.png");
    }

    static cmrc::file getResource(const std::string &path) {
        return _getResource(path);
    }

    static const RawImage getRawImage(const std::string &path, bool flip = false) {
        auto iconF = _getResource(path);
        std::vector<unsigned char> icon{iconF.begin(), iconF.end()};

        int width, height, channels;
        stbi_set_flip_vertically_on_load(flip);
        auto logoAsBMP = stbi_load_from_memory(
            icon.data(),
            icon.size(),
            &width,
            &height,
            &channels,
            0
        );

        if(!channels || !logoAsBMP) {
            throw std::exception();
        }

        return {
            width,
            height,
            logoAsBMP,
            channels
        };
    }

 private:
    static cmrc::file _getResource(const std::string &path) {
        return cmrc::appResources::get_filesystem().open(path);
    }
};

}  // namespace UnderStory
