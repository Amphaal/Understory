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

#include <spdlog/spdlog.h>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "src/core/AssetIntegrator.hpp"

//
// Test cases
//

// using namespace UnderStory::Network;

TEST_CASE("Test asset integration", "[assets]") {
    auto path = std::filesystem::absolute("./tests/resources/test.png");
    auto asset = UnderStory::AssetIntegrator::createAsset(path);

    REQUIRE(asset.file().name() == "test");
    REQUIRE(asset.file().fileextension() == ".png");
    REQUIRE(asset.file().fnv1ahash() == 9008380550126030887);
    REQUIRE(asset.file().contentlength() == 25579);
    REQUIRE(asset.dimensions().width() == 290);
    REQUIRE(asset.dimensions().height() == 300);
}
