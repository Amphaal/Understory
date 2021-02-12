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

#include "SpawnedSocket.h"
#include "IPayloadProcessor.h"

#include "src/base/Context.hpp"

namespace UnderStory {

namespace Network {

class Server : public IPayloadProcessor<SpawnedSocket::RPayload> {
 public:
    Server(Context &appContext, asio::io_context &context, const char* name, unsigned short port);

 private:
    Context _appContext;
    unsigned short _port;
    int _spawnCount = 0;
    const std::string _prefix;

    tcp::acceptor _acceptor;
    std::list<std::shared_ptr<SpawnedSocket>> _spawnedSockets;
    SpawnedSocket::RQueue _incomingQueue;

    void _acceptConnections();
};

}   // namespace Network

}   // namespace UnderStory
