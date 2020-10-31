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

#include "BaseUIHelper.h"
#include "TimelineBound.h"

UnderStory::Animation::TimelineBound::TimelineBound(UnderStory::Animation::BaseUIHelper* animated) {
    _animated.push_back(animated);
}

Magnum::Timeline* UnderStory::Animation::TimelineBound::timeline() {
    return _timeline;
}

void UnderStory::Animation::TimelineBound::advance() {
    for(auto animated : _animated) animated->advance();
}

void UnderStory::Animation::TimelineBound::setupTimeline(Magnum::Timeline* timeline) {
    _timeline = timeline;
}
