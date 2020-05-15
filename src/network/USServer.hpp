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

namespace UnderStory {

namespace Server {

using ClientSockets = std::set<ClientSocket*>;

class ClientSocket : public std::enable_shared_from_this<ClientSocket> {
 public:
    ClientSocket(tcp::socket socket, ClientSockets* sockets) : _socket(std::move(socket)), _clientSockets(sockets) {
        this->_clientSockets->insert(this);

        asio::async_read(
            this->_socket,
            asio::buffer(this->_packageBufferPtr, UnderStory::Defaults::MAXIMUM_BYTES_AS_NETWORK_BUFFER),
            _onSocketRead
        );
    }

    void _onSocketRead(std::error_code ec, std::size_t length) {
        if(ec) return this->_onSocketError(ec);

        // TODO
    }

    void _onSocketError(std::error_code ec) {
        delete this;
    }

    ~ClientSocket() {
        this->_clientSockets->erase(this);
    }

 private:
    tcp::socket _socket;
    ClientSockets* _clientSockets = nullptr;
    void* _packageBufferPtr = nullptr;
};

class USServer {
 public:
    USServer() { }

    std::future<void> startAsync() {
        return std::async(std::launch::async, &USServer::start, this);
    }

    void start() {
        asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), UnderStory::Defaults::UPNP_DEFAULT_TARGET_PORT);
        this->_acceptor = new tcp::acceptor(io_context, endpoint);
        this->_acceptConnections();

        spdlog::debug("UnderStory server listening on {}", UnderStory::Defaults::connectionAddress("*"));
        io_context.run();
    }

 private:
    tcp::acceptor * _acceptor = nullptr;
    ClientSockets _clientSockets;

    void _onNewConnection(std::error_code ec, tcp::socket socket) {
        if(!ec) {
            new ClientSocket(std::move(socket), &this->_clientSockets);
        }

        // accept next connections anyway ?
        this->_acceptConnections();
    }

    void _acceptConnections() {
        this->_acceptor->async_accept(_onNewConnection);
    }
};

}   // namespace Server

}   // namespace UnderStory
