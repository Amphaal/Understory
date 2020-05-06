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

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>

#include "hash_fnv1a.hpp"

#include "src/models/Asset.capnp.h"

namespace UnderStory {

class AssetIntegrator {
 public:
    static Asset::Builder createAsset(const std::filesystem::path &filePath) {
        assert(std::filesystem::exists(filePath));

        ::capnp::MallocMessageBuilder message;
        auto asset = message.initRoot<Asset>();

        // set file extension
        {
            auto ext = filePath.extension().string();
            auto extBuilder = asset.initFileExtension(ext.length());
            std::copy(ext.begin(), ext.end(), extBuilder.begin());
            asset.setFileExtension(extBuilder);
        }

        // std::ifstream fileStream(filePath.c_str(), std::ifstream::in);
        // assert(fileStream);


        //     _fillWidthAndHeight(asset, fileStream);
        //     auto file = asset.initFile(10);
        //     fileStream->


        return asset;
    }

 private:
    static void _fillWidthAndHeight(Asset::Builder &asset, const std::filesystem::path &filePath) {
        int x, y, channels;

        auto fileStream = fopen(filePath.string().c_str(), "r");
        assert(fileStream);

            auto logoAsBMP = stbi_load_from_file(
                fileStream,
                &x,
                &y,
                &channels,
                0
            );

        fclose(fileStream);

        auto size = asset.initSize();
        size.setHeight(y);
        size.setWidth(x);

        asset.setSize(size);
    }
};

}  // namespace UnderStory

