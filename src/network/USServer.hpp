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

namespace Server {

class ClientSocket : public SocketHelper, public std::enable_shared_from_this<ClientSocket> {
 public:
    ClientSocket(tcp::socket socket, std::set<std::shared_ptr<ClientSocket>> &clientSockets)
        : SocketHelper(std::move(socket)), _clientSockets(clientSockets) { }

    void start() override {
        this->_clientSockets.insert(shared_from_this());
        SocketHelper::start();
    }

 private:
    std::set<std::shared_ptr<ClientSocket>> &_clientSockets;

    void _onError(const std::error_code &ec) override {
        SocketHelper::_onError(ec);
        this->_clientSockets.erase(shared_from_this());
    }

    void _handlePayload(const RawPayload &payload) override {
        switch(payload.type) {
            case PayloadType::HANDSHAKE: {
                Handshake handshake;
                handshake.ParseFromString(payload.bytes);
            }
            break;
        }
    };
};

class USServer {
 public:
    explicit USServer(asio::io_context &context, unsigned short port = UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT)
        : _acceptor(context, tcp::endpoint(tcp::v4(), port)) {
        spdlog::debug("UnderStory server listening on port {}", port);
        this->_acceptConnections();
    }

 private:
    tcp::acceptor _acceptor;
    std::set<std::shared_ptr<ClientSocket>> _clientSockets;

    void _acceptConnections() {
        this->_acceptor.async_accept(
            [&](std::error_code ec, tcp::socket socket) {
                if(!ec) {
                    auto client = std::make_shared<ClientSocket>(std::move(socket), this->_clientSockets);
                    client->start();
                }
                this->_acceptConnections();
        });
    }
};

}   // namespace Server

}   // namespace Network

}   // namespace UnderStory
