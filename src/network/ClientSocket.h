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

#include "IPayloadSender.h"
#include "IPayloadReceiver.h"
#include "IClientImpl.h"
#include "IPayloadProcessor.h"

#include "asio/time_t_timer.hpp"

namespace UnderStory {

namespace Network {

class ClientSocket : public tcp::socket, public IPayloadReceiver<>, public IPayloadSender<>, public IClientImpl, public IPayloadProcessor<> {
 public:
    ClientSocket(asio::io_context &context, const char * name, const std::string &host, unsigned short port);

 private:
    SQueue _outgoingQueue;
    RQueue _incomingQueue;

    asio::io_context& _io_context;
    const char* _name;

    void _onError(const std::error_code &ec);

    void _asyncSendPayload(const RawPayload &payload) final;

    time_t_timer _hbTimer;
    void _sendHeartbeats();
};

}   // namespace Network

}   // namespace UnderStory
