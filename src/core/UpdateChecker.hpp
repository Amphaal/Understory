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
#include <map>

#include "src/base/understory.h"

#include "src/network/HTTPDownloader.hpp"

#include <process.hpp>

namespace UnderStory {

class UpdateChecker_Private {
 public:
    static std::string _getManifest() {
        spdlog::info("UpdateChecker : Downloading remote manifest [{}, {}]", APP_REPOSITORY_HOST, APP_REPOSITORY_COMMAND);
        return Network::DownloadHTTPFile(APP_REPOSITORY_HOST, APP_REPOSITORY_COMMAND);
    }

    static auto _extractRemoteVersionsFromManifest(std::string manifestContent) {
        // remove newline
        manifestContent.erase(
            std::remove(
                manifestContent.begin(), 
                manifestContent.end(), 
                '\n'
            ), 
            manifestContent.end()
        );
        
        //
        std::regex findVersionExp(R"|(<Name>(.*?)<\/Name>.*?<Version>(.*?)<\/Version>)|");
        std::smatch match;
        std::map<std::string, std::string> out;

        //
        std::string::const_iterator searchStart( manifestContent.cbegin() );
        while(std::regex_search(searchStart, manifestContent.cend(), match, findVersionExp)) {
            //
            bool first = true;
            std::string key;
            auto count = match.size();

            // iterate
            for (auto &group : match) {
                
                // skip first (whole pattern)
                if (first) {
                    first ^= true;
                    continue;
                }

                // store key
                if (key.empty()) {
                    key = group.str();
                    continue;
                }

                // store version
                auto version = group.str();
                spdlog::info("UpdateChecker : Found remote version from manifest for \"{}\" [{}]", key, version);
                out.insert_or_assign(key, version);
                break;

            }

            //
            searchStart = match.suffix().first;

        }

        // warn if empty
        if(out.empty())
            spdlog::warn("UpdateChecker : Cannot find remote versions from manifest");

        //
        return out;
    }

    static bool _isVersionNewerThanLocal(const std::string &remoteVersion, const std::string &localVersion = APP_CURRENT_VERSION) {
        spdlog::info("UpdateChecker : Local version is [{}]", localVersion);
        auto isNewer =  localVersion < remoteVersion;
        spdlog::info("UpdateChecker : Must update from remote ? [{}]", isNewer);
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

    // returns if successfully requested updater to run
    static bool tryToLaunchUpdater(const std::filesystem::path &updaterPath = _exectedMaintenanceToolPath()) {
        if(!std::filesystem::exists(updaterPath)) {
            spdlog::warn("UpdateChecker : Cannot find updater at [{}], aborting.", updaterPath.string());
            return false;
        }

        // run
        std::vector<std::string> args {updaterPath.string(), "--updater"};
        spdlog::info("UpdateChecker : Launching updater [{}]...", updaterPath.string());
        TinyProcessLib::Process run(args);

        spdlog::info("UpdateChecker : Quitting {}...", APP_NAME);
        return true;
    }

 private:
    static bool _isNewerVersionAvailable() {
        spdlog::info("UpdateChecker : Checking updates...");
        auto manifest = UpdateChecker_Private::_getManifest();
        
        // read versions
        auto versions = UpdateChecker_Private::_extractRemoteVersionsFromManifest(manifest);
        auto hasVersions = versions.size();
        if(!hasVersions) return false;

        // compare
        auto firstVersion = versions.begin()->second;
        return UpdateChecker_Private::_isVersionNewerThanLocal(firstVersion);
    }
};

}  // namespace UnderStory
