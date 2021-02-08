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

#include "src/network/PayloadableSocket.hpp"

namespace UnderStory {

namespace Network {

class ClientBase : public PayloadableSocket {
 public:
    ClientBase(
        asio::io_context &context,
        const char * name,
        const std::string &host,
        unsigned short port
    ) : PayloadableSocket(&context, &_queues, name), _io_context(context) {
        // resolve host
        tcp::resolver resolver(context);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        // on connection
        asio::async_connect(
            this->socket(), 
            endpoints, 
            [&](std::error_code ec, tcp::endpoint endpoint){
                // on error...
                if(ec) 
                    return this->_onError(ec);

                // start receiving
                spdlog::info("Client [{}] connected to {}", name, endpoint.address().to_string());
                this->_startReceiving();
            }
        );
    }

 protected:
    void _asyncSendPayload(const RawPayload &payload) {
        asio::post(this->_io_context, [this, &payload]() {
            this->_sendPayload(payload);
        });
    }

 private:
    asio::io_context& _io_context;
    NetworkQueues _queues;
    const char* name;

    void _onError(const std::error_code &ec) {
        spdlog::error("Client [{}] error >> {}", name, ec.message());
    }
};

}   // namespace Network

}   // namespace UnderStory
