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

#ifdef _DEBUG

    #include <cxxabi.h>

    #include <string>
    #include <memory>

    namespace UnderStory {

    namespace Debug {

        static std::string _demangle(char const* mangled) {
            auto ptr = std::unique_ptr<char, decltype(& std::free)>{
                abi::__cxa_demangle(mangled, nullptr, nullptr, nullptr),
                std::free
            };
            return {ptr.get()};
        }

        template<typename T>
        static std::string demanglePtr(T* ptr) {
            return _demangle(typeid(*ptr).name());
        }

    }   // namespace Debug

    }   // namespace UnderStory

#endif
