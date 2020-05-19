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

#include <spdlog/spdlog.h>

#include <google/protobuf/util/json_util.h>

#include <string>
#include <random>
#include <vector>
#include <fstream>

#include "src/base/fs_compat.h"

#include "src/base/understory.h"

namespace UnderStory {

class Context {
 public:
    enum PredefinedEnvironement {
        UNKNOWN = 0,
        DEFAULT = 1,
        RANDOM = 2,
        CUSTOM = 3
    };

    Context() {}

    ~Context() {
        if(_env == RANDOM) fs::remove_all(this->_pathToContext);
    }

    static Context normal() {
        return Context(PredefinedEnvironement::DEFAULT, _getDefaultContextPath());
    }

    static Context random() {
        return Context(PredefinedEnvironement::RANDOM, _getTemporaryPath());
    }

    static Context custom(const std::string &customContextName) {
        return Context(PredefinedEnvironement::CUSTOM, _getCustomContextPath(customContextName));
    }

    const fs::path path() const {
        return this->_pathToContext;
    }

 private:
    PredefinedEnvironement _env = UNKNOWN;
    fs::path _pathToContext;

    Context(const PredefinedEnvironement& env, const fs::path &pathToContext) : _env(env), _pathToContext(pathToContext) {
        assert(_directoryExists(pathToContext));
        spdlog::debug("Using app context [{}]", pathToContext.string());
    }

    static bool _directoryExists(const fs::path &path) {
        return fs::exists(path) && fs::is_directory(path);
    }

    static fs::path _getTemporaryPath() {
        fs::path tmp_dir_path = fs::temp_directory_path();

        std::random_device dev;
        std::mt19937 prng(dev());
        std::uniform_int_distribution<> dist(1, UINT_MAX);

        fs::path randomPath = tmp_dir_path;
        do {
            randomPath = tmp_dir_path / std::to_string(dist(prng));

            // Attempt to create the directory.
            fs::create_directories(randomPath);
        }  while(!_directoryExists(randomPath));

        // If that failed an exception will have been thrown
        // so no need to check or throw your own

        // Directory successfully created.
        return randomPath;
    }

    static fs::path _getCustomContextPath(const std::string &customContextName) {
        auto path = _getDefaultContextPath() / "_CC" / customContextName;

        fs::create_directories(path);

        return path;
    }

    static fs::path _getDefaultContextPath() {
        fs::path path;

        #if defined(__APPLE__)  // OSX
            path = std::string("/Library/Application Support/") + APP_NAME;
        #elif defined(WIN32)  // Windows
            path = std::string("C:/Users/") + std::getenv("USERNAME") + std::string("/AppData/Local/") + APP_NAME;
        #else  // Linux
            path = std::string("/usr/share/") + APP_NAME;
        #endif

        fs::create_directories(path);

        return path;
    }
};

class ContextImplementation {
 public:
    explicit ContextImplementation(const Context &context, const std::vector<std::string> &linkedSubfolders) : _context(context) {
        // create subfolders
        for(auto &subfolder : linkedSubfolders) {
            auto concat = context.path() / "resources";
            fs::create_directories(concat);
            assert(fs::exists(concat));
        }
    }

 protected:
    Context _context;

    std::ofstream _createWriteFS(const std::string &subfile) const {
        return std::ofstream(_context.path() / subfile);
    }

    std::ifstream _createReadFS(const std::string &subfile) const {
        return std::ifstream(_context.path() / subfile);
    }

    void virtual _saveState() = 0;
};

}  // namespace UnderStory
