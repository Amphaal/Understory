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
#include <set>
#include <memory>
#include <utility>

#include <asio.hpp>
using asio::ip::tcp;

#include "src/network/SocketHelper.hpp"

#include "src/base/Context.hpp"

namespace UnderStory {

namespace Network {

namespace Server {

class ClientSocket : public SocketHelper {
 public:
    ClientSocket(tcp::socket socket, std::set<ClientSocket*> &clientSockets, SocketCallbacks &cb)
        : SocketHelper(std::move(socket), cb), _clientSockets(clientSockets) { }

    void start() override {
        this->_clientSockets.insert(this);
        SocketHelper::start();
    }

 private:
    std::set<ClientSocket*> &_clientSockets;

    void _onError(const std::error_code &ec) override {
        SocketHelper::_onError(ec);
        this->_clientSockets.erase(this);
    }
};

class USServer {
 public:
    SocketCallbacks callbacks;

    explicit USServer(Context &appContext, asio::io_context &context, unsigned short port = UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT)
        : _appContext(appContext), _acceptor(context, tcp::endpoint(tcp::v4(), port)) {
        spdlog::info("UnderStory server listening on port {}", port);
        this->_acceptConnections();
    }

 private:
    tcp::acceptor _acceptor;
    std::set<ClientSocket*> _clientSockets;
    Context _appContext;

    void _acceptConnections() {
        this->_acceptor.async_accept(
            [&](std::error_code ec, tcp::socket socket) {
                if(!ec) {
                    auto client = new ClientSocket(std::move(socket), this->_clientSockets, this->callbacks);
                    client->start();
                }
                this->_acceptConnections();
        });
    }
};

}   // namespace Server

}   // namespace Network

}   // namespace UnderStory
