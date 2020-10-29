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

namespace UnderStory {

class AppBound {
 public:
    AppBound() {}
    explicit AppBound(Magnum::Platform::Application* app) {
       setupApp(app);
    }

    Magnum::Platform::Application* app() {
        return _app;
    }

    static void setupApp(Magnum::Platform::Application* app) {
        _app = app;
    }

 private:
    static inline Magnum::Platform::Application* _app = nullptr;
};

}  // namespace UnderStory
