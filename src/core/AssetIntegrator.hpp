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

#include <string>

#include "FileIntegrator.hpp"
#include "src/models/Asset.pb.h"

#include "src/base/Context.hpp"

namespace UnderStory {

class AssetContext : public ContextImplementation {
 public:
    static inline const std::string resourcesFolder = "resources";
    static inline const std::string resourcesDbFile = "resources.json";

    explicit AssetContext(const Context &context) : ContextImplementation(context, {resourcesFolder}) {
        // read state
        auto stream = this->_createReadFS(resourcesDbFile);
        this->_database.ParseFromIstream(&stream);
        stream.close();
    }

    void integrateAsset(Asset asset) {
        auto &fileDescr = asset.file().file_description();
        auto hash = fileDescr.fnv1a_hash();

        // determine asset file path
        auto dest = this->_context.path() / resourcesFolder / (std::to_string(hash) + fileDescr.file_extension());

        // write asset file
        std::ofstream assetFile(dest);
        asset.mutable_file()->content();


        // clear bytes
        asset.mutable_file()->clear_content();

        // insert to container
        auto assets = this->_database.mutable_assets();
        assets->insert({
            hash,
            asset
        });

        // save
        this->_saveState();
    }

 private:
    AssetDatabase _database;

    void _saveState() override {
        std::string json;
        google::protobuf::util::MessageToJsonString(this->_database, &json);
        auto fs = std::fstream();
        fs << json;
        fs.close();
    }
};

class AssetIntegrator {
 public:
    static Asset createAsset(const fs::path &filePath) {
        // generate file
        auto file = FileIntegrator::createFile(filePath);

        // get size of image
        auto size = _getWidthAndHeight(filePath);

        // bind to Asset
        Asset asset;
        asset.set_allocated_file(file);
        asset.set_allocated_dimensions(size);

        return asset;
    }

 private:
    static Asset_Size* _getWidthAndHeight(const fs::path &filePath) {
        // find x and y
        int x, y, channels;
        stbi_load(
            filePath.string().c_str(),
            &x,
            &y,
            &channels,
            0
        );

        // fill container
        auto size = new Asset_Size;
        size->set_width(x);
        size->set_height(y);

        return size;
    }
};

}  // namespace UnderStory

