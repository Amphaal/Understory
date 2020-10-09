#pragma once

#include <Magnum/Math/Vector2.h>
#include <Corrade/Utility/Debug.h>

template<class V>
struct AnimationState {
    V from;
    V to;
    V current;
    V previous;
};
