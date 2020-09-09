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

#include <functional>
#include <string>
#include <regex>
#include <filesystem>
#include <iostream>
#include <vector>
#include <future>

#include "src/base/understory.h"

#include "src/network/HTTPDownloader.hpp"

#include <process.hpp>

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

                spdlog::debug("UpdateChecker : Found remote version from manifest [{}]", group.str());
                return group;
            }
        }

        spdlog::warn("UpdateChecker : Cannot find remote version from manifest");
        return std::string();
    }

    static bool _isVersionNewerThanLocal(const std::string &remoteVersion, const std::string &localVersion = APP_CURRENT_VERSION) {
        spdlog::debug("UpdateChecker : Local version is [{}]", localVersion);
        auto isNewer =  localVersion < remoteVersion;
        spdlog::debug("UpdateChecker : Must update from remote ? [{}]", isNewer);
        return isNewer;
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
            _isNewerVersionAvailable
        );
    }

    static void tryToLaunchUpdater(const std::filesystem::path &updaterPath = _exectedMaintenanceToolPath()) {
        if(!std::filesystem::exists(updaterPath)) {
            spdlog::warn("UpdateChecker : Cannot find updater at [{}]", updaterPath.string());
            return;
        }

        // run
        std::vector<std::string> args {updaterPath.string(), "--updater"};
        spdlog::debug("UpdateChecker : Launching updater [{}]...", updaterPath.string());
        TinyProcessLib::Process run(args);

        spdlog::debug("UpdateChecker : Quitting {}...", APP_NAME);
        exit(0);
    }

 private:
    static bool _isNewerVersionAvailable() {
        spdlog::debug("UpdateChecker : Checking updates...");
        auto manifest = UpdateChecker_Private::_getManifest();
        auto version = UpdateChecker_Private::_extractRemoteVersionFromManifest(manifest);
        return UpdateChecker_Private::_isVersionNewerThanLocal(version);
    }
};

}  // namespace UnderStory
