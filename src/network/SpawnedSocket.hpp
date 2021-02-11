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

#include "IPayloadSender.hpp"
#include "IPayloadReceiver.hpp"

namespace UnderStory {

namespace Network {

class ISpawnedPayloadReceiver : public IPayloadReceiver<SpawnedRawPayload> {
 public:
    ISpawnedPayloadReceiver(const char* socketName, tcp::socket* socket, RQueue* receiverQueue, int spawnId) : 
        IPayloadReceiver<SpawnedRawPayload>(socketName, socket, receiverQueue) {
            this->_buf.spawnId = spawnId;
        }
};

class NamedSpawnedSocket {
 public:
    NamedSpawnedSocket(const std::string &name) : _sName(name) {}
 protected:
    const std::string _sName;
};

class SpawnedSocket : public tcp::socket, public NamedSpawnedSocket, public ISpawnedPayloadReceiver, public IPayloadSender<> {
 public:
    SpawnedSocket(tcp::socket &&socket, const std::string &name, int spawnId, RQueue* incomingQueue) : 
        tcp::socket(std::move(socket)),
        NamedSpawnedSocket(name),
        ISpawnedPayloadReceiver(this->_sName.c_str(), this, incomingQueue, spawnId),
        IPayloadSender<>       (this->_sName.c_str(), this, &this->_outgoingQueue) {}

    void start() {
        this->_startReceiving();
        spdlog::info("[{}] Client logged to server !", _sName.c_str());
    }

 private:
    SQueue _outgoingQueue;
};

}   // namespace Network

}   // namespace UnderStory
