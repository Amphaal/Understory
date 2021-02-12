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

#include "IClientImpl.h"
#include "Marshaller.h"

#include "src/base/Defaults.hpp"

#include "src/models/Heartbeat.pb.h"

void UnderStory::Network::IClientImpl::initiateHandshake(const std::string &userName) {
    // define handshake
    Handshake hsIn;
        hsIn.set_client_version(APP_CURRENT_VERSION);
        hsIn.set_username(userName);

    // serialize
    auto payload = Marshaller::serialize(hsIn);

    // send
    this->_asyncSendPayload(payload);
}

void UnderStory::Network::IClientImpl::sendHeartbeat() {
    //
    Heartbeat hb;
    auto ts = std::chrono::system_clock::now().time_since_epoch().count();
    hb.set_ts(ts);

    // serialize
    auto payload = Marshaller::serialize(hb);

    // send
    this->_asyncSendPayload(payload);
}
