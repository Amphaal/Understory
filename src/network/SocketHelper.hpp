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
#include <algorithm>
#include <future>

#include <asio.hpp>
using asio::ip::tcp;

#include "src/base/Defaults.hpp"

#include "src/models/User.pb.h"

namespace UnderStory {

namespace Network {

enum class PayloadType {
    UNKNOWN = 0,
    HANDSHAKE = 1
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

using OnPayloadReceivedCallback = std::function<void(const RawPayload&)>;
using OnBytesUploadedCallback = std::function<void(size_t)>;
using OnBytesDownloadedCallback = std::function<void(size_t)>;
struct SocketCallbacks {
    OnPayloadReceivedCallback onPayloadReceived;
    OnBytesUploadedCallback onBytesUploaded;
    OnBytesDownloadedCallback onBytesDownloaded;
};

class SocketHelper : public Marshaller {
 public:
    explicit SocketHelper(tcp::socket socket, SocketCallbacks &cb) : _callbacks(cb), _socket(std::move(socket)) {}
    explicit SocketHelper(asio::io_context& context, SocketCallbacks &cb) : _callbacks(cb), _socket(context) {}

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

    tcp::socket& socket() {
        return this->_socket;
    }

 private:
    tcp::socket _socket;
    RawPayload* _rBuf = nullptr;
    size_t _rOffset = 0;
    size_t _wOffset = 0;
    SocketCallbacks& _callbacks;

    void _sendPayloadType(const RawPayload &payload) {
        // cast to int to prevent random behavior on tests
        auto type = static_cast<int>(payload.type);

        asio::async_write(this->_socket,
            asio::buffer(&type, sizeof(type)),
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
        // determine how many bytes to write
        auto bytesToWrite = std::min(
            payload.bytesSize - this->_wOffset,
            Defaults::MAXIMUM_BYTES_AS_NETWORK_BUFFER
        );

        // write
        asio::async_write(this->_socket,
            asio::buffer(payload.bytes.c_str() + this->_wOffset, bytesToWrite),
            [this, payload](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                // increment buffer offset
                this->_wOffset += length;

                // callback
                if(this->_callbacks.onBytesUploaded) {
                    this->_callbacks.onBytesUploaded(length);
                }

                // keep sending while offset is not at the end
                if(this->_wOffset < payload.bytesSize) {
                    return this->_sendPayloadBytes(payload);
                }

                // reset offset
                this->_wOffset = 0;
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
        // determine how many bytes to write
        auto bytesToRead = std::min(
            this->_rBuf->bytesSize - this->_rOffset,
            Defaults::MAXIMUM_BYTES_AS_NETWORK_BUFFER
        );

        // read
        asio::async_read(this->_socket,
            asio::buffer(this->_rBuf->bytes.begin().base() + this->_rOffset, bytesToRead),
            [this](std::error_code ec, std::size_t length) {
                if(ec) return this->_onError(ec);

                // increment buffer offset
                this->_rOffset += length;

                // callback
                if(this->_callbacks.onBytesDownloaded) {
                    this->_callbacks.onBytesDownloaded(length);
                }

                // keep sending while offset is not at the end
                if(this->_rOffset < this->_rBuf->bytesSize) {
                    return this->_receivePayloadBytes();
                }

                // callback
                if(this->_callbacks.onPayloadReceived) {
                    this->_callbacks.onPayloadReceived(*this->_rBuf);
                }

                // clear buffer
                delete this->_rBuf;
                this->_rBuf = nullptr;
                this->_rOffset = 0;

                // restart : wait to receive the next payload
                this->_receivePayloadType();
        });
    }
};

}   // namespace Network

}   // namespace UnderStory
