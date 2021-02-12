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

#include "Marshaller.h"

#include "src/models/User.pb.h"
#include "src/models/Heartbeat.pb.h"

using UnderStory::Network::RawPayload;
using UnderStory::Network::PayloadType;

RawPayload UnderStory::Network::Marshaller::_serialize(const google::protobuf::Message &protobufMsg, const PayloadType &type) {
    RawPayload payload;
    payload.type = type;
    protobufMsg.SerializePartialToString(&payload.bytes);
    payload.bytesSize = payload.bytes.length();
    return payload;
}

// allowed

template<>
RawPayload UnderStory::Network::Marshaller::serialize(const Handshake &protobufMsg) {
    return _serialize(protobufMsg, PayloadType::HANDSHAKE);
}

template<>
RawPayload UnderStory::Network::Marshaller::serialize(const Heartbeat &protobufMsg) {
    return _serialize(protobufMsg, PayloadType::HEARTBEAT);
}
