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

#include <asio.hpp>
using asio::ip::tcp;

#include "src/network/SocketHelper.hpp"

namespace UnderStory {

namespace Network {

class USClient {
 public:
    USClient(asio::io_context &context, const std::string &host, unsigned short port = UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT)
        : _io_context(context), _socket(context) {
            // resolve host
            tcp::resolver resolver(context);
            auto endpoints = resolver.resolve(host, std::to_string(port));

            // on connection
            asio::async_connect(this->_socket, endpoints, [&](std::error_code ec, tcp::endpoint endpoint){
                if(ec) return this->_onError(ec);
                spdlog::debug("UnderStory client connected to {}", endpoint.address().to_string());
                this->_readIncoming();
            });
        }

 private:
    asio::io_context& _io_context;
    tcp::socket _socket;

    void _onError(const std::error_code &ec) {
        spdlog::error("Client error : {}", ec.message());
    }

    void _readIncoming() {
        auto i = true;
    }

};

}   // namespace Network

}   // namespace UnderStory
