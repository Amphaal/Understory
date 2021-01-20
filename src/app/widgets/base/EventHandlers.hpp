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

#include <Magnum/Platform/Sdl2Application.h>

#include "src/app/widgets/navigation/MouseState.hpp"

using Magnum::Platform::Sdl2Application;

namespace UnderStory {namespace Widget { class AppContainer; }}

namespace UnderStory {

namespace Widget {

class MouseStateBound {
 friend class AppContainer;
  protected:   
    static inline Navigation::MouseState _mouseState;
};

template<typename Ev>
class BasicEventHandler {
 public:
    typedef Ev EventType;
 
 protected:
    virtual void _handleEvent(Ev &event) = 0;
};

class Scroll_EH : public BasicEventHandler<Sdl2Application::MouseScrollEvent> {
      friend class AppContainer;
 protected:
    virtual void handleScrollEvent(BasicEventHandler::EventType &event) = 0;
 private:
    void _handleEvent(BasicEventHandler::EventType &event) final {
        return handleScrollEvent(event);
    }
};

class MousePress_EH : public MouseStateBound, public BasicEventHandler<Sdl2Application::MouseEvent> {
      friend class AppContainer;
 protected:
    virtual void handlePressEvent(BasicEventHandler::EventType &event) = 0; // pressed on element
 private:
    void _handleEvent(BasicEventHandler::EventType &event) final {
        return handlePressEvent(event);
    }
};

class MouseRelease_EH : public MouseStateBound, public BasicEventHandler<Sdl2Application::MouseEvent> {
      friend class AppContainer;
 protected:
    virtual void handleLockReleaseEvent(BasicEventHandler::EventType &event) = 0;  // when element was locked then released
 private:
    void _handleEvent(BasicEventHandler::EventType &event) final {
        return handleLockReleaseEvent(event);
    }
};

class MouseMove_EH : public MouseStateBound, public BasicEventHandler<Sdl2Application::MouseMoveEvent> {
      friend class AppContainer;
 protected:
    virtual void handleLockMoveEvent(BasicEventHandler::EventType &event) = 0;  // when locked element was moved
 private:
    void _handleEvent(BasicEventHandler::EventType &event) final {
        return handleLockMoveEvent(event);
    }
};

}  // namespace Widget

}  // namespace UnderStory