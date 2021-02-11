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

#include <string>

#include "src/models/User.pb.h"

namespace UnderStory {

namespace Network {

enum class PayloadType {
    UNKNOWN = 0,
    HANDSHAKE = 1
};

struct RawPayload {
    PayloadType type = PayloadType::UNKNOWN;
    size_t bytesSize = 0;
    std::string bytes;
};

struct SpawnedRawPayload : RawPayload {
    int spawnId = 0;
};

class Marshaller {
 public:
    static RawPayload serialize(const Handshake &handshake);

 private:
    static RawPayload _serialize(const google::protobuf::Message &protobufMsg, const PayloadType &type);
};

}   // namespace Network

}   // namespace UnderStory
