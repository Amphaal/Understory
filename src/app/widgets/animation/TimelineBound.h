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

#include <vector>

#include <Magnum/Timeline.h>

namespace UnderStory {

namespace Animation {

class BaseUIHelper;

class TimelineBound {
 public:
    TimelineBound(Animation::BaseUIHelper* animated);

    Magnum::Timeline* timeline();

    static void advance();

    static void setupTimeline(Magnum::Timeline* timeline);

 private:
    static inline Magnum::Timeline* _timeline = nullptr;
    static inline std::vector<Animation::BaseUIHelper*> _animated;
};

}  // namespace Animation

}  // namespace UnderStory