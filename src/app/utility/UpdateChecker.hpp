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

#include <functional>
#include <future>

#include "src/core/UpdateChecker.hpp"

namespace UnderStory {

namespace Utility {

class UpdateChecker {
 public:
    UpdateChecker() {}

    void start() {
        // invoke update check
        _future = UnderStory::UpdateChecker::isNewerVersionAvailable();
    }

    void poll() {
        //
        if(_resolved) return;

        //
        if(!_future.valid() || _future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) return;

        //
        auto isNewerVersionAvailable = _future.get();
        if(isNewerVersionAvailable)
            UnderStory::UpdateChecker::tryToLaunchUpdater();
        
        //
        _resolved = true;
    }

 private:
    std::future<bool> _future;
    bool _resolved = false;
};

}  // namespace Utility

}  // namespace UnderStory
