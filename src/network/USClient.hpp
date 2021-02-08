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

#include "ClientBase.hpp"

namespace UnderStory {

namespace Network {

class USClient : private ClientBase {
 public:
    USClient(
        asio::io_context &context,
        const char * name,
        const std::string &host,
        unsigned short port = UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT
    ) : ClientBase(context, name, host, port) {}

    // async init handshake command
    void initiateHandshake(const std::string &userName) {
        // define handshake
        Handshake hsIn;
            hsIn.set_client_version(APP_CURRENT_VERSION);
            hsIn.set_username(userName);

        // serialize
        auto payload = Marshaller::serialize(hsIn);

        // send
        this->_asyncSendPayload(payload);
    }
};

}   // namespace Network

}   // namespace UnderStory
