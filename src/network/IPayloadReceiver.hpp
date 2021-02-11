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

#include <spdlog/spdlog.h>

#include "IPayloadHandler.hpp"

namespace UnderStory {

namespace Network {

template<class T = RawPayload>
class IPayloadReceiver : protected IPayloadHandler<T> {
 public:
    using RQueue = AtomicQueue<T>;
    IPayloadReceiver(const char* socketName, tcp::socket* socket, RQueue* receiverQueue) : 
        IPayloadHandler<T>(socketName, socket, receiverQueue) {}

    virtual void _onPayloadReceiveError(const std::error_code &ec, const char* partStr) {
        spdlog::warn("[{}] Error while reading \"{}\" section", 
            this->_socketName, 
            partStr
        );
    }

    virtual void _onPayloadBytesDownloaded(PayloadType type, size_t downloaded, size_t total) {
        spdlog::info("[{}] Downloading \"{}\" payload [{}/{}]...", 
            this->_socketName,
            magic_enum::enum_name(type),
            downloaded,
            total
        );
    }

    void _startReceiving() {
        //
        if(this->_isProcessing) return;

        //
        this->_isProcessing = true;
        this->_receivePayloadType();
    }

 private:
    void _receivePayloadType() {
        // async read payload type into RawPayload
        asio::async_read(*this->_socketRef,
            asio::buffer(&this->_buf.type, sizeof(this->_buf.type)),
            [this](std::error_code ec, std::size_t length) {
                // log
                this->_log.increment();

                // if error...
                if(ec) 
                    return this->_onPayloadReceiveError(ec, "PAYLOAD_TYPE");

                // continue
                this->_receivePayloadBytesSize();
        });
    }

    void _receivePayloadBytesSize() {
        // async read payload content size into RawPayload
        asio::async_read(*this->_socketRef,
            asio::buffer(&this->_buf.bytesSize, sizeof(this->_buf.bytesSize)),
            [this](std::error_code ec, std::size_t length) {
                // if error...
                if(ec) 
                    return this->_onPayloadReceiveError(ec, "PAYLOAD_SIZE");

                // resize bytes container
                this->_buf.bytes.resize(this->_buf.bytesSize);

                // continue...
                this->_receivePayloadBytes();
        });
    }

    void _receivePayloadBytes() {
        // determine how many bytes to write
        auto bytesToRead = std::min(
            this->_buf.bytesSize - this->_bufContentOffset,
            Defaults::MAXIMUM_BYTES_AS_NETWORK_BUFFER
        );

        // async read payload content into RawPayload
        asio::async_read(*this->_socketRef,
            asio::buffer(this->_buf.bytes.begin().base() + this->_bufContentOffset, bytesToRead),
            [this](std::error_code ec, std::size_t length) {
                // if error...
                if(ec) 
                    return this->_onPayloadReceiveError(ec, "PAYLOAD_CONTENT");

                // increment buffer offset
                this->_bufContentOffset += length;

                // callback
                this->_onPayloadBytesDownloaded(this->_buf.type, length, this->_buf.bytesSize);

                // keep sending while offset is not at the end
                if(this->_bufContentOffset < this->_buf.bytesSize) {
                    return this->_receivePayloadBytes();
                }

                // push to queue
                this->_payloadQueue->push(this->_buf);

                // clear offset
                this->_bufContentOffset = 0;

                // restart : wait to receive the next payload
                this->_receivePayloadType();
        });
    }
};

}   // namespace Network

}   // namespace UnderStory
