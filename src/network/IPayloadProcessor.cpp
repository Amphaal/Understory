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

#include "IPayloadProcessor.h"

#include "SpawnedSocket.h"

template<class T>
UnderStory::Network::IPayloadProcessor<T>::IPayloadProcessor(AtomicQueue<T>* queue) : _queue(queue) {}

template<class T>
void UnderStory::Network::IPayloadProcessor<T>::processPayloadQueue(const Processor &processor) {
    std::mutex m;
    std::unique_lock<std::mutex> lock(m);

    //
    while(true) {
        //
        _queue->waitToBeFilled(lock);
        auto payloads = _queue->popIntoQueue();

        //
        while(!payloads.empty()) {
            // 
            auto shouldBreak = processor(payloads.front());
            if(shouldBreak) return;

            //
            payloads.pop();
        }
    }
}

template class UnderStory::Network::IPayloadProcessor<UnderStory::Network::SpawnedSocket::RPayload>;
template class UnderStory::Network::IPayloadProcessor<UnderStory::Network::RawPayload>;
