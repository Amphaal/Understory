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
#include <future>

#include "src/core/UpdateChecker.hpp"

namespace UnderStory {

namespace Utility {

class UpdateChecker {
 public:
    UpdateChecker() {}

    void start() {
        // invoke update check
        _updateCheckResult = UnderStory::UpdateChecker::isNewerVersionAvailable();
    }

    // returns if results are received
    bool checkResults() {
       if(!_updateCheckResult.valid()) return false;
        if(_updateCheckResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) return false;

        // get result and unsubscribe
        _isNewerVersionAvailable = _updateCheckResult.get();
        return true;
    }

    bool isNewerVersionAvailable() const {
        return _isNewerVersionAvailable;
    }

 private:
    std::future<bool> _updateCheckResult;
    bool _isNewerVersionAvailable = false;
};

}  // namespace Utility

}  // namespace UnderStory
