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
#include <set>
#include <memory>
#include <utility>
#include <queue>

#include "src/network/SocketHelper.hpp"

namespace UnderStory {

namespace Network {

class USClient : public SocketHelper {
 public:
    SocketCallbacks callbacks;

    USClient(asio::io_context &context, const std::string &host, unsigned short port = UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT)
        : SocketHelper(context, this->callbacks), _io_context(context) {
        // resolve host
        tcp::resolver resolver(context);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        // on connection
        asio::async_connect(this->socket(), endpoints, [&](std::error_code ec, tcp::endpoint endpoint){
            if(ec) return this->_onError(ec);

            spdlog::info("UnderStory client connected to {}", endpoint.address().to_string());
            this->start();
        });
    }

    void connectAs(const std::string &userName) {
        // define handshake
        Handshake hsIn;
            auto currentVersion = new std::string(APP_CURRENT_VERSION);
            hsIn.set_allocated_client_version(currentVersion);

            auto username = new std::string(userName);
            hsIn.set_allocated_username(username);

        // send
        asio::post(this->_io_context, [this, hsIn]() {
            auto payload = serialize(hsIn);
            this->sendPayload(payload);
        });
    }

 private:
    asio::io_context& _io_context;

    void _onError(const std::error_code &ec) override {
        spdlog::error("Client error : {}", ec.message());
        SocketHelper::_onError(ec);
    }
};

}   // namespace Network

}   // namespace UnderStory
