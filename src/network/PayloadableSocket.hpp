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

#include "IPayloadReceiver.hpp"
#include "IPayloadSender.hpp"

namespace UnderStory {

namespace Network {

struct NetworkQueues {
    AtomicQueue<RawPayload> incomingQueue;
    AtomicQueue<RawPayload> outgoingQueue;
};

class PayloadableSocket : public IPayloadReceiver<>, public IPayloadSender<> {
 public:
    // from socket itself
    explicit PayloadableSocket(tcp::socket socket, NetworkQueues* queues, const char* name) : 
        IPayloadReceiver<>(name, &_socket, &queues->incomingQueue), 
        IPayloadSender<>(name, &_socket, &queues->outgoingQueue),
        _socket(std::move(socket)) {}

    // from context
    explicit PayloadableSocket(asio::io_context* context, NetworkQueues* queues, const char* name) : 
        IPayloadReceiver(name, &_socket, &queues->incomingQueue), 
        IPayloadSender(name, &_socket, &queues->outgoingQueue),
        _socket(*context) {}

 protected:
    tcp::socket& socket() {
        return this->_socket;
    }

 private:
    tcp::socket _socket;
};

}   // namespace Network

}   // namespace UnderStory
