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

#include <asio.hpp>
using asio::ip::tcp;

#include "src/core/Defaults.hpp"

#include "src/models/User.pb.h"

namespace UnderStory {

namespace Network {

enum class PayloadType {
    HANDSHAKE = 0
};

struct RawPayload {
    PayloadType type;
    size_t bytesSize;
    std::string bytes;
};

class Marshaller {
 public:
    static RawPayload serialize(const Handshake &handshake) {
        return _serialize(handshake, PayloadType::HANDSHAKE);
    }

 private:
    static RawPayload _serialize(const google::protobuf::Message &protobufMsg, const PayloadType &type) {
        RawPayload payload;
        payload.type = type;
        protobufMsg.SerializePartialToString(&payload.bytes);
        payload.bytesSize = payload.bytes.length();
        return payload;
    }
};

class SocketHelper : public Marshaller {
 public:
    explicit SocketHelper(tcp::socket socket) : _socket(std::move(socket)) {}
    explicit SocketHelper(asio::io_context& context) : _socket(context) {}

    virtual void start() {
        this->_receivePayloadType();
    }

    void sendPayload(const RawPayload &payload) {
        this->_sendPayloadType(payload);
    }

 protected:
    virtual void _onError(const std::error_code &ec) {
        this->_socket.close();
    }
    virtual void _handlePayload(const RawPayload &payload) = 0;

    tcp::socket& socket() {
        return this->_socket;
    }

 private:
    tcp::socket _socket;
    RawPayload* _rBuf = nullptr;

    void _sendPayloadType(const RawPayload &payload) {
        asio::async_write(this->_socket,
            asio::buffer(&payload.type, sizeof(payload.type)),
            [this, payload](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                this->_sendPayloadBytesSize(payload);
        });
    }

    void _sendPayloadBytesSize(const RawPayload &payload) {
        asio::async_write(this->_socket,
            asio::buffer(&payload.bytesSize, sizeof(payload.bytesSize)),
            [this, payload](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                this->_sendPayloadBytes(payload);
        });
    }

    void _sendPayloadBytes(const RawPayload &payload) {
        asio::async_write(this->_socket,
            asio::buffer(payload.bytes),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);
                // TODO register upload speed
        });
    }

    void _receivePayloadType() {
        // new buffer
        this->_rBuf = new RawPayload;

        // get payload type
        asio::async_read(this->_socket,
            asio::buffer(&this->_rBuf->type, sizeof(this->_rBuf->type)),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                this->_receivePayloadBytesSize();
        });
    }

    void _receivePayloadBytesSize() {
        asio::async_read(this->_socket,
            asio::buffer(&this->_rBuf->bytesSize, sizeof(this->_rBuf->bytesSize)),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                // resize bytes container
                this->_rBuf->bytes.resize(this->_rBuf->bytesSize);
                this->_receivePayloadBytes();
        });
    }

    void _receivePayloadBytes() {
        asio::async_read(this->_socket,
            asio::buffer(this->_rBuf->bytes),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                // handle payload
                this->_handlePayload(*this->_rBuf);
                delete this->_rBuf;
                this->_rBuf = nullptr;

                // receive new payload
                this->_receivePayloadType();
                // TODO register download speed
        });
    }
};

}   // namespace Network

}   // namespace UnderStory
