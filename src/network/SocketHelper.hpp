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

#include "src/core/Defaults.hpp"

namespace UnderStory {

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
    SocketHelper(const std::string &addressWithoutPort, int ZMQSocketType, bool mustBind = true) {
        this->_boundAddress = UnderStory::Defaults::connectionAddress(addressWithoutPort);

        // start router
        auto context = zmq_ctx_new();
        this->_socket = zmq_socket(context, ZMQSocketType);
        auto bindOrConnect = mustBind ? zmq_bind : zmq_connect;
        auto rc = bindOrConnect(this->_socket, this->_boundAddress.c_str());
        assert(rc == 0);
    }

    const char* boundAddress() const {
        return this->_boundAddress.c_str();
    }

    const void* socket() const {
        return this->_socket;
    }

    void sendPayload(const UnderStory::RawPayload &payload) {
        this->_sendPayloadType(payload);
        this->_sendPayloadBytes(payload);
    }

    const UnderStory::RawPayload waitForRawPayload() const {
        return this->_handleIncomingAsRawPayload();
    }

 private:
    std::string _boundAddress;
    void* _socket = nullptr;
    static inline int _maximumZMQMessageSizeInBytes = 64000;

    void _sendPayloadBytes(const UnderStory::RawPayload &payload) const {
        // check if has payload bytes
        auto bytesLen = payload.bytes.length();
        if(!bytesLen) return;

        // prepare
        auto submsgs = ceil(static_cast<double>(bytesLen) / _maximumZMQMessageSizeInBytes);
        auto lastMsgSize = bytesLen % _maximumZMQMessageSizeInBytes;
        auto index = payload.bytes.c_str();
        assert(submsgs);

        // iterate
        while(submsgs) {
            auto isLastMessage = submsgs == 1;
            size_t messageSize = isLastMessage && lastMsgSize ? lastMsgSize : _maximumZMQMessageSizeInBytes;

            // build message
            zmq_msg_t msg;
            auto rc = zmq_msg_init_size(&msg, messageSize);
            assert (rc == 0);

            // create sub-buffer
            auto data = malloc(messageSize);
            memcpy(data, index, messageSize);

            // init data
            zmq_msg_init_data(
                &msg,
                data,
                messageSize,
                [](void *data, void *hint) {
                    free(data);
                },
                nullptr
            );

            // send
            rc = zmq_msg_send(&msg, this->_socket, !isLastMessage ? ZMQ_SNDMORE : 0);
            assert(messageSize == rc);

            // go to next message
            submsgs--;
            index += messageSize;
        }
    }

    void _sendPayloadType(const UnderStory::RawPayload &payload) {
        auto payloadType = new int(static_cast<int>(payload.type));
        auto expectedSize = sizeof(&payloadType);

        // build message
        zmq_msg_t msg;
        auto rc = zmq_msg_init_size(&msg, expectedSize);
        assert(rc == 0);

        // init data
        zmq_msg_init_data(
            &msg,
            payloadType,
            expectedSize,
            [](void *data, void *hint) {
                free(data);
            },
            nullptr
        );

        // send
        rc = zmq_msg_send(&msg, this->_socket, ZMQ_SNDMORE);
        assert(expectedSize == rc);
    }

    const UnderStory::RawPayload _handleIncomingAsRawPayload() const {
        RawPayload payload;

        // iterate to build a payload
        int64_t more;
        size_t more_size = sizeof(more);
        size_t count = 0;
        do {
            /* Create an empty ØMQ message to hold the message part */
            zmq_msg_t part;
            int rc = zmq_msg_init(&part);
            assert(rc == 0);

            /* Block until a message is available to be received from socket */
            rc = zmq_msg_recv(&part, this->_socket, 0);
            assert(rc != -1);

            /* Determine if more message parts are to follow */
            rc = zmq_getsockopt(this->_socket, ZMQ_RCVMORE, &more, &more_size);
            assert(rc == 0);

            // content
            auto partData = zmq_msg_data(&part);
            auto partSize = zmq_msg_size(&part);

            if(!count) {   // identity
                // do nothing ?
            } else if(count == 1) {   // append PayloadType
                int type = -1;
                std::memcpy(&type, partData, partSize);
                payload.type = static_cast<PayloadType>(type);
            } else {   // append content
                std::string part(reinterpret_cast<char*>(partData), partSize);
                payload.bytes += part;
            }

            // iterate
            count++;

            // dispose
            zmq_msg_close (&part);
        } while (more);

        return payload;
    }
};

}   // namespace UnderStory

