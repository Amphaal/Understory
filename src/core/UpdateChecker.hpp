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

#include <functional>
#include <string>
#include <regex>
#include <filesystem>
#include <iostream>

#include "src/base/understory.h"

#include "src/network/HTTPDownloader.hpp"

namespace UnderStory {

class UpdateChecker_Private {
 public:
    static std::string _getManifest() {
        return Network::DownloadHTTPFile(APP_REPOSITORY_HOST, APP_REPOSITORY_COMMAND);
    }

    static std::string _extractRemoteVersionFromManifest(const std::string &manifestContent) {
        std::regex findVersionExp(R"|(<Version>(.*?)<\/Version>)|");
        std::smatch match;

        while (std::regex_search(manifestContent, match, findVersionExp)) {
            // remove whole match
            if(match.length() < 2) continue;
            auto first = true;

            // return first occurence
            for (auto &group : match) {
                if (first) {
                    first = false;
                    continue;
                }
                return group;
            }
        }

        return std::string();
    }

    static bool _isVersionNewerThanLocal(const std::string &remoteVersion, const std::string &localVersion = APP_CURRENT_VERSION) {
        return localVersion < remoteVersion;
    }

    static std::filesystem::path _exectedMaintenanceToolPath() {
        auto installerDir = std::filesystem::current_path().parent_path();

        // TODO(amphaal) MacOS ?
        #ifdef WIN32
            std::string installerExec = "maintenancetool.exe";
        #else
            std::string installerExec = "maintenancetool";
        #endif

        auto installerPath = installerDir /= installerExec;
        return installerPath;
    }
};

class UpdateChecker : private UpdateChecker_Private {
 public:
    static std::future<bool> isNewerVersionAvailable() {
        return std::async(
            std::launch::async,
            [&]() { return _isNewerVersionAvailable(); }
        );
    }

    static void tryToLaunchUpdater() {
        auto path = _exectedMaintenanceToolPath();
        if(!std::filesystem::exists(path)) return;

        // command
        auto command = path.string();
        command += " --updater";

        // run
        std::system(command.c_str());

        exit(0);
    }

 private:
    static bool _isNewerVersionAvailable() {
        auto manifest = UpdateChecker_Private::_getManifest();
        auto version = UpdateChecker_Private::_extractRemoteVersionFromManifest(manifest);
        return UpdateChecker_Private::_isVersionNewerThanLocal(version);
    }
};

}  // namespace UnderStory
