#pragma once

#include <string>

#include <magic_enum.hpp>

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

class Marshaller {
 public:
    static RawPayload serialize(const Handshake &handshake) {
        return _serialize(handshake, PayloadType::HANDSHAKE);
    }

 private:
    static RawPayload _serialize(const google::protobuf::Message &protobufMsg, const PayloadType &type) {
        RawPayload payload;
        payload.type = type;
        protobufMsg.SerializePartialToString(&payload.bytes);
        payload.bytesSize = payload.bytes.length();
        return payload;
    }
};

}   // namespace Network

}   // namespace UnderStory
