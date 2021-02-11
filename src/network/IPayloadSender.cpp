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

#include "IPayloadSender.h"

#include <spdlog/spdlog.h>
#include <magic_enum.hpp>

#include "src/base/Defaults.hpp"

template<class T>
UnderStory::Network::IPayloadSender<T>::IPayloadSender(const char* socketName, tcp::socket* socket, SQueue* senderQueue) : 
    IPayloadHandler<T>(socketName, socket, senderQueue) {}

template<class T>
void UnderStory::Network::IPayloadSender<T>::_sendPayload(const T& payload) {
    // add ptr to queue
    this->_payloadQueue->push(payload);
    
    // if already processing, let async handle 
    if(this->_isProcessing) return;

    // if not, start sending from queue
    this->_sendPayloadType();
}

template<class T>
void UnderStory::Network::IPayloadSender<T>::_onPayloadSendError(const std::error_code &ec, const char* partStr) {
    spdlog::warn("[{}] Error while writing \"{}\" section", 
        this->_socketName, 
        partStr
    );
}

template<class T>
void UnderStory::Network::IPayloadSender<T>::_onPayloadBytesUploaded(PayloadType type, size_t uploaded, size_t total) {
    spdlog::info("[{}] Uploading \"{}\" payload [{}/{}]...", 
        this->_socketName,
        magic_enum::enum_name(type),
        uploaded,
        total
    );
}

template<class T>
void UnderStory::Network::IPayloadSender<T>::_sendPayloadType() {
    // copy shared ptr from queue
    this->_buf = this->_payloadQueue->front();

    // log
    this->_log.increment();

    // async write payload type
    asio::async_write(*this->_socketRef,
        asio::buffer(&this->_buf.type, sizeof(this->_buf.type)),
        [this](std::error_code ec, std::size_t length) {
            // if error...
            if(ec) 
                return this->_onPayloadSendError(ec, "PAYLOAD_TYPE");

            // continue...
            this->_sendPayloadBytesSize();
    });
}

template<class T>
void UnderStory::Network::IPayloadSender<T>::_sendPayloadBytesSize() {
    asio::async_write(*this->_socketRef,
        asio::buffer(&this->_buf.bytesSize, sizeof(this->_buf.bytesSize)),
        [this](std::error_code ec, std::size_t length) {
            // if error...
            if(ec) 
                return this->_onPayloadSendError(ec, "PAYLOAD_SIZE");

            // continue...
            this->_sendPayloadBytes();
    });
}

template<class T>
void UnderStory::Network::IPayloadSender<T>::_sendPayloadBytes() {
    // determine how many bytes to write
    auto bytesToWrite = std::min(
        this->_buf.bytesSize - this->_bufContentOffset,
        Defaults::MAXIMUM_BYTES_AS_NETWORK_BUFFER
    );

    // write
    asio::async_write(*this->_socketRef,
        asio::buffer(this->_buf.bytes.c_str() + this->_bufContentOffset, bytesToWrite),
        [this](std::error_code ec, std::size_t length) {
            // if error...
            if(ec) 
                return this->_onPayloadSendError(ec, "PAYLOAD_CONTENT");

            // increment buffer offset
            this->_bufContentOffset += length;

            // callback
            this->_onPayloadBytesUploaded(this->_buf.type, length, this->_buf.bytesSize);

            // keep sending while offset is not at the end
            if(this->_bufContentOffset < this->_buf.bytesSize) {
                return this->_sendPayloadBytes();
            }

            // reset offset
            this->_bufContentOffset = 0;

            // pop from queue, unref
            auto isQueueEmpty = this->_payloadQueue->pop();

            // if payloads are still in queue, handle them
            if(!isQueueEmpty)
                this->_sendPayloadType();
    });
}

template class UnderStory::Network::IPayloadSender<UnderStory::Network::RawPayload>;
