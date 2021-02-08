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

#include <queue>
#include <mutex>

namespace UnderStory {

namespace Network {

template<class T>
class AtomicQueue : private std::queue<T> {
 public:
    // Mutexed push
    void push(const T& val) {
        std::unique_lock<std::mutex> lock(this->_m);
        std::queue<T>::push(val);
    }

    const T& front() {
        std::unique_lock<std::mutex> lock(this->_m);
        assert(!std::queue<T>::empty());
        return std::queue<T>::front();
    }

    // returns if queue is empty after pop
    bool pop() {
        std::unique_lock<std::mutex> lock(this->_m);
        assert(!std::queue<T>::empty());
        std::queue<T>::pop();
        return std::queue<T>::empty();
    }

 private:
    std::mutex _m;
};

}   // namespace Network

}   // namespace UnderStory
