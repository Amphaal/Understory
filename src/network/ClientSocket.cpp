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

#include "ClientSocket.h"

#include "src/base/Defaults.hpp"

#include <spdlog/spdlog.h>

UnderStory::Network::ClientSocket::ClientSocket(asio::io_context &context, const char * name, const std::string &host, unsigned short port) :
    tcp::socket(context), 
    IPayloadReceiver(name, this, &this->_incomingQueue),
    IPayloadSender(name, this, &this->_outgoingQueue),
    IPayloadProcessor(&this->_incomingQueue),
    _io_context(context),
    _hbTimer(context),
    _name(name) {
    // resolve host
    tcp::resolver resolver(context);
    auto endpoints = resolver.resolve(host, std::to_string(port));

    // log
    spdlog::info("[{}] Attempting connection to {}:{}...", 
        this->_name, 
        host,
        port
    );

    // on connection
    asio::async_connect(
        *this, 
        endpoints, 
        [this](std::error_code ec, tcp::endpoint endpoint){
            // on error...
            if(ec) 
                return this->_onError(ec);

            // start receiving
            this->_startReceiving();

            //
            this->_heartbeating();

            // log
            spdlog::info("[{}] Successfully connected to {}:{} !", 
                this->_name, 
                endpoint.address().to_string(),
                endpoint.port()
            );
        }
    );
}

void UnderStory::Network::ClientSocket::_heartbeating() {
    _hbTimer.expires_after(Defaults::HEARTBEAT_FREQUENCY);
    _hbTimer.async_wait([this](const std::error_code&){
          // send
          this->sendHeartbeat();

          // keep looping
          this->_heartbeating();
    });
}


void UnderStory::Network::ClientSocket::_onError(const std::error_code &ec) {
    spdlog::error("[{}] error >> {}", this->_name, ec.message());
}

void UnderStory::Network::ClientSocket::_asyncSendPayload(const RawPayload &payload) {
    asio::post(this->_io_context, [this, payload]() {
        this->_sendPayload(payload);
    });
}
