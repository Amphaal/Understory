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

#include "PayloadLogger.h"

#include <chrono>

UnderStory::Network::PayloadLogger::PayloadLogger() : _traceBuf{0} {}

void UnderStory::Network::PayloadLogger::logNewPayload() {
    _traceBuf.payloadId++;
    _traceBuf.startedAt = _epochNow();
}

UnderStory::Network::PayloadTrace::tp UnderStory::Network::PayloadLogger::_epochNow() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}

void UnderStory::Network::PayloadLogger::logPayloadType(PayloadType payloadType) {
    _traceBuf.payloadType = payloadType;
}

void UnderStory::Network::PayloadLogger::logPayloadSize(size_t expectedSize) {
    _traceBuf.expectedSize = expectedSize;
}

void UnderStory::Network::PayloadLogger::commitPayloadSpeed(size_t loaded) {
    speedQueue.push({
        _traceBuf.payloadId,
        _epochNow(),
        loaded
    });
}

void UnderStory::Network::PayloadLogger::commitLog() {
    _traceBuf.endedAt = _epochNow();
    tracesQueue.push(_traceBuf);
}
