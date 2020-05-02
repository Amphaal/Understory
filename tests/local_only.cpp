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

#include <chrono>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "src/network/UPnPHandler.hpp"
#include "src/app/Utility.hpp"

//
// Test cases
//

TEST_CASE("Test uPnP", "[network]") {
    spdlog::set_level(spdlog::level::debug);

    UnderStory::UPnPHandler handler(
        UnderStory::Utility::UPNP_DEFAULT_TARGET_PORT,
        UnderStory::Utility::UPNP_REQUEST_DESCRIPTION
    );

    auto request = handler.run();
    auto status = request.wait_for(std::chrono::seconds(5));

    REQUIRE(status != std::future_status::timeout);
    REQUIRE(handler.hasSucceded());
}
