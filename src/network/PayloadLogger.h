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

#include "AtomicQueue.h"
#include "Payloads.h"

namespace UnderStory {

namespace Network {

struct PayloadTrace {
   using tp = int64_t; 

   int payloadId;
   tp startedAt;
   tp endedAt;
   PayloadType payloadType;
   size_t expectedSize;
};

struct NetworkLoadSpeedTrace {
   int payloadId;
   PayloadTrace::tp doneAt;
   size_t bytesExchanged;
};

class PayloadLogger {
 public:
    using tp = int64_t; 
    PayloadLogger();

    AtomicQueue<PayloadTrace>           tracesQueue;
    AtomicQueue<NetworkLoadSpeedTrace>  speedQueue;

    void logNewPayload();
    void logPayloadType(PayloadType payloadType);
    void logPayloadSize(size_t expectedSize);
   
    void commitPayloadSpeed(size_t loaded);
    void commitLog();

 private:
    PayloadTrace _traceBuf;

    static PayloadTrace::tp _epochNow();
};

}   // namespace Network

}   // namespace UnderStory
