// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020-2021 Guillaume Vara <guillaume.vara@gmail.com>

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

    Asset fetchAsset(const google::protobuf::uint64 &fileId) {
        auto assets = this->_database.mutable_assets();
        if(!assets->contains(fileId)) {
            spdlog::warn("Cannot find [{}] asset from database", fileId);
            return Asset();
        }

        // duplicate
        Asset asset;
        asset.CopyFrom(assets->at(fileId));

        // get asset file path
        auto assetFilePath = _assetFilePath(asset);
        if(!fs::exists(assetFilePath)) {
            spdlog::warn("Cannot find [{}] asset file", assetFilePath.string());
            return Asset();
        }

        // read
        std::string assetFileBytes;
        {
            std::ifstream assetFile(assetFilePath);
            assetFile >> assetFileBytes;
            assetFile.close();
        }

        // bind
        asset.mutable_file()->set_content(assetFileBytes);
        return asset;
    }

    void integrateAsset(Asset asset) {
        // determine asset file path
        auto dest = this->_assetFilePath(asset);

        // write asset file
        {
            std::ofstream assetFile(dest);
            assetFile << asset.mutable_file()->content();
            assetFile.close();
        }

        // clear bytes
        asset.mutable_file()->clear_content();

        // prepare
        auto assets = this->_database.mutable_assets();
        auto hash = asset.file().file_description().fnv1a_hash();

        // insert to container
        assets->insert({
            hash,
            asset
        });

        // save
        this->_saveState();
    }

 private:
    AssetDatabase _database;

    const fs::path _assetFilePath(const Asset &asset) const {
        auto &fileDescr = asset.file().file_description();
        auto filename = (std::to_string(fileDescr.fnv1a_hash()) + fileDescr.file_extension());

        return this->_context.path() / resourcesFolder / filename;
    }

    void _saveState() override {
        std::string json;
        auto result = google::protobuf::util::MessageToJsonString(this->_database, &json);
        if(result != google::protobuf::util::Status::OK) throw std::exception();

        auto fs = this->_createWriteFS(resourcesDbFile);
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

