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

#include <string>

#include "SocketHelper.hpp"

namespace UnderStory {

class USServer : public SocketHelper {
 public:
    explicit USServer(const std::string &addressWithoutPort) : SocketHelper(addressWithoutPort, ZMQ_ROUTER) {
        spdlog::debug("UnderStory server listening on {}", this->boundAddress());
    }

    std::future<void> runAsync() {
        return std::async(std::launch::async, &USServer::run, this);
    }

    void run() {
        // loop
        while (!this->_mustShutdown) {
            auto payload = this->waitForRawPayload();
            // TODO handle
        }
    }

 private:
    bool _mustShutdown = false;
};

}   // namespace UnderStory
