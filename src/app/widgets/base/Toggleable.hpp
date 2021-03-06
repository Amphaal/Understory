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

#include <functional>

#include "src/app/widgets/base/EventHandlers.hpp"

namespace UnderStory {

namespace Widget {

class Toggleable {
 public:
    Toggleable() {}

    void toggle() {
        _toggled = !_toggled;
        _onToggled(_toggled);
    }

    bool isToggled() const {
        return _toggled;
    }

 protected:
    virtual void _onToggled(bool isToggled) {}

 private:
    bool _toggled = false;
};

class Button : public Toggleable, public MouseRelease_EH, public MousePress_EH {
 public:
    using ToggleCallback = std::function<void(bool)>;
    void setToogleCallback(ToggleCallback cb) {
        _toggleCb = cb;
    }

    bool isPreToggled() const {
        return _preToggling;
    }

 protected:
    void _onToggled(bool isToggled) override {
        if(_toggleCb) _toggleCb(isToggled);
    }

 private:
    bool _preToggling = false;
    ToggleCallback _toggleCb;

    void handleLockReleaseEvent(BasicEventHandler::EventType &event) final {
        _preToggling = false;
        toggle();
    }
        
    void handlePressEvent(BasicEventHandler::EventType &event) final {
        _preToggling = true;
    }
};

}  // namespace Widget

}  // namespace UnderStory
