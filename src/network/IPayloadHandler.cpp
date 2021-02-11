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

#include "IPayloadHandler.h"

#include "Marshaller.h"

void UnderStory::Network::PayloadLog::increment() {
    _payloadIdCount++;
    _latestPayloadTP = std::chrono::system_clock::now();
}

int UnderStory::Network::PayloadLog::id() const {
    return _payloadIdCount;
}

std::chrono::system_clock::time_point UnderStory::Network::PayloadLog::started() const {
    return _latestPayloadTP;
}

template<class T>
UnderStory::Network::IPayloadHandler<T>::IPayloadHandler(const char* socketName, tcp::socket* socket, AtomicQueue<T>* payloadQueue) : 
    _socketName(socketName), 
    _socketRef(socket), 
    _payloadQueue(payloadQueue) { }

template class UnderStory::Network::IPayloadHandler<UnderStory::Network::RawPayload>;
template class UnderStory::Network::IPayloadHandler<UnderStory::Network::SpawnedRawPayload>;
