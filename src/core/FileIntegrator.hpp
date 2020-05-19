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

#include <cassert>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>

#include "src/base/fs_compat.h"

#include "src/models/File.pb.h"

#include "hash_fnv1a.hpp"

namespace UnderStory {

class FileIntegrator {
 public:
    static File* createFile(const fs::path &filePath) {
        assert(fs::exists(filePath));

        // read file content
        std::ifstream fileStream(filePath.c_str(), std::ifstream::in | std::ios::binary);
        std::string fileContent;
        fileContent.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

        // get file hash
        auto hash = hash_64_fnv1a(fileContent.c_str(), fileContent.length());

        // bind to File
        auto file = new File;
            auto description = new FileDescriptor;
            description->set_name(filePath.stem().string());
            description->set_file_extension(filePath.extension().string());
            description->set_fnv1a_hash(hash);

        file->set_content(fileContent.c_str());
        file->set_content_length(fileContent.length());
        file->set_allocated_file_description(description);

        return file;
    }
};

}  // namespace UnderStory

