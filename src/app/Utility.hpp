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
    struct RawImage {
        int width;
        int height;
        stbi_uc * pixels = nullptr;
        int channels;

        ~RawImage() {
            if(pixels) delete pixels;
        }
    };

    static const RawImage getIcon() {
        auto iconF = cmrc::appResources::get_filesystem().open("logo.png");
        std::vector<unsigned char> icon{iconF.begin(), iconF.end()};

        int width, height, channels;
        auto logoAsBMP = stbi_load_from_memory(
            icon.data(),
            icon.size(),
            &width,
            &height,
            &channels,
            0
        );

        return {
            width,
            height,
            logoAsBMP,
            channels
        };
    }
};

}  // namespace UnderStory
