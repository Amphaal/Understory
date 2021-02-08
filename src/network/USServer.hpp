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

#include <string>
#include <list>
#include <memory>
#include <utility>

#include <asio.hpp>
using asio::ip::tcp;

#include "src/network/PayloadableSocket.hpp"

#include "src/base/Context.hpp"

namespace UnderStory {

namespace Network {

namespace Server {

class SpawnedSocket : protected PayloadableSocket {
 public:
    SpawnedSocket(tcp::socket socket, const std::string &name) : 
        PayloadableSocket(std::move(socket), &_queues, name.c_str()), _name(name) {}

    void start() {
        this->_startReceiving();
    }

 private:
    NetworkQueues _queues;
    const std::string _name;
};

class USServer {
 public:
    explicit USServer(
        Context &appContext,
        asio::io_context &context,
        const char* name,
        unsigned short port = UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT
    ) : _prefix(name),
        _port(port), 
        _appContext(appContext), 
        _acceptor(context, tcp::endpoint(tcp::v4(), port)) {}

    void start() {
        // begin
        _acceptConnections();

        // log
        spdlog::info("Server listening on port {}", _port);
    } 

 private:
    tcp::acceptor _acceptor;
    std::list<std::shared_ptr<SpawnedSocket>> _spawnedSockets;
    Context _appContext;
    unsigned short _port;
    int _cliCount = 0;
    const std::string _prefix;

    void _acceptConnections() {
        this->_acceptor.async_accept(
            [&](std::error_code ec, tcp::socket socket) {
                // define sock name
                _cliCount++;
                std::string sockName = _prefix;
                sockName += "_Sock";
                sockName += std::to_string(_cliCount);
            
                // if error code
                if(ec) {
                    spdlog::error("[{}] issue while accepting connection {} !", _prefix, sockName.c_str());
                } else {
                   // if OK
                    auto sock_ptr = std::make_shared<SpawnedSocket>(
                        std::move(socket),
                        sockName
                    );

                    // add to list
                    _spawnedSockets.push_back(sock_ptr);

                    // listen to client 
                    sock_ptr->start();
                }

                // then accept connexions again
                this->_acceptConnections();
        });
    }
};

}   // namespace Server

}   // namespace Network

}   // namespace UnderStory
