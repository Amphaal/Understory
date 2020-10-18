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

#include <rxcpp/rx.hpp>

namespace UnderStory {

namespace Utility {

class UpdateChecker {
 public:
    UpdateChecker() {}

    void start() {
        // timer to check for completness
        _tmr = rxcpp::observable<>::interval(std::chrono::seconds(2))
            .subscribe_on(rxcpp::observe_on_new_thread())
            .subscribe([&](int) {
                this->_onUpdateCheckDone();
            });

        // invoke update check
        _updateCheckResult = UnderStory::UpdateChecker::isNewerVersionAvailable();
    }

 private:
    rxcpp::composite_subscription _tmr;

    std::future<bool> _updateCheckResult;
    void _onUpdateCheckDone() {
        if(!_updateCheckResult.valid()) return;
        if(_updateCheckResult.wait_for(std::chrono::seconds(0)) != std::future_status::ready) return;

        // get result and unsubscribe
        auto isNewerVersionAvailable = _updateCheckResult.get();
        _tmr.unsubscribe();

        // do nothing if not newer
        if(!isNewerVersionAvailable) return;

        // launch updater
        UnderStory::UpdateChecker::tryToLaunchUpdater();
    }
};

}  // namespace Utility

}  // namespace UnderStory
