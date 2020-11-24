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

#include <Magnum/Platform/Sdl2Application.h>

using Magnum::Platform::Sdl2Application;

namespace UnderStory {namespace Widget { class AppContainer; }}

namespace UnderStory {

namespace Widget {

template<typename Ev>
class BasicEventHandler {
 public:
    typedef Ev EventType;
 
 protected:
    virtual void _handleEvent(Ev &event) = 0;
};

class ScrollEventHandler : public BasicEventHandler<Sdl2Application::MouseScrollEvent> {
  friend class AppContainer;
 protected:
    virtual void handleScrollEvent(BasicEventHandler::EventType &event) = 0;
 private:
    void _handleEvent(BasicEventHandler::EventType &event) final {
        return handleScrollEvent(event);
    }
};

}  // namespace Widget

}  // namespace UnderStory