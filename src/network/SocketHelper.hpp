// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

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

#include <spdlog/spdlog.h>

#include <string>
#include <memory>
#include <utility>
#include <future>

#include "src/models/User.pb.h"

#include "src/core/Defaults.hpp"

namespace UnderStory {

enum class PayloadType {
    PING = 0,
    HANDSHAKE = 1
};

struct RawPayload {
    PayloadType type;
    std::string bytes;
};

}   // namespace UnderStory