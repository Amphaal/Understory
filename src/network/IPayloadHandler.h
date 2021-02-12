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

// MUST declare winsock2 and windows.h to ensure appropriate behavior and prevent segfaults from asio
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif
#include <asio.hpp>
using asio::ip::tcp;

#include "PayloadLogger.h"

namespace UnderStory {

namespace Network {

template<class T>
class IPayloadHandler : public PayloadLogger {
 public:
    IPayloadHandler(const char* socketName, tcp::socket* socket, AtomicQueue<T>* payloadQueue);

 protected:
    tcp::socket* _socketRef;
    const char* _socketName;
    AtomicQueue<T>* _payloadQueue;

    T _buf;
    size_t _bufContentOffset = 0;
    std::atomic<bool> _isProcessing = false;
};

}   // namespace Network

}   // namespace UnderStory
