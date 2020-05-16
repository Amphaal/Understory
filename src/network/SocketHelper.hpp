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

#include <spdlog/spdlog.h>

#include <string>
#include <memory>
#include <utility>
#include <future>

#include "src/models/User.pb.h"

#include <asio.hpp>
using asio::ip::tcp;

#include "src/core/Defaults.hpp"

namespace UnderStory {

namespace Network {

enum class PayloadType {
    PING = 0,
    HANDSHAKE = 1
};

struct RawPayload {
    PayloadType type;
    std::string bytes;
};

class SocketHelper {
 public:
    explicit SocketHelper(tcp::socket socket) : _socket(std::move(socket)) {}
    explicit SocketHelper(asio::io_context &context) : _socket(context) {}

 protected:
    virtual void _onError(const std::error_code &ec) {
        this->_socket.close();
    }
    virtual void _handlePayload(const RawPayload &payload) = 0;

 private:
    tcp::socket _socket;
    RawPayload* _readBufferPayload = nullptr;

    void _sendPayload(const RawPayload &payload) {
        asio::async_write(this->_socket,
            asio::buffer(payload.bytes),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                // TODO register upload speed
        });
    }

    void _receivePayloadType() {
        // new buffer
        this->_readBufferPayload = new RawPayload;

        // get payload type
        asio::async_read(this->_socket,
            asio::buffer(&this->_readBufferPayload->type, sizeof(this->_readBufferPayload->type)),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                this->_receivePayloadBytes();
                // TODO register download speed
        });
    }

    void _receivePayloadBytes() {
        asio::async_read(this->_socket,
            asio::buffer(this->_readBufferPayload->bytes),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                // handle payload
                this->_handlePayload(*this->_readBufferPayload);
                delete this->_readBufferPayload;
                this->_readBufferPayload = nullptr;

                // receive new payload
                this->_receivePayloadType();
                // TODO register download speed
        });
    }
};

}   // namespace Network

}   // namespace UnderStory
