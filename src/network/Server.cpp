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

#include "Server.h"

UnderStory::Network::Server::Server(Context &appContext, asio::io_context &context, const char* name, unsigned short port) : 
    IPayloadProcessor(&_incomingQueue),
    _appContext(appContext),
    _prefix(name),
    _port(port), 
    _acceptor(context, tcp::endpoint(tcp::v4(), port)) {
        // log
        spdlog::info("[{}] Listening on port {}", _prefix, _port);
        
        // begin
        _acceptConnections();
    }

void UnderStory::Network::Server::_acceptConnections() {
    this->_acceptor.async_accept([&](std::error_code ec, tcp::socket socket) {
        // define sock id
        _spawnCount++;

        // define name
        std::string sockName = _prefix;
        sockName += "_Sock";
        sockName += std::to_string(_spawnCount);
    
        // if error code
        if(ec) {
            spdlog::error("[{}] Issue while accepting connection {} !", _prefix, sockName.c_str());
        } else {
            // if OK
            auto sock_ptr = std::make_shared<SpawnedSocket>(
                std::move(socket),
                sockName,
                this->_spawnCount,
                &this->_incomingQueue
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
