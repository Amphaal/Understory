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

#include <cista/serialization.h>
#include <vector>

namespace UnderStory { namespace Network {

enum class Alteration {
    Unknown,
    Focused,
    Selected,
    Removed,
    Added,
    Reset,
    MetadataChanged,
    BulkMetadataChanged,
    AssetChanged,
    AtomTemplateChanged,
    AtomTemplateSelected,
    ToySelected,
    FogChanged,
    FogModeChanged,
    Replaced
};

enum class Interactor {
    Undefined,
    Local_MapLayout,
    Local_Map,
    Local_AtomEditor,
    Local_AtomDB,
    Local_ToysTV,
    RPZServer,
    RPZClient,
    Local_System
};

struct PayloadDataBase {
    uint8_t alterationType = 0;  // UnderStory::Network::Alteration
};

template<Alteration A>
struct PayloadData : public PayloadDataBase {};

template <Alteration A>
class Payload {
 public:
    Payload() {
        this->_payloadData.alterationType = static_cast<uint8_t>(A);
    }
    Alteration alterationType() const {
        return static_cast<Alteration>(this->_payloadData.alterationType);
    }

    const auto &data() const {
        return this->_payloadData;
    }

    bool networkHandled() const { return false; }  // virtual
    bool allowsRedoUndo() const { return false; }  // virtual
    bool triggersFogOfWarRecheck() const { return false; }  // virtual
    bool preventCachedAnimation() const { return false; }  // virtual

    auto serialize() const {
        return cista::serialize(this->data());
    }

    static auto deserialize(const std::vector<unsigned char> &buf) {
        return cista::deserialize<PayloadData<A>>(buf);
    }

    // virtual void updateModel() = 0;
    // virtual void updateView() = 0;

 private:
    PayloadData<A> _payloadData;
};

}  // namespace Network
}  // namespace UnderStory
